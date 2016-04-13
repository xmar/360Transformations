#include "layoutPyramidal.hpp"
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

