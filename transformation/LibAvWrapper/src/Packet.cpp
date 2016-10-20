#include "Packet.hpp"
#include <stdexcept>

#define DEBUG_Packet 0
#if DEBUG_Packet
#include <iostream>
#define PRINT_DEBUG_Packet(s) {std::cout << s << std::endl;}
#else
#define PRINT_DEBUG_Packet(s) {}
#endif // DEBUG_VideoWrite

using namespace IMT::LibAv;

Packet::~Packet()
{
    Free();
}

void Packet::Free(void)
{
    if (m_isInit)
    {
        PRINT_DEBUG_Packet("free pkt -- start")
        m_isInit = false;
        av_packet_unref(&m_pkt);
        PRINT_DEBUG_Packet("free pkt -- done")
    }
}

bool Packet::SetAvPacketWithEncoder(AVCodecContext* context, AVFrame* frame)
{
    PRINT_DEBUG_Packet("1# Free pkt")
    Free();
    PRINT_DEBUG_Packet("1# init pkt")
    av_init_packet(&m_pkt);
    m_pkt.data = nullptr;
    m_pkt.size = 0;
    PRINT_DEBUG_Packet("1# run coding")
    //int ret = avcodec_encode_video2(context, &m_pkt, frame, &got_output);
    int ret = avcodec_send_frame(context, frame);
    PRINT_DEBUG_Packet("1# done")
    if (ret < 0)
    {
        throw std::runtime_error("Error while encoding frame");
    }
    if (ret == 0)
    {
        PRINT_DEBUG_Packet("1# ask codec for received pkt")
        ret = avcodec_receive_packet(context, &m_pkt);
        if (ret != AVERROR(EAGAIN))
        {
            PRINT_DEBUG_Packet("1# received pkt")
            m_isInit = true;
        }
        else
        {
          PRINT_DEBUG_Packet("1# no received pkt")
          av_packet_unref(&m_pkt);
        }
    }
    else
    {
        PRINT_DEBUG_Packet("1# free pkt")
        //av_packet_unref(&m_pkt);
    }
    return m_isInit;
}

bool Packet::GetNextPacketAfterSentFrame(AVCodecContext* context)
{
  PRINT_DEBUG_Packet("2# free pkt")
  Free();
  av_init_packet(&m_pkt);
  PRINT_DEBUG_Packet("2# init pkt")
  m_pkt.data = nullptr;
  m_pkt.size = 0;
  PRINT_DEBUG_Packet("2# ask codec for received pkt")
  int ret = avcodec_receive_packet(context, &m_pkt);
  if (ret != AVERROR(EAGAIN))
  {
      PRINT_DEBUG_Packet("2# received pkt")
      m_isInit = true;
  }
  else
  {
    PRINT_DEBUG_Packet("2# no received pkt")
    av_packet_unref(&m_pkt);
  }
  return m_isInit;
}

int Packet::GetNextPacket(AVFormatContext* fmt_ctx)
{

    if (fmt_ctx == nullptr) return -1;
    Free();
    int ret = -1;
    if ((ret = av_read_frame(fmt_ctx, &m_pkt)) >= 0 )
    {
        m_isInit = true;
    }
    return ret;
}
