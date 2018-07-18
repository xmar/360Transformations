#include "LayoutCubeMap2.hpp"

using namespace IMT;

unsigned int LayoutCubeMap2::IStartOffset(LayoutCubeMapBased::Faces f) const
{
    switch(f)
    {
    case Faces::Front:
        return GetResH(Faces::Left);
    case Faces::Left:
        return 0;
    case Faces::Right:
        return GetResH(Faces::Left)+m_maxOffsetTFB;
    case Faces::Top:
        return GetResH(Faces::Left)+(m_maxOffsetTFB-GetResH(Faces::Top))/2;
    case Faces::Bottom:
        return GetResH(Faces::Left)+(m_maxOffsetTFB-GetResH(Faces::Bottom))/2;
    case Faces::Back:
        return GetResH(Faces::Left)+m_maxOffsetTFB+GetResH(Faces::Right);
    case Faces::Black:
    case Faces::Last:
        std::invalid_argument("IStartOffset: Last is not a valid face");
    }
}

unsigned int LayoutCubeMap2::IEndOffset(LayoutCubeMapBased::Faces f) const
{
    switch(f)
    {
    case Faces::Front:
        return IStartOffset(Faces::Right);
    case Faces::Left:
        return IStartOffset(Faces::Front);
    case Faces::Right:
        return IStartOffset(Faces::Back);
    case Faces::Top:
        return GetResH(Faces::Left)+GetResH(Faces::Top)+(m_maxOffsetTFB-GetResH(Faces::Top))/2;
    case Faces::Bottom:
        return GetResH(Faces::Left)+GetResH(Faces::Bottom)+(m_maxOffsetTFB-GetResH(Faces::Bottom))/2;
    case Faces::Back:
        return GetResH(Faces::Left)+m_maxOffsetTFB+GetResH(Faces::Right)+GetResH(Faces::Back);
    case Faces::Black:
    case Faces::Last:
        std::invalid_argument("IEndOffset: Last is not a valid face");
    }
}

unsigned int LayoutCubeMap2::JStartOffset(LayoutCubeMapBased::Faces f) const
{
    switch(f)
    {
    case Faces::Front:
        return GetResV(Faces::Top)+(m_maxOffsetLFRB-GetResV(Faces::Front))/2;
    case Faces::Left:
        return GetResV(Faces::Top)+(m_maxOffsetLFRB-GetResV(Faces::Left))/2;
    case Faces::Right:
        return GetResV(Faces::Top)+(m_maxOffsetLFRB-GetResV(Faces::Right))/2;
    case Faces::Top:
        return 0;
    case Faces::Bottom:
        return GetResV(Faces::Top)+m_maxOffsetLFRB;
    case Faces::Back:
        return GetResV(Faces::Top)+(m_maxOffsetLFRB-GetResV(Faces::Back))/2;
    case Faces::Black:
    case Faces::Last:
        std::invalid_argument("JStartOffset: Last is not a valid face");
    }
}

unsigned int LayoutCubeMap2::JEndOffset(LayoutCubeMapBased::Faces f) const
{
    switch(f)
    {
    case Faces::Front:
        return JStartOffset(Faces::Bottom)-(m_maxOffsetLFRB-GetResV(Faces::Front))/2;
    case Faces::Left:
        return GetResV(Faces::Top)+GetResV(Faces::Left)+(m_maxOffsetLFRB-GetResV(Faces::Left))/2;
    case Faces::Right:
        return GetResV(Faces::Top)+GetResV(Faces::Right)+(m_maxOffsetLFRB-GetResV(Faces::Right))/2;
    case Faces::Top:
        return JStartOffset(Faces::Front)-(m_maxOffsetLFRB-GetResV(Faces::Front))/2;
    case Faces::Bottom:
        return GetResV(Faces::Top)+m_maxOffsetLFRB+GetResV(Faces::Bottom);
    case Faces::Back:
        return GetResV(Faces::Top)+GetResV(Faces::Back)+(m_maxOffsetLFRB-GetResV(Faces::Back))/2;
    case Faces::Black:
    case Faces::Last:
        std::invalid_argument("JEndOffset: Last is not a valid face");
    }
}

