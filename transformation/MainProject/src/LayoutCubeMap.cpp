#include "LayoutCubeMap.hpp"

#include <algorithm>
#include <iterator>

using namespace IMT;

unsigned int LayoutCubeMap::IStartOffset(LayoutCubeMapBased::Faces f) const
{
    switch(f)
    {
        case Faces::Front:
            return (m_maxOffsetCols[0]-GetRes(f))/2;
        case Faces::Back:
            return (m_maxOffsetCols[0]-GetRes(f))/2;
        case Faces::Right:
            return m_maxOffsetCols[0]+(m_maxOffsetCols[1]-GetRes(f))/2;
        case Faces::Left:
            return m_maxOffsetCols[0]+(m_maxOffsetCols[1]-GetRes(f))/2;
        case Faces::Top:
            return m_maxOffsetCols[0]+m_maxOffsetCols[1]+(m_maxOffsetCols[2]-GetRes(f))/2;
        case Faces::Bottom:
            return m_maxOffsetCols[0]+m_maxOffsetCols[1]+(m_maxOffsetCols[2]-GetRes(f))/2;
        case Faces::Last:
        case Faces::Black:
            std::invalid_argument("IStartOffset: Last is not a valid face");
    }
}
unsigned int LayoutCubeMap::IEndOffset(LayoutCubeMapBased::Faces f) const
{
    switch(f)
    {
        case Faces::Front:
            return m_maxOffsetCols[0]-(m_maxOffsetCols[0]-GetRes(f))/2;
        case Faces::Back:
            return m_maxOffsetCols[0]-(m_maxOffsetCols[0]-GetRes(f))/2;
        case Faces::Right:
            return m_maxOffsetCols[0]+m_maxOffsetCols[1]-(m_maxOffsetCols[1]-GetRes(f))/2;
        case Faces::Left:
            return m_maxOffsetCols[0]+m_maxOffsetCols[1]-(m_maxOffsetCols[1]-GetRes(f))/2;
        case Faces::Top:
            return m_maxOffsetCols[0]+m_maxOffsetCols[1]+m_maxOffsetCols[2]-(m_maxOffsetCols[2]-GetRes(f))/2;
        case Faces::Bottom:
            return m_maxOffsetCols[0]+m_maxOffsetCols[1]+m_maxOffsetCols[2]-(m_maxOffsetCols[2]-GetRes(f))/2;
        case Faces::Last:
        case Faces::Black:
            std::invalid_argument("IEndOffset: Last is not a valid face");
    }
}
unsigned int LayoutCubeMap::JStartOffset(LayoutCubeMapBased::Faces f) const
{
    switch(f)
    {
        case Faces::Front:
            return (m_maxOffsetRows[0]-GetRes(f))/2;
        case Faces::Back:
            return m_maxOffsetRows[0]+(m_maxOffsetRows[1]-GetRes(f))/2;
        case Faces::Right:
            return (m_maxOffsetRows[0]-GetRes(f))/2;;
        case Faces::Left:
            return m_maxOffsetRows[0]+(m_maxOffsetRows[1]-GetRes(f))/2;
        case Faces::Top:
            return (m_maxOffsetRows[0]-GetRes(f))/2;;
        case Faces::Bottom:
            return m_maxOffsetRows[0]+(m_maxOffsetRows[1]-GetRes(f))/2;
        case Faces::Last:
        case Faces::Black:
            std::invalid_argument("JStartOffset: Last is not a valid face");
    }
}
unsigned int LayoutCubeMap::JEndOffset(LayoutCubeMapBased::Faces f) const
{
    switch(f)
    {
        case Faces::Front:
            return m_maxOffsetRows[0]-(m_maxOffsetRows[0]-GetRes(f))/2;
        case Faces::Back:
            return m_maxOffsetRows[0]+m_maxOffsetRows[1]-(m_maxOffsetRows[1]-GetRes(f))/2;
        case Faces::Right:
            return m_maxOffsetRows[0]-(m_maxOffsetRows[0]-GetRes(f))/2;;
        case Faces::Left:
            return m_maxOffsetRows[0]+m_maxOffsetRows[1]-(m_maxOffsetRows[1]-GetRes(f))/2;
        case Faces::Top:
            return m_maxOffsetRows[0]-(m_maxOffsetRows[0]-GetRes(f))/2;;
        case Faces::Bottom:
            return m_maxOffsetRows[0]+m_maxOffsetRows[1]-(m_maxOffsetRows[1]-GetRes(f))/2;
        case Faces::Last:
        case Faces::Black:
            std::invalid_argument("JEndOffset: Last is not a valid face");
    }
}

