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
extern "C"
{
   #include <libavutil/imgutils.h>
//   #include <libavutil/parseutils.h>
   #include <libswscale/swscale.h>
}

#define DEBUG_VideoWrite 1
#if DEBUG_VideoWrite
#define PRINT_DEBUG_VideoWrite(s) std::cout << s << std::endl;
#else
#define PRINT_DEBUG_VideoWrite(s) {}
#endif // DEBUG_VideoWrite

#include "Packet.hpp"

#include <string>
#include <exception>
#include <memory>
#include <array>
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

            template<int nbStreams>
            void Init(std::string codecName, std::array<unsigned, nbStreams>  widthVect, std::array<unsigned, nbStreams> heightVect, unsigned fps, unsigned gop_size, std::array<unsigned, nbStreams> bit_rateVect)
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
                AVOutputFormat* outformat = nullptr;
                outformat = av_guess_format(0, m_outputFileName.c_str(), 0);
                if (!outformat)
                {
                    throw std::runtime_error("Cannot guess output format from file name: "+m_outputFileName);
                }

                m_fmt_ctx->oformat = outformat;

                for (unsigned i = 0; i < nbStreams; ++i)
                {
                    const auto& bit_rate = bit_rateVect[i];
                    const auto& width = widthVect[i];
                    const auto& height = heightVect[i];
                    //Get the codec
                    PRINT_DEBUG_VideoWrite("Find the codec by name")
                    AVCodec* codec = avcodec_find_encoder_by_name(codecName.c_str());
                    if (!codec)
                    {
                        throw std::runtime_error("Codec"+codecName+"not found");
                    }

                    //New video stream
                    PRINT_DEBUG_VideoWrite("Create the new video stream")
                    m_vstream.push_back(avformat_new_stream(m_fmt_ctx, codec));
                    unsigned id = m_vstream.size()-1;
                    m_vstream[id]->id = id;
                    m_codec_ctx.push_back(m_vstream[id]->codec);
                    avcodec_get_context_defaults3(m_codec_ctx[id], codec);
                    m_codec_ctx[id]->bit_rate = bit_rate;
                    m_codec_ctx[id]->sample_aspect_ratio.num = 1;
                    m_codec_ctx[id]->sample_aspect_ratio.den = 1;
                    m_codec_ctx[id]->width = width;
                    m_codec_ctx[id]->height = height + height%2;
                    m_codec_ctx[id]->time_base.num = 1;
                    m_codec_ctx[id]->time_base.den = fps;
                    m_codec_ctx[id]->gop_size = gop_size;
                    m_codec_ctx[id]->pix_fmt = AV_PIX_FMT_YUV420P;
                    m_codec_ctx[id]->max_b_frames = 2;
                    m_vstream[id]->time_base.num = 1;
                    m_vstream[id]->time_base.den = fps;


                    //Open codec
                    PRINT_DEBUG_VideoWrite("Open the codec")
                    AVDictionary* voptions = nullptr;
                    //av_dict_set(&voptions, "profile", "baseline", 0);
                    if (avcodec_open2(m_codec_ctx[id], codec, nullptr/*&voptions*/) < 0)
                    {
                        throw std::runtime_error("Cannot open "+codecName);
                    }
                    outformat->video_codec = codec->id;
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

                PRINT_DEBUG_VideoWrite("Write header")
                if (m_fmt_ctx->oformat->flags & AVFMT_GLOBALHEADER)
                {
                    for (auto& codec_ctx: m_codec_ctx)
                    {
                        codec_ctx->flags |= CODEC_FLAG_GLOBAL_HEADER;
                    }
                }

                auto ret = avformat_write_header(m_fmt_ctx, NULL);
                if (ret < 0)
                {
                    throw std::runtime_error("Error occurred when opening output file");
                }
            }

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
