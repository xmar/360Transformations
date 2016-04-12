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
        if (i == 0) {normalizedJ = 0.5;}
        else {normalizedJ = (double(j%m_edge)-0.5*m_edge)/i+0.5;}
        return Layout::NormalizedFaceInfo(CoordF(normalizedI, normalizedJ), static_cast<int>(Faces::Left));
    }
    if ( inInterval(j, 0, m_edge) && inInterval(i, 3.0/2*m_edge - 1.0/2*j, 3.0/2*m_edge + 1.0/2*j))
    {//then top
        if (j == 0) {normalizedI = 0.5;}
        else {normalizedI = (double(i%m_edge)-0.5*m_edge)/j+0.5;}
        return Layout::NormalizedFaceInfo(CoordF(normalizedI, normalizedJ), static_cast<int>(Faces::Top));
    }
    if ( inInterval(j, 2*m_edge, 3*m_edge) && inInterval(i, m_edge + 1.0/2*(j-2*m_edge), 2*m_edge - 1.0/2*(j-2*m_edge)))
    {//then bottom
        if ( 3*m_edge - j == 0) {normalizedI = 0.5;}
        else {normalizedI = (double(i%m_edge)-0.5*m_edge)/(3*m_edge - j)+0.5;}
        return Layout::NormalizedFaceInfo(CoordF(normalizedI, normalizedJ), static_cast<int>(Faces::Bottom));
    }
    if ( inInterval(i, 2*m_edge, 3*m_edge) && inInterval(j, m_edge + 1.0/2*(i-2*m_edge), 2*m_edge - 1.0/2*(i-2*m_edge)))
    {//then right
        if ( 3*m_edge - i == 0) {normalizedJ = 0.5;}
        else {normalizedJ = (double(j%m_edge)-0.5*m_edge)/(3*m_edge - i)+0.5;}
        return Layout::NormalizedFaceInfo(CoordF(normalizedI, normalizedJ), static_cast<int>(Faces::Right));
    }
    //otherwise it's black
    return Layout::NormalizedFaceInfo(CoordF(0,0), static_cast<int>(Faces::Black));
}
#define BORDER(x) x
#define BORDER_DIAG(x,pos) x
CoordF LayoutPyramidal2::FromNormalizedInfoTo2d(const Layout::NormalizedFaceInfo& ni) const
{
    auto f = static_cast<Faces>(ni.m_faceId);
    const CoordF& coord (ni.m_normalizedFaceCoordinate);
    const double& normalizedI (coord.x);
    const double& normalizedJ (coord.y);

    switch (f)
    {
        case Faces::Base:
            return CoordF(m_edge*BORDER(normalizedI)+m_edge, BORDER(normalizedJ)*m_edge+m_edge);
        case Faces::Left:
            return CoordF(m_edge*BORDER(normalizedI), BORDER_DIAG(normalizedJ, normalizedI)*m_edge+m_edge);
        case Faces::Right:
            return CoordF(m_edge*normalizedI+2*m_edge, BORDER_DIAG(normalizedJ, normalizedI)*m_edge+m_edge);
        case Faces::Top:
            return CoordF(m_edge*BORDER_DIAG(normalizedI,normalizedJ)+m_edge, m_edge*BORDER(normalizedJ));
        case Faces::Bottom:
            return CoordF(m_edge*BORDER_DIAG(normalizedI,normalizedJ)+m_edge, m_edge*BORDER(normalizedJ)+2*m_edge);
        case Faces::Last:
        case Faces::Black:
            throw std::invalid_argument("FromNormalizedInfoTo2d: Last or Black are not valid faces");

    }

}
