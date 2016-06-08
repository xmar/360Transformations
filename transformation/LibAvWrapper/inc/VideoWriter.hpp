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
#include <opencv2/opencv.hpp>

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

            void Init(std::string codecName, unsigned width, unsigned height, unsigned fps, unsigned gop_size, std::vector<unsigned> bit_rate);

            VideoWriter& operator<<(const cv::Mat& pict);
            void Write(const cv::Mat& pict, int streamId);

            void Flush(int streamId);

        private:
            std::string m_outputFileName;
            AVFormatContext* m_fmt_ctx;
            std::vector<AVCodecContext*> m_codec_ctx;
            std::vector<AVStream*> m_vstream;
            unsigned m_pts;

            bool m_isInit;

            VideoWriter(const VideoWriter& vw) = delete;
            VideoWriter& operator=(const VideoWriter& vw) = delete;

            std::shared_ptr<Packet> Encode(const cv::Mat& pict, int streamId);
            std::shared_ptr<Packet> Encode(AVFrame* frame, int streamId);
            void PrivateWrite(std::shared_ptr<Packet> sharedPkt, int streamId);
    };
}
}
