#include "Packet.hpp"
#include <stdexcept>

#define DEBUG_Packet 0
#if DEBUG_Packet
#define PRINT_DEBUG_Packet(s) std::cout << s << std::endl;
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
    PRINT_DEBUG_Packet("Free pkt")
    Free();
    int got_output;
    PRINT_DEBUG_Packet("init pkt")
    av_init_packet(&m_pkt);
    m_pkt.data = nullptr;
    m_pkt.size = 0;
    PRINT_DEBUG_Packet("run coding")
    int ret = avcodec_encode_video2(context, &m_pkt, frame, &got_output);
    PRINT_DEBUG_Packet("done")
    if (ret < 0)
    {
        throw std::runtime_error("Error while encoding frame");
    }
    if (got_output)
    {
        m_isInit = true;
    }
    else
    {
        PRINT_DEBUG_Packet("free pkt")
        av_packet_unref(&m_pkt);
    }
    return m_isInit;
}
