#pragma once

extern "C"
{
    #include <libavformat/avformat.h>
}
#include <opencv2/opencv.hpp>

namespace IMT
{
namespace LibAv
{
class VideoEncoder;
class Packet
{
    public:
        Packet(void): m_pkt(), m_isInit(false) {};
        ~Packet(void);

        AVPacket& GetPkt(void) {return m_pkt;}
    protected:

    private:
        friend class VideoWriter;

        AVPacket m_pkt;
        bool m_isInit;

        Packet(const Packet& p) = delete;
        Packet& operator=(const Packet& p) = delete;

        /** \brief Set the value of m_pkt given the encoder codecContext + the AvFrame to encode
         *
         * \param context AVCodecContext*
         * \return bool return true if a pkt was set, false otherwise
         *
         */
        bool SetAvPacketWithEncoder(AVCodecContext* context, AVFrame* frame);
        void Free(void);
};
}
}
