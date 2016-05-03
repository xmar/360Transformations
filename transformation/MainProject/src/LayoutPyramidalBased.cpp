#include "LayoutPyramidalBased.hpp"

using namespace IMT;

double LayoutPyramidalBased::UsePlanEquation(double x) const //Use the plan equation to compute the second (non nul) variable knowing the value of x
{
    return -m_canonicTopPlan[3] - x * m_canonicTopPlan[0];
}

Layout::NormalizedFaceInfo LayoutPyramidalBased::From3dToNormalizedFaceInfo(const Coord3dSpherical& sphericalCoord) const
{
    Coord3dSpherical p = Rotation(sphericalCoord, m_rotationMat.t());

    FaceToPlanFct<Faces> lambda = [this] (Faces f) {return this->FaceToPlan(f);};
    auto rtr = IntersectionCart(lambda, p);
    Coord3dCart inter = std::get<0>(rtr);
    Faces f = std::get<1>(rtr);

    double normalizedI(0);
    double normalizedJ(0);
    switch (f)
    {
        case Faces::Base:
            normalizedI = inter.y/m_baseEdge+0.5;
            normalizedJ = -inter.z/m_baseEdge+0.5;
            break;
        case Faces::Left:
            normalizedI = (inter.x + m_pyramidHeight)/(m_pyramidHeight+1);
            normalizedJ = -inter.z/(normalizedI*m_baseEdge)+0.5;
            break;
        case Faces::Right:
            normalizedI = (1-inter.x)/(m_pyramidHeight+1);
            normalizedJ = -inter.z/((1-normalizedI)*m_baseEdge)+0.5;
            break;
        case Faces::Top:
            normalizedJ = (inter.x + m_pyramidHeight)/(m_pyramidHeight+1);
            normalizedI = inter.y/(normalizedJ*m_baseEdge)+0.5;
            break;
        case Faces::Bottom:
            normalizedJ = (1-inter.x)/(m_pyramidHeight+1);
            normalizedI = inter.y/((1-normalizedJ)*m_baseEdge)+0.5;
            break;
         case Faces::Black:
         case Faces::Last:
            {
               std::cout << "Error: no intersection detected..." << std::endl;
               return Layout::NormalizedFaceInfo(CoordF(0, 0),static_cast<int>(Faces::Last));
            }
    }
    return Layout::NormalizedFaceInfo(CoordF(normalizedI, normalizedJ),static_cast<int>(f));
}
Coord3dCart LayoutPyramidalBased::FromNormalizedInfoTo3d(const NormalizedFaceInfo& ni) const
{
    auto f = static_cast<Faces>(ni.m_faceId);
    const CoordF& coord (ni.m_normalizedFaceCoordinate);
    const double& normalizedI (coord.x);
    const double& normalizedJ (coord.y);
    switch (f)
    {
    case Faces::Base:
        {
            Coord3dCart inter(1, (normalizedI-0.5)*m_baseEdge, (0.5 - normalizedJ)*m_baseEdge);
            return Rotation(inter, m_rotationMat);
        }
    case Faces::Left:
        {
            double z = -normalizedI*(normalizedJ-0.5) * m_baseEdge;
            double x = (m_pyramidHeight+1.0)*normalizedI-m_pyramidHeight;
            double y = -UsePlanEquation(x);
            return Rotation(Coord3dCart(x,y,z), m_rotationMat);
        }
    case Faces::Top:
        {
            double y = (normalizedI-0.5)*normalizedJ * m_baseEdge;
            double x =  (m_pyramidHeight+1.0)*normalizedJ-m_pyramidHeight;
            double z = UsePlanEquation(x);
            return Rotation(Coord3dCart(x,y,z), m_rotationMat);
        }
    case Faces::Right:
        {
            double z = -((normalizedJ-0.5)*(1-normalizedI)) * m_baseEdge;
            double x = -(m_pyramidHeight+1.0)*normalizedI+1;
            double y = UsePlanEquation(x);
            return Rotation(Coord3dCart(x,y,z), m_rotationMat);
        }
    case Faces::Bottom:
        {
            double y = ((normalizedI-0.5)*(1-normalizedJ)) * m_baseEdge;
            double x = -(m_pyramidHeight+1.0)*normalizedJ+1;
            double z = -UsePlanEquation(x);
            return Rotation(Coord3dCart(x,y,z), m_rotationMat);
        }
    case Faces::Last:
        throw std::invalid_argument("FromNormalizedInfoTo3d: Last is not a valid face");
    case Faces::Black:
        return Coord3dCart(0,0,0);
    }
}


