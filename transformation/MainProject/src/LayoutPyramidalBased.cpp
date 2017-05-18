#include "LayoutPyramidalBased.hpp"

using namespace IMT;

double LayoutPyramidalBased::UsePlanEquation(double x) const //Use the plan equation to compute the second (non nul) variable knowing the value of x
{
    return -m_canonicTopPlan[3] - x * m_canonicTopPlan[0];
}

Layout::NormalizedFaceInfo LayoutPyramidalBased::From3dToNormalizedFaceInfo(const Coord3dSpherical& sphericalCoord) const
{
    Coord3dSpherical p = Rotation(sphericalCoord, m_rotQuaternion.Inv()) - m_vectorOffsetRatio*Coord3dCart(1, 0, 0);

    FaceToPlanFct<Faces> lambda = [this] (Faces f) {return this->FaceToPlan(f);};
    auto rtr = IntersectionCart(lambda, p);
    Coord3dCart inter = std::get<0>(rtr);
    Faces f = std::get<1>(rtr);

    double normalizedI(0);
    double normalizedJ(0);
    switch (f)
    {
        case Faces::Base:
            normalizedI = inter.GetY()/m_baseEdge+0.5;
            normalizedJ = -inter.GetZ()/m_baseEdge+0.5;
            break;
        case Faces::Left:
            normalizedI = (inter.GetX() + m_pyramidHeight)/(m_pyramidHeight+1);
            normalizedJ = -inter.GetZ()/(normalizedI*m_baseEdge)+0.5;
            break;
        case Faces::Right:
            normalizedI = (1-inter.GetX())/(m_pyramidHeight+1);
            normalizedJ = -inter.GetZ()/((1-normalizedI)*m_baseEdge)+0.5;
            break;
        case Faces::Top:
            normalizedJ = (inter.GetX() + m_pyramidHeight)/(m_pyramidHeight+1);
            normalizedI = inter.GetY()/(normalizedJ*m_baseEdge)+0.5;
            break;
        case Faces::Bottom:
            normalizedJ = (1-inter.GetX())/(m_pyramidHeight+1);
            normalizedI = inter.GetY()/((1-normalizedJ)*m_baseEdge)+0.5;
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
    Coord3dCart v;
    switch (f)
    {
    case Faces::Base:
        {
            Coord3dCart inter(1, (normalizedI-0.5)*m_baseEdge, (0.5 - normalizedJ)*m_baseEdge);
            return v = std::move(inter);
        }
    case Faces::Left:
        {
            double z = -normalizedI*(normalizedJ-0.5) * m_baseEdge;
            double x = (m_pyramidHeight+1.0)*normalizedI-m_pyramidHeight;
            double y = -UsePlanEquation(x);
            v = Coord3dCart(x,y,z);
        }
    case Faces::Top:
        {
            double y = (normalizedI-0.5)*normalizedJ * m_baseEdge;
            double x =  (m_pyramidHeight+1.0)*normalizedJ-m_pyramidHeight;
            double z = UsePlanEquation(x);
            v = Coord3dCart(x,y,z);
        }
    case Faces::Right:
        {
            double z = -((normalizedJ-0.5)*(1-normalizedI)) * m_baseEdge;
            double x = -(m_pyramidHeight+1.0)*normalizedI+1;
            double y = UsePlanEquation(x);
            v = Coord3dCart(x,y,z);
        }
    case Faces::Bottom:
        {
            double y = ((normalizedI-0.5)*(1-normalizedJ)) * m_baseEdge;
            double x = -(m_pyramidHeight+1.0)*normalizedJ+1;
            double z = -UsePlanEquation(x);
            v = Coord3dCart(x,y,z);
        }
    case Faces::Last:
        throw std::invalid_argument("FromNormalizedInfoTo3d: Last is not a valid face");
    case Faces::Black:
        return Coord3dCart(0,0,0);
    }
    if (m_vectorOffsetRatio != 0)
    {
      v = v/v.Norm();
      auto x = v.GetX();
      //We compute the norm of the original vector V0 (we know that after the addition of m_vectorOffsetRatio*(1,0,0) the norm of the vector should be 1)
      auto norm = -m_vectorOffsetRatio*x + std::sqrt(m_vectorOffsetRatio*m_vectorOffsetRatio*(x*x-1)+1);
      v = norm*v + m_vectorOffsetRatio*Coord3dCart(1, 0, 0);
    }
    return Rotation(v, m_rotQuaternion);
}
