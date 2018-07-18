#include "LayoutCubeMap.hpp"

#include <algorithm>
#include <iterator>

using namespace IMT;

unsigned int LayoutCubeMap::IStartOffset(LayoutCubeMapBased::Faces f) const
{
    int facePositionId = m_fp.m_faceToId.at(f);
    switch(facePositionId)
    {
        case 4:
            return m_maxOffsetCols[0]+(m_maxOffsetCols[1]-GetResH(f))/2;
        case 1:
            return m_maxOffsetCols[0]+(m_maxOffsetCols[1]-GetResH(f))/2;
        case 0:
            return (m_maxOffsetCols[0]-GetResH(f))/2;
        case 2:
            return m_maxOffsetCols[0]+m_maxOffsetCols[1]+(m_maxOffsetCols[2]-GetResH(f))/2;
        case 3:
            return (m_maxOffsetCols[0]-GetResH(f))/2;
        case 5:
            return m_maxOffsetCols[0]+m_maxOffsetCols[1]+(m_maxOffsetCols[2]-GetResH(f))/2;
        default:
            std::cout << "IStartOffset: Last is not a valid face" << std::endl;
            throw std::invalid_argument("IStartOffset: Last is not a valid face");
    }
}
unsigned int LayoutCubeMap::IEndOffset(LayoutCubeMapBased::Faces f) const
{
    int facePositionId = m_fp.m_faceToId.at(f);
    switch(facePositionId)
    {
        case 4:
            return m_maxOffsetCols[0]+m_maxOffsetCols[1]-(m_maxOffsetCols[1]-GetResH(f))/2;
        case 1:
            return m_maxOffsetCols[0]+m_maxOffsetCols[1]-(m_maxOffsetCols[1]-GetResH(f))/2;
        case 0:
            return m_maxOffsetCols[0]-(m_maxOffsetCols[0]-GetResH(f))/2;
        case 2:
            return m_maxOffsetCols[0]+m_maxOffsetCols[1]+m_maxOffsetCols[2]-(m_maxOffsetCols[2]-GetResH(f))/2;
        case 3:
            return m_maxOffsetCols[0]-(m_maxOffsetCols[0]-GetResH(f))/2;
        case 5:
            return m_maxOffsetCols[0]+m_maxOffsetCols[1]+m_maxOffsetCols[2]-(m_maxOffsetCols[2]-GetResH(f))/2;
        default:
            std::cout << "IEndOffset: Last is not a valid face" << std::endl;
            throw std::invalid_argument("IEndOffset: Last is not a valid face");
    }
}
unsigned int LayoutCubeMap::JStartOffset(LayoutCubeMapBased::Faces f) const
{
    int facePositionId = m_fp.m_faceToId.at(f);
    switch(facePositionId)
    {
        case 4:
            return m_maxOffsetRows[0]+(m_maxOffsetRows[1]-GetResV(f))/2;
        case 1:
            return (m_maxOffsetRows[0]-GetResV(f))/2;
        case 0:
            return (m_maxOffsetRows[0]-GetResV(f))/2;;
        case 2:
            return (m_maxOffsetRows[0]-GetResV(f))/2;
        case 3:
            return m_maxOffsetRows[0]+(m_maxOffsetRows[1]-GetResV(f))/2;;
        case 5:
            return m_maxOffsetRows[0]+(m_maxOffsetRows[1]-GetResV(f))/2;
        default:
            std::cout << "JHStartOffset: Last is not a valid face" << std::endl;
            throw std::invalid_argument("JStartOffset: Last is not a valid face");
    }
}
unsigned int LayoutCubeMap::JEndOffset(LayoutCubeMapBased::Faces f) const
{
    int facePositionId = m_fp.m_faceToId.at(f);
    switch(facePositionId)
    {
        case 4:
            return m_maxOffsetRows[0]+m_maxOffsetRows[1]-(m_maxOffsetRows[1]-GetResV(f))/2;
        case 1:
            return m_maxOffsetRows[0]-(m_maxOffsetRows[0]-GetResV(f))/2;
        case 0:
            return m_maxOffsetRows[0]-(m_maxOffsetRows[0]-GetResV(f))/2;;
        case 2:
            return m_maxOffsetRows[0]-(m_maxOffsetRows[0]-GetResV(f))/2;
        case 3:
            return m_maxOffsetRows[0]+m_maxOffsetRows[1]-(m_maxOffsetRows[1]-GetResV(f))/2;;
        case 5:
            return m_maxOffsetRows[0]+m_maxOffsetRows[1]-(m_maxOffsetRows[1]-GetResV(f))/2;
        default:
            std::cout << "JEndOffset: Last is not a valid face" << std::endl;
            throw std::invalid_argument("JEndOffset: Last is not a valid face");
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
    double u = double(pixel.x - IStartOffset(f))/GetResH(f);
    double v = double(pixel.y - JStartOffset(f))/GetResV(f);
    double i(-1);
    double j(-1);
    std::tie(i,j) = m_fp.GetIJ2dToNorm(u, v, f);
    return Layout::NormalizedFaceInfo(CoordF(i,j), static_cast<int>(f));
}

#define BORDERH(x) (MAX(1.0, MIN(GetResH(f)-1,x)))
#define BORDERV(x) (MAX(1.0, MIN(GetResV(f)-1,x)))

CoordF LayoutCubeMap::FromNormalizedInfoTo2d(const Layout::NormalizedFaceInfo& ni) const
{
    Faces f = static_cast<LayoutCubeMapBased::Faces>(ni.m_faceId);
    const CoordF& coord (ni.m_normalizedFaceCoordinate);
    if (f != Faces::Last && f != Faces::Black)
    {
        double i(-1);
        double j(-1);
        std::tie(i,j) = m_fp.GetIJNormTo2d(coord.x, coord.y, f);
        return CoordF(BORDERH(GetResH(f)*i)+IStartOffset(f), BORDERV(GetResV(f)*j)+JStartOffset(f));
    }
    else
    {
        std::cout << "FromNormalizedInfoTo2d: Last is not a valid face" << std::endl;
        throw std::invalid_argument("FromNormalizedInfoTo2d: Last is not a valid face");
    }
}

std::shared_ptr<Picture> LayoutCubeMap::ReadNextPictureFromVideoImpl(void)
{
    bool isInit = false;
    cv::Mat outputMat;
    if (UseTile())
    {
      for (unsigned i = 0; i < 6; ++i)
      {
          Faces f = static_cast<Faces>(i);
          cv::Rect roi( IStartOffset(f),  JStartOffset(f), GetResH(f), GetResV(f) );
          auto facePictPtr = m_inputVideoPtr->GetNextPicture(i);
          if (!isInit)
          {
              outputMat = cv::Mat( m_outHeight, m_outWidth, facePictPtr->type());
              isInit = true;
          }
          cv::Mat facePictMat ( outputMat, roi);
          facePictPtr->copyTo(facePictMat);
      }
    }
    else
    {
      auto facePictPtr = m_inputVideoPtr->GetNextPicture(0);
      //outputMat = cv::Mat( m_outHeight, m_outWidth, facePictPtr->type());
      facePictPtr->copyTo(outputMat);
    }
    return std::make_shared<Picture>(outputMat);
}

void LayoutCubeMap::WritePictureToVideoImpl(std::shared_ptr<Picture> pict)
{
  if (UseTile())
  {
    for (unsigned i = 0; i < 6; ++i)
    {
        Faces f = static_cast<Faces>(i);
        cv::Rect roi( IStartOffset(f),  JStartOffset(f), GetResH(f), GetResV(f) );
        cv::Mat facePictMat ( pict->GetMat(), roi);
        m_outputVideoPtr->Write( facePictMat, i);
    }
  }
  else
  {
    m_outputVideoPtr->Write(pict->GetMat(), 0);
  }
}

std::shared_ptr<IMT::LibAv::VideoReader> LayoutCubeMap::InitInputVideoImpl(std::string pathToInputVideo, unsigned nbFrame)
{
    std::shared_ptr<IMT::LibAv::VideoReader> vrPtr = std::make_shared<IMT::LibAv::VideoReader>(pathToInputVideo);
    vrPtr->Init(nbFrame);
    if ((UseTile() && vrPtr->GetNbStream() != 6) || (vrPtr->GetNbStream() != 1))
    {
        std::cout << "Unsupported number of stream for CubeMap input video: "<<vrPtr->GetNbStream() <<" instead of 6" << std::endl;
        return nullptr;
    }
    //we could add some other check for instance on the width, height of each stream
    return vrPtr;
}

std::shared_ptr<IMT::LibAv::VideoWriter> LayoutCubeMap::InitOutputVideoImpl(std::string pathToOutputVideo, std::string codecId, unsigned fps, unsigned gop_size, std::vector<int> bit_rateVect)
{
  std::shared_ptr<IMT::LibAv::VideoWriter> vwPtr = std::make_shared<IMT::LibAv::VideoWriter>(pathToOutputVideo);
  if (UseTile())
  {
    std::array<int, 6> br;
    std::copy_n(std::make_move_iterator(bit_rateVect.begin()), 6, br.begin());
    std::array<unsigned, 6> resArr;
    for (unsigned i = 0; i < 6; ++i)
    {
        resArr[i] = GetResH(static_cast<Faces>(i));
    }
    vwPtr->Init<6>(codecId, resArr, resArr, fps, gop_size, br);
  }
  else
  {
    std::array<int, 1> br;
    std::copy_n(std::make_move_iterator(bit_rateVect.begin()), 1, br.begin());
    std::array<unsigned, 1> resArrV;
    std::array<unsigned, 1> resArrH;
    resArrV[0] = m_outHeight;
    resArrH[0] = m_outWidth;
    vwPtr->Init<1>(codecId, resArrH, resArrV, fps, gop_size, br);
  }
  return vwPtr;
}

static IMT::LayoutConfigParserCubemap maker("cubeMap", false);
static IMT::LayoutConfigParserCubemap maker_eac("EAC", true);

