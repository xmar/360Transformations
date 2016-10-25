#include "VideoWriter.hpp"
#include <stdexcept>


using namespace IMT::LibAv;

VideoWriter::VideoWriter(const std::string& outputFileName): m_outputFileName(outputFileName),  m_fmt_ctx(NULL),
 m_codec_ctx(), m_vstream(), m_isInit(false), m_lastFramesQueue(), m_pts(0)
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
        for (auto* codec_ctx: m_codec_ctx)
        {
            avcodec_close(codec_ctx);
        }
        //av_free(m_vstream);
        avformat_close_input(&m_fmt_ctx);
        avformat_free_context(m_fmt_ctx);
        av_free(m_fmt_ctx);
        m_fmt_ctx = nullptr;
        m_vstream.clear();
        m_codec_ctx.clear();

        // for(auto& queue: m_lastFramesQueue)
        // {
        //    while(!queue.empty())
        //    {
        //      auto* frame = queue.front();
        //      queue.pop();
        //      av_freep(&frame->data[0]);
        //      av_frame_free(&frame);
        //    }
        // }
        // m_lastFramesQueue.clear();
    }
}


VideoWriter& VideoWriter::operator<<(const cv::Mat& pict)
{
    Write(pict, 0);
    return *this;
}

void VideoWriter::Write(const cv::Mat& pict, int streamId)
{
    EncodeAndWrite(pict, streamId);
    // if (sharedPkt != nullptr)
    // {
    //     PrivateWrite(sharedPkt, streamId);
    // }
}

void VideoWriter::Flush(int streamId)
{
    EncodeAndWrite(nullptr, streamId);
    // while (sharedPkt != nullptr)
    // {
    //     PrivateWrite(sharedPkt, streamId);
    //     sharedPkt = nullptr;//Encode(nullptr, streamId);
    // }
}

// void VideoWriter::PrivateWrite(std::shared_ptr<Packet> sharedPkt, int streamId)
// {
//     auto& pkt = sharedPkt->GetPkt();
//     if (pkt.pts != AV_NOPTS_VALUE)
//     {
//         pkt.pts = av_rescale_q(pkt.pts, m_codec_ctx[streamId]->time_base, m_vstream[streamId]->time_base);
//     }
//     if (pkt.dts != AV_NOPTS_VALUE)
//     {
//         pkt.dts = av_rescale_q(pkt.dts, m_codec_ctx[streamId]->time_base, m_vstream[streamId]->time_base);
//     }
//     pkt.stream_index = m_vstream[streamId]->index;
//     PRINT_DEBUG_VideoWrite("Start writing packet")
//     if (av_interleaved_write_frame(m_fmt_ctx, &pkt) < 0)
//     {
//         throw std::runtime_error("Error while writing pkt");
//     }
//     PRINT_DEBUG_VideoWrite("Writing done")
//     if (sharedPkt->GetNextPacketAfterSentFrame(m_codec_ctx[streamId]))
//     {
//       PRINT_DEBUG_VideoWrite("New packet received")
//       PrivateWrite(sharedPkt, streamId);
//     }
// }

void VideoWriter::EncodeAndWrite(const cv::Mat& pict, int streamId)
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
    auto* convert_ctx = sws_getContext(frame->width, frame->height, (enum AVPixelFormat)src_pix_fmt, frame->width, frame->height, (enum AVPixelFormat)frame->format, SWS_FAST_BILINEAR, NULL, NULL, NULL);
    sws_scale(convert_ctx, src.data, src.linesize, 0, frame->height, frame->data, frame->linesize);

    sws_freeContext(convert_ctx);
    PRINT_DEBUG_VideoWrite("Encode: frame generated")
    EncodeAndWrite(frame, streamId);
}

void VideoWriter::EncodeAndWrite(AVFrame* frame, int streamId)
{
    AVPacket pkt;
    PRINT_DEBUG_VideoWrite("1# run coding")
    int ret = avcodec_send_frame(m_codec_ctx[streamId], frame);
    PRINT_DEBUG_VideoWrite("1# done")
    if (ret < 0)
    {
        throw std::runtime_error("Error while encoding frame");
    }
    if (ret == 0) //Coding successfull
    {
        PRINT_DEBUG_VideoWrite("1# ask codec for received pkt")
        while(true)
        {

          PRINT_DEBUG_VideoWrite("1# init pkt")
          av_init_packet(&pkt);
          pkt.data = nullptr;
          pkt.size = 0;

          ret = avcodec_receive_packet(m_codec_ctx[streamId], &pkt);
          if (ret != AVERROR(EAGAIN) && ret >= 0) //received an encoded pkt
          {
              PRINT_DEBUG_VideoWrite("1# received pkt")
              if (pkt.pts != AV_NOPTS_VALUE)
              {
                  pkt.pts = av_rescale_q(pkt.pts, m_codec_ctx[streamId]->time_base, m_vstream[streamId]->time_base);
              }
              if (pkt.dts != AV_NOPTS_VALUE)
              {
                  pkt.dts = av_rescale_q(pkt.dts, m_codec_ctx[streamId]->time_base, m_vstream[streamId]->time_base);
              }
              pkt.stream_index = m_vstream[streamId]->index;
              PRINT_DEBUG_VideoWrite("Start writing packet")
              if (av_interleaved_write_frame(m_fmt_ctx, &pkt) < 0)
              {
                  throw std::runtime_error("Error while writing pkt");
              }
              PRINT_DEBUG_VideoWrite("Writing done")

              av_packet_unref(&pkt); //The pkt is not usefull anymore
          }
          else // No pkt left
          {
            PRINT_DEBUG_VideoWrite("1# no received pkt ")
            av_packet_unref(&pkt);
            break;
          }
        }
    }
    //Now the frame is not use full anymore
    if (frame != nullptr)
    {
      av_frame_unref(frame);
      av_freep(&frame->data[0]);
      av_frame_free(&frame);
    }
}
