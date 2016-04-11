#include "layoutCubeMap.hpp"

using namespace IMT;


LayoutCubeMapBased::Faces LayoutCubeMap::From2dToFace(unsigned int i, unsigned int j) const
{
    //From
    if (i < m_cubeEdge && j < m_cubeEdge)
    {
        return Faces::Front;
    }
    else if (i < m_cubeEdge && j < 2*m_cubeEdge)
    {
        return Faces::Back;
    }
    else if (i < 2*m_cubeEdge && j < m_cubeEdge)
    {
        return Faces::Right;
    }
    else if (i < 2*m_cubeEdge && j < 2*m_cubeEdge)
    {
        return Faces::Left;
    }
    else if (i < 3*m_cubeEdge && j < m_cubeEdge)
    {
        return Faces::Top;
    }
    else if (i < 3*m_cubeEdge && j < 2*m_cubeEdge)
    {
        return Faces::Bottom;
    }
}

Layout::NormalizedFaceInfo LayoutCubeMap::From2dToNormalizedFaceInfo(const CoordI& pixel) const
{
    auto f = From2dToFace(pixel.x, pixel.y);
    double i = double(pixel.x%m_cubeEdge)/m_cubeEdge;
    double j = double(pixel.y%m_cubeEdge)/m_cubeEdge;
    return Layout::NormalizedFaceInfo(CoordF(i,j), static_cast<int>(f));
}

#define BORDER(x) (MAX(1.0, MIN(m_cubeEdge,x)))

CoordF LayoutCubeMap::FromNormalizedInfoTo2d(const Layout::NormalizedFaceInfo& ni) const
{
    LayoutCubeMapBased::Faces f = static_cast<LayoutCubeMapBased::Faces>(ni.m_faceId);
    const CoordF& coord (ni.m_normalizedFaceCoordinate);
    switch (f)
    {
        case LayoutCubeMapBased::Faces::Front:
            return CoordF(BORDER(m_cubeEdge*coord.x), BORDER(m_cubeEdge*coord.y));
        case LayoutCubeMapBased::Faces::Back:
            return CoordF(BORDER(m_cubeEdge*coord.x), BORDER(m_cubeEdge*coord.y)+m_cubeEdge);
        case LayoutCubeMapBased::Faces::Left:
            return CoordF(BORDER(m_cubeEdge*coord.x)+m_cubeEdge, BORDER(m_cubeEdge*coord.y));
        case LayoutCubeMapBased::Faces::Right:
            return CoordF(BORDER(m_cubeEdge*coord.x)+m_cubeEdge, BORDER(m_cubeEdge*coord.y)+m_cubeEdge);
        case LayoutCubeMapBased::Faces::Top:
            return CoordF(BORDER(m_cubeEdge*coord.x)+2*m_cubeEdge, BORDER(m_cubeEdge*coord.y));
        case LayoutCubeMapBased::Faces::Bottom:
            return CoordF(BORDER(m_cubeEdge*coord.x)+2*m_cubeEdge, BORDER(m_cubeEdge*coord.y)+m_cubeEdge);
        case LayoutCubeMapBased::Faces::Last:
        case LayoutCubeMapBased::Faces::Black:
            std::invalid_argument("FromNormalizedInfoTo2d: Last is not a valid face");
    }
}
