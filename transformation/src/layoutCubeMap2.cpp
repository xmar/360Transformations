#include "layoutCubeMap2.hpp"

using namespace IMT;

unsigned int LayoutCubeMap2::IStartOffset(LayoutCubeMapBased::Faces f) const
{
    switch(f)
    {
    case Faces::Front:
        return GetRes(Faces::Left);
    case Faces::Left:
        return 0;
    case Faces::Right:
        return GetRes(Faces::Left)+m_maxOffsetTFB;
    case Faces::Top:
        return GetRes(Faces::Left)+(m_maxOffsetTFB-GetRes(Faces::Top))/2;
    case Faces::Bottom:
        return GetRes(Faces::Left)+(m_maxOffsetTFB-GetRes(Faces::Bottom))/2;
    case Faces::Back:
        return GetRes(Faces::Left)+m_maxOffsetTFB+GetRes(Faces::Right);
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
        return GetRes(Faces::Left)+GetRes(Faces::Top)+(m_maxOffsetTFB-GetRes(Faces::Top))/2;
    case Faces::Bottom:
        return GetRes(Faces::Left)+GetRes(Faces::Bottom)+(m_maxOffsetTFB-GetRes(Faces::Bottom))/2;
    case Faces::Back:
        return GetRes(Faces::Left)+m_maxOffsetTFB+GetRes(Faces::Right)+GetRes(Faces::Back);
    case Faces::Black:
    case Faces::Last:
        std::invalid_argument("IStartOffset: Last is not a valid face");
    }
}

unsigned int LayoutCubeMap2::JStartOffset(LayoutCubeMapBased::Faces f) const
{
    switch(f)
    {
    case Faces::Front:
        return GetRes(Faces::Top)+(m_maxOffsetLFRB-GetRes(Faces::Front))/2;
    case Faces::Left:
        return GetRes(Faces::Top)+(m_maxOffsetLFRB-GetRes(Faces::Left))/2;
    case Faces::Right:
        return GetRes(Faces::Top)+(m_maxOffsetLFRB-GetRes(Faces::Right))/2;
    case Faces::Top:
        return 0;
    case Faces::Bottom:
        return GetRes(Faces::Top)+m_maxOffsetLFRB;
    case Faces::Back:
        return GetRes(Faces::Top)+(m_maxOffsetLFRB-GetRes(Faces::Back))/2;
    case Faces::Black:
    case Faces::Last:
        std::invalid_argument("IStartOffset: Last is not a valid face");
    }
}

unsigned int LayoutCubeMap2::JEndOffset(LayoutCubeMapBased::Faces f) const
{
    switch(f)
    {
    case Faces::Front:
        return JStartOffset(Faces::Bottom)-(m_maxOffsetLFRB-GetRes(Faces::Front))/2;
    case Faces::Left:
        return GetRes(Faces::Top)+GetRes(Faces::Left)+(m_maxOffsetLFRB-GetRes(Faces::Left))/2;
    case Faces::Right:
        return GetRes(Faces::Top)+GetRes(Faces::Right)+(m_maxOffsetLFRB-GetRes(Faces::Right))/2;
    case Faces::Top:
        return JStartOffset(Faces::Front)-(m_maxOffsetLFRB-GetRes(Faces::Front))/2;
    case Faces::Bottom:
        return GetRes(Faces::Top)+m_maxOffsetLFRB+GetRes(Faces::Bottom);
    case Faces::Back:
        return GetRes(Faces::Top)+GetRes(Faces::Back)+(m_maxOffsetLFRB-GetRes(Faces::Back))/2;
    case Faces::Black:
    case Faces::Last:
        std::invalid_argument("IStartOffset: Last is not a valid face");
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

#define BORDER(x) (MAX(1.0, MIN(GetRes(f)-1,x)))

CoordF LayoutCubeMap2::FromNormalizedInfoTo2d(const Layout::NormalizedFaceInfo& ni) const
{
    auto f = static_cast<LayoutCubeMapBased::Faces>(ni.m_faceId);
    const CoordF& coord(ni.m_normalizedFaceCoordinate);
    if (f != Faces::Black && f != Faces::Last)
    {
        return CoordF(BORDER(GetRes(f)*coord.x)+IStartOffset(f), BORDER(GetRes(f)*coord.y)+JStartOffset(f));
    }
    else
    {
        std::invalid_argument("FromNormalizedInfoTo2d: Last is not a valid face");
    }
}

Layout::NormalizedFaceInfo LayoutCubeMap2::From2dToNormalizedFaceInfo(const CoordI& pixel) const
{
    auto f = From2dToFace(pixel.x, pixel.y);
    if (f == Faces::Black)
    {
        return NormalizedFaceInfo(CoordF(0, 0), static_cast<int>(f));
    }
    double normalizedI = double(pixel.x-IStartOffset(f))/GetRes(f);
    double normalizedJ = double(pixel.y-JStartOffset(f))/GetRes(f);
    return NormalizedFaceInfo(CoordF(normalizedI, normalizedJ), static_cast<int>(f));
}
