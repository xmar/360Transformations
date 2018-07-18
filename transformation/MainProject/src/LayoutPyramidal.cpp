#include "LayoutPyramidal.hpp"
#include <limits>

using namespace IMT;



Layout::NormalizedFaceInfo LayoutPyramidal::From2dToNormalizedFaceInfo(const CoordI& pixel) const
{
    unsigned int i = pixel.x;
    unsigned int j = pixel.y;
    double normalizedI = double(i%m_outHeight)/m_outHeight;
    double normalizedJ = double(j%m_outHeight)/m_outHeight;

    if (inInterval(i, m_outHeight+1, 2*m_outHeight))
    {//then we are on the base
         return Layout::NormalizedFaceInfo(CoordF(normalizedI, normalizedJ), static_cast<int>(Faces::Base));
    }
    if (inInterval(i, 0, m_outHeight))
    {//then top or left
        if (inInterval(normalizedJ-0.5, -0.5*normalizedI, 0.5*normalizedI))
        {//Left face
            normalizedJ = normalizedI != 0 ? (normalizedJ-0.5)/normalizedI+0.5 : 0.5;
            return Layout::NormalizedFaceInfo(CoordF(normalizedI, normalizedJ), static_cast<int>(Faces::Left));
        }
        else
        {//top face
            normalizedJ = normalizedJ + (normalizedJ >= 0.5 ? -0.5 : 0.5);
            normalizedJ = normalizedI != 1 ? (normalizedJ-0.5)/(1-normalizedI)+0.5 : 0.5;
            return Layout::NormalizedFaceInfo(CoordF(normalizedJ, 1-normalizedI), static_cast<int>(Faces::Top));
        }
    }
    else
    {//then right or bottom
        if (inInterval(normalizedJ-0.5, -0.5+0.5*normalizedI, 0.5-0.5*normalizedI))
        {//Right face
            normalizedJ = normalizedI != 1 ? (normalizedJ-0.5)/(1-normalizedI)+0.5 : 0.5;
            return Layout::NormalizedFaceInfo(CoordF(normalizedI, normalizedJ), static_cast<int>(Faces::Right));
        }
        else
        {//bottom face
            normalizedJ = normalizedJ + (normalizedJ >= 0.5 ? -0.5 : 0.5);
            normalizedJ = normalizedI != 0 ? (normalizedJ-0.5)/normalizedI+0.5 : 0.5;
            return Layout::NormalizedFaceInfo(CoordF(normalizedJ, 1-normalizedI), static_cast<int>(Faces::Bottom));
        }
    }
}
#define BORDER(x) (x)
#define BORDER_DIAG(x,pos) (x)
CoordF LayoutPyramidal::FromNormalizedInfoTo2d(const Layout::NormalizedFaceInfo& ni) const
{
    auto f = static_cast<Faces>(ni.m_faceId);
    const CoordF& coord (ni.m_normalizedFaceCoordinate);
    double normalizedI (coord.x);
    double normalizedJ (coord.y);

    switch (f)
    {
        case Faces::Base:
            return CoordF(m_outHeight*normalizedI+m_outHeight, normalizedJ*m_outHeight);
        case Faces::Left:
            return CoordF(m_outHeight*normalizedI, ((normalizedJ-0.5)*normalizedI+0.5)*m_outHeight);
        case Faces::Right:
            return CoordF(m_outHeight*normalizedI+2*m_outHeight, ((normalizedJ-0.5)*(1-normalizedI)+0.5)*m_outHeight);
        case Faces::Top:
            {
                normalizedI = (normalizedI-0.5)*(1-normalizedJ)+0.5;
                normalizedI = normalizedI + (normalizedI <=0.5 ? 0.5 : -0.5);
                return CoordF(m_outHeight*(1-normalizedJ), m_outHeight*normalizedI);
            }
        case Faces::Bottom:
            {
                normalizedI = (normalizedI-0.5)*normalizedJ+0.5;
                normalizedI = normalizedI + (normalizedI <=0.5 ? 0.5 : -0.5);
                return CoordF(m_outHeight*(1-normalizedJ)+2*m_outHeight, m_outHeight*normalizedI);
            }
        case Faces::Last:
        case Faces::Black:
            throw std::invalid_argument("FromNormalizedInfoTo2d: Last or Black are not valid faces");

    }
}

std::shared_ptr<Picture> LayoutPyramidal::ReadNextPictureFromVideoImpl(void)
{
    bool isInit = false;
    cv::Mat outputMat;
    if (UseTile())
    {
        std::cout << "Unsupported tile track for Pyramidal video" << std::endl;
        return nullptr;
    }
    else
    {
      auto facePictPtr = m_inputVideoPtr->GetNextPicture(0);
      facePictPtr->copyTo(outputMat);
    }
    return std::make_shared<Picture>(outputMat);
}

void LayoutPyramidal::WritePictureToVideoImpl(std::shared_ptr<Picture> pict)
{
    if (UseTile())
    {
        std::cout << "Unsupported tile track for Pyramidal video" << std::endl;
        return;
    }
    else
    {
      m_outputVideoPtr->Write(pict->GetMat(),0);
    }
}

std::shared_ptr<IMT::LibAv::VideoReader> LayoutPyramidal::InitInputVideoImpl(std::string pathToInputVideo, unsigned nbFrame)
{
    std::shared_ptr<IMT::LibAv::VideoReader> vrPtr = std::make_shared<IMT::LibAv::VideoReader>(pathToInputVideo);
    vrPtr->Init(nbFrame);
    if ((UseTile() && vrPtr->GetNbStream() != 5) || (vrPtr->GetNbStream() != 1))
    {
        std::cout << "Unsupported number of stream for Pyramidal input video: "<<vrPtr->GetNbStream() <<" instead of 5" << std::endl;
        return nullptr;
    }
    //we could add some other check for instance on the width, height of each stream
    return vrPtr;
}

std::shared_ptr<IMT::LibAv::VideoWriter> LayoutPyramidal::InitOutputVideoImpl(std::string pathToOutputVideo, std::string codecId, unsigned fps, unsigned gop_size, std::vector<int> bit_rateVect)
{
    std::shared_ptr<IMT::LibAv::VideoWriter> vwPtr = std::make_shared<IMT::LibAv::VideoWriter>(pathToOutputVideo);
    if (UseTile())
    {
        std::cout << "Unsupported tile track for Pyramidal video" << std::endl;
        return nullptr;
    }
    else
    {
      std::array<int, 1> br;
      std::copy_n(std::make_move_iterator(bit_rateVect.begin()), 1, br.begin());
      std::array<unsigned, 1> widthArr;
      std::array<unsigned, 1> heightArr;
      widthArr[0] = m_outWidth;
      heightArr[0] = m_outHeight;
      vwPtr->Init<1>(codecId, widthArr, heightArr, fps, gop_size, br);
    }
    return vwPtr;
}


REGISTER_LAYOUT("pyramid", LayoutConfigParserPyramid);