LayoutCubeMapBased::Faces LayoutCubeMap::From2dToFace(unsigned int i, unsigned int j) const
{
    for(const auto& f: get_range<Faces>())
    {
        if (f != Faces::Last && f != Faces::Black && InFace(i,j,f))
        {
            return f;
        }
    }
    return Faces::Black;
}

Layout::NormalizedFaceInfo LayoutCubeMap::From2dToNormalizedFaceInfo(const CoordI& pixel) const
{
    auto f = From2dToFace(pixel.x, pixel.y);
    if (f == Faces::Black)
    {
        return Layout::NormalizedFaceInfo(CoordF(0,0), static_cast<int>(f));
    }
    double i = double(pixel.x - IStartOffset(f))/GetRes(f);
    double j = double(pixel.y - JStartOffset(f))/GetRes(f);
    return Layout::NormalizedFaceInfo(CoordF(i,j), static_cast<int>(f));
}

#define BORDER(x) (MAX(1.0, MIN(GetRes(f)-1,x)))

CoordF LayoutCubeMap::FromNormalizedInfoTo2d(const Layout::NormalizedFaceInfo& ni) const
{
    Faces f = static_cast<LayoutCubeMapBased::Faces>(ni.m_faceId);
    const CoordF& coord (ni.m_normalizedFaceCoordinate);
    if (f != Faces::Last && f != Faces::Black)
    {
        return CoordF(BORDER(GetRes(f)*coord.x)+IStartOffset(f), BORDER(GetRes(f)*coord.y)+JStartOffset(f));
    }
    else
    {
        throw std::invalid_argument("FromNormalizedInfoTo2d: Last is not a valid face");
    }
}

std::shared_ptr<Picture> LayoutCubeMap::ReadNextPictureFromVideoImpl(void)
{
    bool isInit = false;
    cv::Mat outputMat;
    for (unsigned i = 0; i < 6; ++i)
    {
        Faces f = static_cast<Faces>(i);
        cv::Rect roi( IStartOffset(f),  JStartOffset(f), GetRes(f), GetRes(f) );
        auto facePictPtr = m_inputVideoPtr->GetNextPicture(i);
        if (!isInit)
        {
            outputMat = cv::Mat( m_outHeight, m_outWidth, facePictPtr->type());
            isInit = true;
        }
        cv::Mat facePictMat ( outputMat, roi);
        facePictPtr->copyTo(facePictMat);
    }
    return std::make_shared<Picture>(outputMat);
}

void LayoutCubeMap::WritePictureToVideoImpl(std::shared_ptr<Picture> pict)
{
    for (unsigned i = 0; i < 6; ++i)
    {
        Faces f = static_cast<Faces>(i);
        cv::Rect roi( IStartOffset(f),  JStartOffset(f), GetRes(f), GetRes(f) );
        cv::Mat facePictMat ( pict->GetMat(), roi);
        m_outputVideoPtr->Write( facePictMat, i);
    }
}

std::shared_ptr<IMT::LibAv::VideoReader> LayoutCubeMap::InitInputVideoImpl(std::string pathToInputVideo, unsigned nbFrame)
{
    std::shared_ptr<IMT::LibAv::VideoReader> vrPtr = std::make_shared<IMT::LibAv::VideoReader>(pathToInputVideo);
    vrPtr->Init(nbFrame);
    if (vrPtr->GetNbStream() != 6)
    {
        std::cout << "Unsupported number of stream for CubeMap input video: "<<vrPtr->GetNbStream() <<" instead of 6" << std::endl;
        return nullptr;
    }
    //we could add some other check for instance on the width, height of each stream
    return vrPtr;
}

std::shared_ptr<IMT::LibAv::VideoWriter> LayoutCubeMap::InitOutputVideoImpl(std::string pathToOutputVideo, std::string codecId, unsigned fps, unsigned gop_size, std::vector<unsigned> bit_rateVect)
{
    std::shared_ptr<IMT::LibAv::VideoWriter> vwPtr = std::make_shared<IMT::LibAv::VideoWriter>(pathToOutputVideo);
    std::array<unsigned, 6> br;
    std::copy_n(std::make_move_iterator(bit_rateVect.begin()), 6, br.begin());
    std::array<unsigned, 6> resArr;
    for (unsigned i = 0; i < 6; ++i)
    {
        resArr[i] = GetRes(static_cast<Faces>(i));
    }
    vwPtr->Init<6>(codecId, resArr, resArr, fps, gop_size, br);
    return vwPtr;
}
