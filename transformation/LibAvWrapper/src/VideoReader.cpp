#include "VideoReader.hpp"

#include <iostream>
#include <Packet.hpp>

#define DEBUG_VideoReader 0
#if DEBUG_VideoReader
#define PRINT_DEBUG_VideoReader(s) std::cout << s << std::endl;
#else
#define PRINT_DEBUG_VideoReader(s) {}
#endif // DEBUG_VideoReader

extern "C"
{
   #include <libavutil/imgutils.h>
//   #include <libavutil/parseutils.h>
   #include <libswscale/swscale.h>
}

using namespace IMT::LibAv;

VideoReader::VideoReader(std::string inputPath): m_inputPath(inputPath), m_fmt_ctx(nullptr), m_videoStreamIds(),
    m_wasDecoded(false), m_outputFrames(), m_streamIdToVecId()
{
    //ctor
}

VideoReader::~VideoReader()
{
    //dtor
}

void printA(AVFormatContext* _a)
{
std::cout << "duration    " << (unsigned long)_a->duration << "\n";
std::cout << "streams     " << _a->nb_streams << "\n";
unsigned nbVideo = 0;
for (unsigned i = 0; i < _a->nb_streams; ++i)
{
    if(_a->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
    {
        nbVideo++;
    }
}
std::cout << "vid stream  " << nbVideo << "\n";
std::cout << "format name " << _a->iformat->name << "\n";
std::cout << "bit_rate    " << _a->bit_rate << "\n";
std::cout << "long name   " << _a->iformat->long_name << "\n";
}

void VideoReader::Init(unsigned nbFrames)
{
    PRINT_DEBUG_VideoReader("Register codecs");
    av_register_all();

    int ret = 0;

//    uint8_t* buffer = nullptr;
//    size_t buffer_size;
//    int ret = av_file_map(m_inputPath.c_str(), &buffer, &buffer_size, 0, nullptr);
//    if (ret < 0)
//    {
//        std::cout << "Error while slurping the file content into the buffer" << std::endl;
//    }

//    struct buffer_data bd = { 0 };
//    bd.ptr  = buffer;
//    bd.size = buffer_size;

    PRINT_DEBUG_VideoReader("Allocate format context");
    if (!(m_fmt_ctx = avformat_alloc_context())) {
        ret = AVERROR(ENOMEM);
        std::cout << "Error while allocating the format context" << std::endl;
    }

    PRINT_DEBUG_VideoReader("Open format context: "<<m_inputPath);
    ret = avformat_open_input(&m_fmt_ctx, m_inputPath.c_str(), nullptr, nullptr);
    if (ret < 0) {
        std::cout << "Could not open input" << std::endl;
    }

    PRINT_DEBUG_VideoReader("Find streams info");
    ret = avformat_find_stream_info(m_fmt_ctx, nullptr);
    if (ret < 0) {
        std::cout << "Could not find stream information" << std::endl;
    }

    PRINT_DEBUG_VideoReader("Dump format context");
    av_dump_format(m_fmt_ctx, 0, m_inputPath.c_str(), 0);

    printA(m_fmt_ctx);

    PRINT_DEBUG_VideoReader("Init video stream decoders");
    for (unsigned i = 0; i < m_fmt_ctx->nb_streams; ++i)
    {
        if(m_fmt_ctx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
        {
            m_outputFrames.emplace_back();
            m_streamIdToVecId[i] = m_videoStreamIds.size();
            m_videoStreamIds.push_back(i);
            auto* decoder = avcodec_find_decoder(m_fmt_ctx->streams[i]->codec->codec_id);
            if(!decoder)
            {
                std::cout << "Could not find the decoder for stream id " << i << std::endl;
            }
            PRINT_DEBUG_VideoReader("Init decoder for stream id " << i);
            if ((ret = avcodec_open2(m_fmt_ctx->streams[i]->codec, decoder, nullptr)) < 0)
            {
                std::cout << "Could not open the decoder for stream id " << i << std::endl;
            }
        }
    }
    DecodeAllFrames(nbFrames);
}

static bool AllDone(const std::vector<bool>& vect)
{
    bool r = true;
    for (auto b: vect)
    {
        r &= b;
    }
    return r;
}

static std::shared_ptr<cv::Mat> ToMat(AVCodecContext* codecCtx, AVFrame* frame_ptr)
{
    int w = codecCtx->width;
    int h = codecCtx->height;
    struct SwsContext *convert_ctx;
    convert_ctx = sws_getContext(w, h, codecCtx->pix_fmt, w, h, AV_PIX_FMT_BGR24, SWS_FAST_BILINEAR,
        NULL, NULL, NULL);
    if(convert_ctx == NULL)
    {
        std::cout << "Cannot initialize the conversion context!" << std::endl;
    }
    AVFrame* frame_ptr2 = av_frame_alloc();
    av_image_alloc(frame_ptr2->data, frame_ptr2->linesize, w, h, AV_PIX_FMT_BGR24, 1);
    sws_scale(convert_ctx, frame_ptr->data, frame_ptr->linesize, 0, h, frame_ptr2->data, frame_ptr2->linesize);
    cv::Mat mat(codecCtx->height, codecCtx->width, CV_8UC3, frame_ptr2->data[0], frame_ptr2->linesize[0]);

    auto returnMat = std::make_shared<cv::Mat>(mat.clone());
    av_frame_free(&frame_ptr2);
    return returnMat;
}

void VideoReader::DecodeAllFrames(unsigned nbFrames)
{
    PRINT_DEBUG_VideoReader("Decode all video frames");
    Packet pkt;
    std::vector<bool> doneVect(m_outputFrames.size(), false);
    std::vector<bool> gotOne(m_outputFrames.size(), false);
    while ( pkt.GetNextPacket(m_fmt_ctx) >= 0 )//otherwise no packet left to read
    {
        unsigned streamId = pkt.GetPkt().stream_index;
        if (m_streamIdToVecId.count(streamId) > 0 && !doneVect[m_streamIdToVecId[streamId]])
        { //then the pkt belong to a stream we care about.
            PRINT_DEBUG_VideoReader("Got a pkt for streamId "<<streamId)
            AVFrame* frame_ptr = av_frame_alloc();
            const AVPacket* const packet_ptr = &pkt.GetPkt();
            int got_a_frame = 0;
            auto* codecCtx = m_fmt_ctx->streams[streamId]->codec;
            int ret = avcodec_decode_video2(codecCtx, frame_ptr, &got_a_frame, packet_ptr);
            if (got_a_frame)
            {
                gotOne[m_streamIdToVecId[streamId]] = true;
                m_outputFrames[m_streamIdToVecId[streamId]].push(ToMat(codecCtx, frame_ptr));
                av_frame_free(&frame_ptr);
            }
            doneVect[m_streamIdToVecId[streamId]] = (gotOne[m_streamIdToVecId[streamId]] && (!got_a_frame)) || (m_outputFrames[m_streamIdToVecId[streamId]].size() >= nbFrames);
        }
    }
    unsigned streamVectId = 0;
    AVPacket pktNull;
    av_init_packet(&pktNull);
    pktNull.data = nullptr;
    pktNull.size = 0;
    while(!AllDone(doneVect))
    {
        if (!doneVect[streamVectId])
        {
            PRINT_DEBUG_VideoReader("Flush streamVectId "<<streamVectId)
            AVFrame* frame_ptr = av_frame_alloc();
            const AVPacket* const packet_ptr = &pktNull;
            int got_a_frame = 0;
            auto* codecCtx = m_fmt_ctx->streams[m_videoStreamIds[streamVectId]]->codec;
            int ret = avcodec_decode_video2(m_fmt_ctx->streams[m_videoStreamIds[streamVectId]]->codec, frame_ptr, &got_a_frame, packet_ptr);
            if (got_a_frame)
            {
                m_outputFrames[streamVectId].push(ToMat(codecCtx, frame_ptr));
                //m_outputFrames[streamVectId].emplace();
                av_frame_free(&frame_ptr);
            }
            doneVect[streamVectId] = (!got_a_frame) || (m_outputFrames[streamVectId].size() >= nbFrames);
            streamVectId = (streamVectId + 1) % m_outputFrames.size();
        }
    }

    for (auto i = 0; i <  m_outputFrames.size(); ++i)
    {
        PRINT_DEBUG_VideoReader(m_outputFrames[i].size() << " frames decoded from stream vect id "<<i)
    }
    m_wasDecoded = true;
}

std::shared_ptr<cv::Mat> VideoReader::GetNextPicture(unsigned streamId)
{
    if (m_wasDecoded && streamId < m_outputFrames.size())
    {
        if (!m_outputFrames[streamId].empty())
        {
            auto matPtr = m_outputFrames[streamId].front();
            m_outputFrames[streamId].pop();
            return matPtr;
        }
    }
    return nullptr;

}
