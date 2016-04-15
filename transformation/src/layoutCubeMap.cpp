#include "layoutCubeMap.hpp"

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
