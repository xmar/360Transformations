#include "layoutPyramidal2.hpp"

using namespace IMT;



Layout::NormalizedFaceInfo LayoutPyramidal2::From2dToNormalizedFaceInfo(const CoordI& pixel) const
{
    unsigned int i = pixel.x;
    unsigned int j = pixel.y;
    double normalizedI = double(i%m_edge)/m_edge;
    double normalizedJ = double(j%m_edge)/m_edge;
    if (inInterval(i, m_edge, 2*m_edge-1) && inInterval(j, m_edge, 2*m_edge-1))
    {//then we are on the base
         return Layout::NormalizedFaceInfo(CoordF(normalizedI, normalizedJ), static_cast<int>(Faces::Base));
    }
    if (inInterval(i, 0, m_edge) && inInterval(j, 3.0/2*m_edge - 1.0/2*i, 3.0/2*m_edge + 1.0/2*i))
    {//then left
        if (normalizedI == 0) {normalizedJ = 0.5;}
        else {normalizedJ = (normalizedJ-0.5)/normalizedI+0.5;}
        return Layout::NormalizedFaceInfo(CoordF(normalizedI, normalizedJ), static_cast<int>(Faces::Left));
    }
    if ( inInterval(j, 0, m_edge) && inInterval(i, 3.0/2*m_edge - 1.0/2*j, 3.0/2*m_edge + 1.0/2*j))
    {//then top
        if (normalizedJ == 0) {normalizedI = 0.5;}
        else {normalizedI = (normalizedI-0.5)/normalizedJ+0.5;}
        return Layout::NormalizedFaceInfo(CoordF(normalizedI, normalizedJ), static_cast<int>(Faces::Top));
    }
    if ( inInterval(j, 2*m_edge, 3*m_edge) && inInterval(i, m_edge + 1.0/2*(j-2*m_edge), 2*m_edge - 1.0/2*(j-2*m_edge)))
    {//then bottom
        if ( normalizedJ == 1) {normalizedI = 0.5;}
        else {normalizedI = (normalizedI-0.5)/(1-normalizedJ)+0.5;}
        return Layout::NormalizedFaceInfo(CoordF(normalizedI, normalizedJ), static_cast<int>(Faces::Bottom));
    }
    if ( inInterval(i, 2*m_edge, 3*m_edge) && inInterval(j, m_edge + 1.0/2*(i-2*m_edge), 2*m_edge - 1.0/2*(i-2*m_edge)))
    {//then right
        if ( normalizedI == 1) {normalizedJ = 0.5;}
        else {normalizedJ = (normalizedJ-0.5)/(1-normalizedI)+0.5;}
        return Layout::NormalizedFaceInfo(CoordF(normalizedI, normalizedJ), static_cast<int>(Faces::Right));
    }
    //otherwise it's black
    return Layout::NormalizedFaceInfo(CoordF(0,0), static_cast<int>(Faces::Black));
}

CoordF LayoutPyramidal2::FromNormalizedInfoTo2d(const Layout::NormalizedFaceInfo& ni) const
{
    auto f = static_cast<Faces>(ni.m_faceId);
    const CoordF& coord (ni.m_normalizedFaceCoordinate);
    const double& normalizedI (coord.x);
    const double& normalizedJ (coord.y);

    switch (f)
    {
        case Faces::Base:
            return CoordF(m_edge*normalizedI+m_edge, normalizedJ*m_edge+m_edge);
        case Faces::Left:
            return CoordF(m_edge*normalizedI, (normalizedI*(normalizedJ-0.5)+0.5)*m_edge+m_edge);
        case Faces::Right:
            return CoordF(m_edge*normalizedI+2*m_edge, ((1-normalizedI)*(normalizedJ-0.5)+0.5)*m_edge+m_edge);
        case Faces::Top:
            return CoordF(m_edge*(normalizedJ*(normalizedI-0.5)+0.5)+m_edge, m_edge*normalizedJ);
        case Faces::Bottom:
            return CoordF(m_edge*((1-normalizedJ)*(normalizedI-0.5)+0.5)+m_edge, m_edge*normalizedJ+2*m_edge);
        case Faces::Last:
        case Faces::Black:
            throw std::invalid_argument("FromNormalizedInfoTo2d: Last or Black are not valid faces");

    }

}