LayoutCubeMapBased::Faces LayoutCubeMap2::From2dToFace(unsigned int i, unsigned int j) const
{
	if ( InFace(i,j,Faces::Left))
    {
        return Faces::Left;
    }
    else if (InFace(i,j,Faces::Top))
    {
        return Faces::Top;
    }
    else if (InFace(i,j,Faces::Front))
    {
        return Faces::Front;
    }
    else if (InFace(i,j,Faces::Bottom))
    {
        return Faces::Bottom;
    }
    else if (InFace(i,j,Faces::Right))
    {
        return Faces::Right;
    }
    else if (InFace(i,j,Faces::Back))
    {
        return Faces::Back;
    }
    else
    {
        return Faces::Black;
    }
}

#define BORDERH(x) (MAX(1.0, MIN(GetResH(f)-1,x)))
#define BORDERV(x) (MAX(1.0, MIN(GetResV(f)-1,x)))

CoordF LayoutCubeMap2::FromNormalizedInfoTo2d(const Layout::NormalizedFaceInfo& ni) const
{
    auto f = static_cast<LayoutCubeMapBased::Faces>(ni.m_faceId);
    const CoordF& coord(ni.m_normalizedFaceCoordinate);
    if (f != Faces::Black && f != Faces::Last)
    {
        return CoordF(BORDERH(GetResH(f)*coord.x)+IStartOffset(f), BORDERV(GetResV(f)*coord.y)+JStartOffset(f));
    }
    else
    {
        std::invalid_argument("FromNormalizedInfoTo2d: Last or Black are not a valid face");
    }
}

Layout::NormalizedFaceInfo LayoutCubeMap2::From2dToNormalizedFaceInfo(const CoordI& pixel) const
{
    auto f = From2dToFace(pixel.x, pixel.y);
    if (f == Faces::Black)
    {
        return NormalizedFaceInfo(CoordF(0, 0), static_cast<int>(f));
    }
    double normalizedI = double(pixel.x-IStartOffset(f))/GetResH(f);
    double normalizedJ = double(pixel.y-JStartOffset(f))/GetResV(f);
    return NormalizedFaceInfo(CoordF(normalizedI, normalizedJ), static_cast<int>(f));
}

std::shared_ptr<Picture> LayoutCubeMap2::ReadNextPictureFromVideoImpl(void)
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
          //std::cout << "Expected Width: "<< GetRes(f) << "; Height " << GetRes(f)  << "; received width "<< facePictPtr->cols << " height "<< facePictPtr->rows << std::endl;
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

void LayoutCubeMap2::WritePictureToVideoImpl(std::shared_ptr<Picture> pict)
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

std::shared_ptr<IMT::LibAv::VideoReader> LayoutCubeMap2::InitInputVideoImpl(std::string pathToInputVideo, unsigned nbFrame)
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

std::shared_ptr<IMT::LibAv::VideoWriter> LayoutCubeMap2::InitOutputVideoImpl(std::string pathToOutputVideo, std::string codecId, unsigned fps, unsigned gop_size, std::vector<int> bit_rateVect)
{
  std::shared_ptr<IMT::LibAv::VideoWriter> vwPtr = std::make_shared<IMT::LibAv::VideoWriter>(pathToOutputVideo);
  if(UseTile())
  {
    std::array<int, 6> br;
    std::copy_n(std::make_move_iterator(bit_rateVect.begin()), 6, br.begin());
    std::array<unsigned, 6> resArr;
    for (unsigned i = 0; i < 6; ++i)
    {
        resArr[i] = GetResH(static_cast<Faces>(i));
    }
    vwPtr->Init<6>(codecId, resArr, resArr, fps, gop_size, br);
    return vwPtr;
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

static IMT::LayoutConfigParserCubemap2 maker("cubeMap2", false);
static IMT::LayoutConfigParserCubemap2 maker_eac("EAC2", true);

