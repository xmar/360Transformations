#include "VideoWriter.hpp"
#include <stdexcept>


using namespace IMT::LibAv;

VideoWriter::VideoWriter(const std::string& outputFileName): m_outputFileName(outputFileName),  m_fmt_ctx(NULL),
 m_codec_ctx(), m_vstream(), m_isInit(false), m_pts(0)
{}

VideoWriter::VideoWriter(VideoWriter&& vw): m_outputFileName(), m_fmt_ctx(NULL), m_isInit(false)
{
    std::swap(*this, vw);
}

VideoWriter& VideoWriter::operator=(VideoWriter&& vw)
{
    std::swap(*this, vw);
    return *this;
}

VideoWriter::~VideoWriter(void)
{
    if (!m_isInit)
    {
        for (auto& stream:  m_vstream)
        {
            Flush(stream->index);
        }

        m_isInit = false;
        av_write_trailer(m_fmt_ctx);
        for (auto& codec_ctx: m_codec_ctx)
        {
            avcodec_close(codec_ctx);
        }
        //av_free(m_vstream);
        avformat_close_input(&m_fmt_ctx);
        av_free(m_fmt_ctx);
        m_fmt_ctx = nullptr;
        m_vstream.clear();
        m_codec_ctx.clear();
    }
}


VideoWriter& VideoWriter::operator<<(const cv::Mat& pict)
{
    Write(pict, 0);
    return *this;
}

void VideoWriter::Write(const cv::Mat& pict, int streamId)
{
    auto sharedPkt = Encode(pict, streamId);
    if (sharedPkt != nullptr)
    {
        PrivateWrite(sharedPkt, streamId);
    }
}

void VideoWriter::Flush(int streamId)
{
    auto sharedPkt = Encode(nullptr, streamId);
    while (sharedPkt != nullptr)
    {
        PrivateWrite(sharedPkt, streamId);
        sharedPkt = Encode(nullptr, streamId);
    }
}

void VideoWriter::PrivateWrite(std::shared_ptr<Packet> sharedPkt, int streamId)
{
    auto& pkt = sharedPkt->GetPkt();
    std::cout << "TEST" << std::endl;
    if (pkt.pts != AV_NOPTS_VALUE)
    {
        pkt.pts = av_rescale_q(pkt.pts, m_codec_ctx[streamId]->time_base, m_vstream[streamId]->time_base);
    }
    if (pkt.dts != AV_NOPTS_VALUE)
    {
        pkt.dts = av_rescale_q(pkt.dts, m_codec_ctx[streamId]->time_base, m_vstream[streamId]->time_base);
    }
    pkt.stream_index = m_vstream[streamId]->index;
    std::cout << "StreamId = " << streamId << " stream_index = " << pkt.stream_index << std::endl;
    if (av_interleaved_write_frame(m_fmt_ctx, &pkt) < 0)
    {
        throw std::runtime_error("Error while writing pkt");
    }
}

std::shared_ptr<Packet> VideoWriter::Encode(const cv::Mat& pict, int streamId)
{
    PRINT_DEBUG_VideoWrite("Start Encode")
    AVFrame* frame = av_frame_alloc();
    //avcodec_get_frame_defaults(frame);

    AVFrame src;
    src.format = AV_PIX_FMT_BGR24;
    src.data[0] = pict.data;
    src.linesize[0] = pict.step;
    src.linesize[1] = 0;
    src.linesize[2] = 0;

    frame->format = AV_PIX_FMT_YUV420P;
    frame->width = m_codec_ctx[streamId]->width;
    frame->height = m_codec_ctx[streamId]->height;
    int buffSize = av_image_alloc(frame->data, frame->linesize, frame->width, frame->height, (enum AVPixelFormat)frame->format, 1);
//    frame->data[0] = pict.data;
//    frame->data[1] = pict.data;
//    frame->data[2] = pict.data;
//    frame->linesize[0] = m_codec_ctx->width;
//    frame->linesize[1] = m_codec_ctx->height;
//    frame->linesize[2] = 0;
    frame->pts = m_pts++;

    enum AVPixelFormat src_pix_fmt = AV_PIX_FMT_BGR24;
    auto convert_ctx = sws_getContext(frame->width, frame->height, (enum AVPixelFormat)src_pix_fmt, frame->width, frame->height, (enum AVPixelFormat)frame->format, SWS_FAST_BILINEAR, NULL, NULL, NULL);
    sws_scale(convert_ctx, src.data, src.linesize, 0, frame->height, frame->data, frame->linesize);


    PRINT_DEBUG_VideoWrite("Encode: frame generated")
    return Encode(frame, streamId);
}

std::shared_ptr<Packet> VideoWriter::Encode(AVFrame* frame, int streamId)
{
    auto pkt = std::make_shared<Packet>();
    PRINT_DEBUG_VideoWrite("Get pkt")
    if (pkt->SetAvPacketWithEncoder(m_codec_ctx[streamId], frame))
    {
        PRINT_DEBUG_VideoWrite("Encode: ok")
        av_frame_free(&frame);
        PRINT_DEBUG_VideoWrite("frame cleaned")
        return pkt;
    }
    else
    {
        PRINT_DEBUG_VideoWrite("Encode: no pkt")
        av_frame_free(&frame);
        PRINT_DEBUG_VideoWrite("frame cleaned")
        return nullptr;
    }
}
