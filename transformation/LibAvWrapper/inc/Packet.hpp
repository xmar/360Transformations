#pragma once

extern "C"
{
    #include <libavformat/avformat.h>
}

namespace IMT
{
namespace LibAv
{
class Packet
{
    public:
        Packet(void): m_pkt(), m_isInit(false) {};
        ~Packet(void);

        AVPacket& GetPkt(void) {return m_pkt;}
    protected:

    private:
        friend class VideoWriter;
        friend class VideoReader;

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
        /**
        *  After a successfull SetAvPacketWithEncoder try to get other packet from the encoder
        */
        bool GetNextPacketAfterSentFrame(AVCodecContext* context);
        int GetNextPacket(AVFormatContext* fmt_ctx);
        void Free(void);
};
}
}
