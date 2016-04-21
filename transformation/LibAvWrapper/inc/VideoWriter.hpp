/**
 * Author: Xavier Corbillon
 * Wrapper for libav to write a video file
 */
#pragma once

extern "C"
{
    #include <libavformat/avformat.h>
    #include "libavutil/pixfmt.h"
}

#include "Packet.hpp"

#include <string>
#include <exception>
#include <memory>

namespace IMT
{
namespace LibAv
{
    class VideoException;

    class VideoReader;

    class VideoWriter
    {
        public:
            VideoWriter(const std::string& outputFileName);
            VideoWriter(VideoWriter&& vw);
            VideoWriter& operator=(VideoWriter&& vw);
            ~VideoWriter(void);

            void Init(std::string codecName, unsigned width, unsigned height, unsigned fps, unsigned gop_size, unsigned bit_rate);

            VideoWriter& operator<<(const cv::Mat& pict);

            void Flush(void);

        private:
            std::string m_outputFileName;
            AVFormatContext* m_fmt_ctx;
            AVCodecContext* m_codec_ctx;
            AVStream* m_vstream;
            unsigned m_pts;

            bool m_isInit;

            VideoWriter(const VideoWriter& vw) = delete;
            VideoWriter& operator=(const VideoWriter& vw) = delete;

            std::shared_ptr<Packet> Encode(const cv::Mat& pict);
            std::shared_ptr<Packet> Encode(AVFrame* frame);
            void PrivateWrite(std::shared_ptr<Packet> sharedPkt);
    };
}
}
