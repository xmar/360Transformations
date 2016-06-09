#pragma once
#include <string>
#include <vector>
#include <queue>
#include <memory>

#include <opencv2/opencv.hpp>

extern "C"
{
    #include <libavcodec/avcodec.h>
    #include <libavformat/avformat.h>
    #include <libavformat/avio.h>
    #include <libavutil/file.h>
}

namespace IMT {
namespace LibAv {

class VideoReader
{
    public:
        VideoReader(std::string inputPath);
        VideoReader(const VideoReader&) = delete;
        VideoReader& operator=(const VideoReader&) = delete;

        virtual ~VideoReader(void);

        void Init(unsigned nbFrames);

        std::shared_ptr<cv::Mat> GetNextPicture(unsigned streamId);

        unsigned GetNbStream(void) const {return m_videoStreamIds.size();}

    protected:

    private:
        std::string m_inputPath;
        AVFormatContext* m_fmt_ctx;
        std::vector<unsigned int> m_videoStreamIds;
        std::map<unsigned int, unsigned int> m_streamIdToVecId;
        //First version: we totaly decode the video and store in a vector the output frames
        bool m_wasDecoded;
        std::vector<std::queue<std::shared_ptr<cv::Mat>>> m_outputFrames;

        void DecodeAllFrames(unsigned nbFrames);
};
}
}
