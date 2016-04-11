#include "layoutCubeMap2.hpp"

using namespace IMT;

LayoutCubeMapBased::Faces LayoutCubeMap2::From2dToFace(unsigned int i, unsigned int j) const
{
    //From
	if ( i <= m_cubeEdge && j > m_cubeEdge && j <= 2*m_cubeEdge)
    {
        return Faces::Left;
    }
    else if (i > m_cubeEdge && i <= 2*m_cubeEdge && j <= m_cubeEdge)
    {
        return Faces::Top;
    }
    else if (i > m_cubeEdge && i <= 2*m_cubeEdge && j > m_cubeEdge && j <= 2*m_cubeEdge)
    {
        return Faces::Front;
    }
    else if (i > m_cubeEdge && i <= 2*m_cubeEdge && j > 2*m_cubeEdge && j <= 3*m_cubeEdge)
    {
        return Faces::Bottom;
    }
    else if (i > 2*m_cubeEdge && i <= 3*m_cubeEdge && j > m_cubeEdge && j <= 2*m_cubeEdge)
    {
        return Faces::Right;
    }
    else if (i > 3*m_cubeEdge && i <= 4*m_cubeEdge && j > m_cubeEdge && j <=2*m_cubeEdge)
    {
        return Faces::Back;
    }
    else {
        return Faces::Black;
    }
}

#define BORDER(x) (MAX(1.0, MIN(m_cubeEdge-1,x)))

CoordF LayoutCubeMap2::FromNormalizedInfoTo2d(const Layout::NormalizedFaceInfo& ni) const
{
    auto f = static_cast<LayoutCubeMapBased::Faces>(ni.m_faceId);
    const CoordF& coord(ni.m_normalizedFaceCoordinate);
    switch (f)
    {
        case Faces::Front:
            return CoordF(BORDER(m_cubeEdge*coord.x)+m_cubeEdge, BORDER(m_cubeEdge*coord.y)+m_cubeEdge);
        case Faces::Back:
            return CoordF(BORDER(m_cubeEdge*coord.x)+3*m_cubeEdge, BORDER(m_cubeEdge*coord.y)+m_cubeEdge);
        case Faces::Left:
            return CoordF(BORDER(m_cubeEdge*coord.x), BORDER(m_cubeEdge*coord.y)+m_cubeEdge);
        case Faces::Right:
            return CoordF(BORDER(m_cubeEdge*coord.x)+2*m_cubeEdge, BORDER(m_cubeEdge*coord.y)+m_cubeEdge);
        case Faces::Top:
            return CoordF(BORDER(m_cubeEdge*coord.x)+m_cubeEdge, BORDER(m_cubeEdge*coord.y));
        case Faces::Bottom:
            return CoordF(BORDER(m_cubeEdge*coord.x)+m_cubeEdge, BORDER(m_cubeEdge*coord.y)+2*m_cubeEdge);
        case Faces::Black:
        case Faces::Last:
            std::invalid_argument("FromNormalizedInfoTo2d: Last is not a valid face");
    }
}

Layout::NormalizedFaceInfo LayoutCubeMap2::From2dToNormalizedFaceInfo(const CoordI& pixel) const
{
    double normalizedI = double(pixel.x%m_cubeEdge)/m_cubeEdge;
    double normalizedJ = double(pixel.y%m_cubeEdge)/m_cubeEdge;
    return NormalizedFaceInfo(CoordF(normalizedI, normalizedJ), static_cast<int>(From2dToFace(pixel.x, pixel.y)));
}
