#include "layoutPyramidal.hpp"
#include <limits>

using namespace IMT;

/*          3*h
 *   +-----+-----+-----+
 *   |    /|     |\    |
 *   |  /  |     |  \  |
 *   |/  L |  Ba |  R \|  h
 *   |\    |     |    /|
 *   |  \  |     |  /  |
 *   | T  \|     |/  Bo|
 *   +-----+-----+-----+
 */

double LayoutPyramidal::UsePlanEquation(double x) const //Use the plan equation to compute the second (non nul) variable knowing the value of x
{
    return -m_canonicTopPlan[3] - x * m_canonicTopPlan[0];
}

Layout::NormalizedFaceInfo LayoutPyramidal::From2dToNormalizedFaceInfo(const CoordI& pixel) const
{
    unsigned int i = pixel.x;
    unsigned int j = pixel.y;
    if (inInterval(i, m_outHeight+1, 2*m_outHeight))
    {//then we are on the base
        i -= m_outHeight;
        return Layout::NormalizedFaceInfo(CoordF(double(i)/m_outHeight, double(j)/m_outHeight),
                                          static_cast<int>(Face::Base));
    }
    if (inInterval(i, 0, m_outHeight))
    {//then top or left
        double normalizedI = double(i)/m_outHeight; // from 0 to 1
        double normalizedJ = double(j)/m_outHeight-0.5; //from -0.5 to 0.5
        if (inInterval(normalizedJ, -0.5*normalizedI, 0.5*normalizedI))
        {//Left face
             normalizedI = 1.0-normalizedI;
             return Layout::NormalizedFaceInfo(CoordF(normalizedI, normalizedJ+0.5),
                                          static_cast<int>(Face::Left));
        }
        else
        {//top face
            normalizedJ = std::fmod(normalizedJ + 1,1.0)-0.5;

            return Layout::NormalizedFaceInfo(CoordF(normalizedI, normalizedJ+0.5),
                                          static_cast<int>(Face::Top));
        }
    }
    else
    {//then right or bottom
        double normalizedI = double(i-2*m_outHeight)/m_outHeight; // from 0 to 1
        double normalizedJ = double(j)/m_outHeight-0.5; //from -0.5 to 0.5
        if (inInterval(normalizedJ, -0.5+0.5*normalizedI, 0.5-0.5*normalizedI))
        {//Right face
            return Layout::NormalizedFaceInfo(CoordF(normalizedI, normalizedJ+0.5),
                                          static_cast<int>(Face::Right));
        }
        else
        {//bottom face
            normalizedJ = (std::fmod(normalizedJ + 1,1.0)-0.5);
            normalizedI = 1.0-normalizedI;
            return Layout::NormalizedFaceInfo(CoordF(normalizedI, normalizedJ+0.5),
                                          static_cast<int>(Face::Left));;
        }
    }
}
#define BORDER(x) (MAX(0,MIN((m_outHeight-1)/m_outHeight, x)))
#define BORDER_DIAG(x,pos) (x)
CoordF LayoutPyramidal::FromNormalizedInfoTo2d(const Layout::NormalizedFaceInfo& ni) const
{
    auto f = static_cast<Face>(ni.m_faceId);
    const CoordF& coord (ni.m_normalizedFaceCoordinate);
    const double normalizedI (BORDER(coord.x));
    double normalizedJ (coord.y);

    switch (f)
    {
        case Face::Base:
            return CoordF(m_outHeight*normalizedI+m_outHeight, BORDER(normalizedJ)*m_outHeight);
        case Face::Left:
            return CoordF(m_outHeight*normalizedI+2*m_outHeight, BORDER_DIAG(normalizedJ, normalizedI)*m_outHeight);
        case Face::Right:
            return CoordF( m_outHeight-normalizedI, BORDER_DIAG(normalizedJ, normalizedI)*m_outHeight);
        case Face::Top:
            return CoordF( m_outHeight*normalizedI, m_outHeight*BORDER_DIAG(normalizedJ-0.5, normalizedI)-0.5);
        case Face::Bottom:
            return CoordF( m_outHeight*normalizedI+2*m_outHeight, BORDER_DIAG(normalizedJ-0.5, normalizedI));
         case Face::Last:
            return CoordF(0,0);

    }

}
Layout::NormalizedFaceInfo LayoutPyramidal::From3dToNormalizedFaceInfo(const Coord3dSpherical& sphericalCoord) const
{
    auto f = Face::Last;
    Coord3dCart inter;
    double minRho = std::numeric_limits<double>::max();


    //Transform to the canonic representation
    Coord3dSpherical p = Rotation(sphericalCoord, -m_yaw, -m_pitch, -m_roll);

    for (auto testF: get_range<LayoutPyramidal::Face>())
    {
        try {
            auto plan = FaceToPlan(testF);
            auto interSphe = IntersectionPlanSpherical(plan, p); //raise exception if no intersection
            if (minRho > interSphe.x && AlmostEqual(interSphe.y, p.y)) //check direction
            {
                minRho = interSphe.x;
                inter = SphericalToCart(interSphe);
                f = testF;
            }
        } catch ( std::logic_error& le )
        { //no intersection with this face
            continue;
        }
    }
    double normalizedI(0);
    double normalizedJ(0);
    switch (f)
    {
        case Face::Base:
            normalizedI = inter.y/m_baseEdge+0.5;
            normalizedJ = inter.z/m_baseEdge+0.5;
            break;
        case Face::Left:
            normalizedI = m_outHeight*(1.0 - inter.x)/m_pyramidHeight;
            normalizedJ = inter.z/m_baseEdge+0.5;
            break;
        case Face::Right:
            normalizedI = m_outHeight*(1.0 - inter.x)/m_pyramidHeight;
            normalizedJ = inter.z/m_baseEdge+0.5;
            break;
        case Face::Top:
            normalizedI = (1.0 - inter.x)/m_pyramidHeight;
            normalizedJ = std::fmod(inter.y/m_baseEdge + 1.0,1.0);
            break;
        case Face::Bottom:
            normalizedI = (inter.x - 1.0 + m_pyramidHeight)/m_pyramidHeight;
            normalizedJ = std::fmod(inter.y/m_baseEdge + 1.0,1.0);
            break;
         case Face::Last:
            {
               std::cout << "Error: no intersection detected..." << std::endl;
               return Layout::NormalizedFaceInfo(CoordF(0, 0),static_cast<int>(Face::Last));
            }
    }
    return Layout::NormalizedFaceInfo(CoordF(normalizedI, normalizedJ),static_cast<int>(f));
}
Coord3dCart LayoutPyramidal::FromNormalizedInfoTo3d(const NormalizedFaceInfo& ni) const
{
    auto f = static_cast<Face>(ni.m_faceId);
    const CoordF& coord (ni.m_normalizedFaceCoordinate);
    const double& normalizedI (coord.x);
    const double& normalizedJ (coord.y);
    switch (f)
    {
    case Face::Base:
        {
            Coord3dCart inter(1, (normalizedI-0.5)*m_baseEdge, (normalizedJ-0.5)*m_baseEdge);
            return Rotation(inter, m_yaw, m_pitch, m_roll);
        }
    case Face::Left:
        {
            double z = normalizedJ * m_baseEdge;
            double x = (1.0-normalizedI*m_pyramidHeight);
            double y = -UsePlanEquation(x);
            return Rotation(Coord3dCart(x,y,z), m_yaw, m_pitch, m_roll);
        }
    case Face::Top:
        {
            double y = normalizedJ * m_baseEdge;
            double x = (1.0-normalizedI*m_pyramidHeight);
            double z = -UsePlanEquation(x);
            return Rotation(Coord3dCart(x,y,z), m_yaw, m_pitch, m_roll);
        }
    case Face::Right:
        {
            double z = normalizedJ * m_baseEdge;
            double x = (1.0-normalizedI*m_pyramidHeight);
            double y = UsePlanEquation(x);
            return Rotation(Coord3dCart(x,y,z), m_yaw, m_pitch, m_roll);
        }
    case Face::Bottom:
        {
            double y = normalizedJ * m_baseEdge;
            double x = (1.0-normalizedI*m_pyramidHeight);
            double z = UsePlanEquation(x);
            return Rotation(Coord3dCart(x,y,z), m_yaw, m_pitch, m_roll);
        }
    case Face::Last:
        throw std::invalid_argument("FromNormalizedInfoTo3d: Last is not a valid face");
    }
}
