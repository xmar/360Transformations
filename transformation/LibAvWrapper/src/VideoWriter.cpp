#include "VideoWriter.hpp"
#include <stdexcept>

#define DEBUG_VideoWrite 0
#if DEBUG_VideoWrite
#define PRINT_DEBUG_VideoWrite(s) std::cout << s << std::endl;
#else
#define PRINT_DEBUG_VideoWrite(s) {}
#endif // DEBUG_VideoWrite


extern "C"
{
   #include <libavutil/imgutils.h>
//   #include <libavutil/parseutils.h>
   #include <libswscale/swscale.h>
}

using namespace IMT::LibAv;

VideoWriter::VideoWriter(const std::string& outputFileName): m_outputFileName(outputFileName),  m_fmt_ctx(NULL),
 m_codec_ctx(nullptr), m_vstream(nullptr), m_isInit(false), m_pts(0)
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
        Flush();
        m_isInit = false;
        av_write_trailer(m_fmt_ctx);
        avcodec_close(m_codec_ctx);
        //av_free(m_vstream);
        avformat_close_input(&m_fmt_ctx);
        av_free(m_fmt_ctx);
        m_fmt_ctx = nullptr;
        m_vstream = nullptr;
        m_codec_ctx = nullptr;
    }
}

void VideoWriter::Init(std::string codecName, unsigned width, unsigned height, unsigned fps, unsigned gop_size, unsigned bit_rate)
{
    //av_log_set_level(AV_LOG_DEBUG);
    PRINT_DEBUG_VideoWrite("Start init video writer")
    if (m_isInit)
    {
        return;
    }
    avformat_alloc_output_context2(&m_fmt_ctx, NULL, NULL, m_outputFileName.c_str());
    if (!m_fmt_ctx)
    {
        throw std::runtime_error("Coulnt allocate output video "+m_outputFileName);
    }

    //Detect output container
    PRINT_DEBUG_VideoWrite("Detect container")
    AVOutputFormat *outformat = nullptr;
    outformat = av_guess_format(0, m_outputFileName.c_str(), 0);
    if (!outformat)
    {
        throw std::runtime_error("Cannot guess output format from file name: "+m_outputFileName);
    }

    m_fmt_ctx->oformat = outformat;

    //Get the codec
    PRINT_DEBUG_VideoWrite("Find the codec by name")
    AVCodec* codec = avcodec_find_encoder_by_name(codecName.c_str());
    if (!codec)
    {
        throw std::runtime_error("Codec"+codecName+"not found");
    }

    //New video stream
    PRINT_DEBUG_VideoWrite("Create the new video stream")
    m_vstream = avformat_new_stream(m_fmt_ctx, codec);
    m_codec_ctx = m_vstream->codec;
    m_codec_ctx->bit_rate = bit_rate;
    m_codec_ctx->sample_aspect_ratio.num = 1;
    m_codec_ctx->sample_aspect_ratio.den = 1;
    m_codec_ctx->width = width;
    m_codec_ctx->height = height + height%2;
    m_codec_ctx->time_base.num = 1;
    m_codec_ctx->time_base.den = fps;
    m_codec_ctx->gop_size = gop_size;
    m_codec_ctx->pix_fmt = AV_PIX_FMT_YUV420P;
    m_vstream->time_base.num = 1;
    m_vstream->time_base.den = fps;


    //Open codec
    PRINT_DEBUG_VideoWrite("Open the codec")
    AVDictionary* voptions = nullptr;
    //av_dict_set(&voptions, "profile", "baseline", 0);
    if (avcodec_open2(m_codec_ctx, codec, nullptr/*&voptions*/) < 0)
    {
        throw std::runtime_error("Cannot open "+codecName);
    }

    //Open output file:
    PRINT_DEBUG_VideoWrite("Open the file")
    av_dump_format(m_fmt_ctx, 0, m_outputFileName.c_str(), 1);
    if (!(m_fmt_ctx->oformat->flags & AVFMT_NOFILE))
    {
        if (avio_open(&m_fmt_ctx->pb, m_outputFileName.c_str(), AVIO_FLAG_WRITE) < 0)
        {
            throw std::runtime_error("Could not open output file "+m_outputFileName);
        }
    }

    outformat->video_codec = codec->id;

    PRINT_DEBUG_VideoWrite("Write header")
    if (m_fmt_ctx->oformat->flags & AVFMT_GLOBALHEADER)
    {
         m_codec_ctx->flags |= CODEC_FLAG_GLOBAL_HEADER;
    }

    auto ret = avformat_write_header(m_fmt_ctx, NULL);
    if (ret < 0)
    {
        throw std::runtime_error("Error occurred when opening output file");
    }
}

VideoWriter& VideoWriter::operator<<(const cv::Mat& pict)
{
    auto sharedPkt = Encode(pict);
    if (sharedPkt != nullptr)
    {
        PrivateWrite(sharedPkt);
    }
}

void VideoWriter::Flush(void)
{
    auto sharedPkt = Encode(nullptr);
    while (sharedPkt != nullptr)
    {
        PrivateWrite(sharedPkt);
        sharedPkt = Encode(nullptr);
    }
}

void VideoWriter::PrivateWrite(std::shared_ptr<Packet> sharedPkt)
{
    auto& pkt = sharedPkt->GetPkt();
    if (pkt.pts != AV_NOPTS_VALUE)
    {
        pkt.pts = av_rescale_q(pkt.pts, m_codec_ctx->time_base, m_vstream->time_base);
    }
    if (pkt.dts != AV_NOPTS_VALUE)
    {
        pkt.dts = av_rescale_q(pkt.dts, m_codec_ctx->time_base, m_vstream->time_base);
    }
    pkt.stream_index = m_vstream->index;
    if (av_interleaved_write_frame(m_fmt_ctx, &pkt) < 0)
    {
        throw std::runtime_error("Error while writing pkt");
    }
}

std::shared_ptr<Packet> VideoWriter::Encode(const cv::Mat& pict)
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
    frame->width = m_codec_ctx->width;
    frame->height = m_codec_ctx->height;
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
    return Encode(frame);
}

std::shared_ptr<Packet> VideoWriter::Encode(AVFrame* frame)
{
    auto pkt = std::make_shared<Packet>();
    PRINT_DEBUG_VideoWrite("Get pkt")
    if (pkt->SetAvPacketWithEncoder(m_codec_ctx, frame))
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
