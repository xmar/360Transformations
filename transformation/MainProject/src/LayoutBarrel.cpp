#include <LayoutBarrel.hpp>

#include <math.h>

using namespace IMT;

LayoutBarrel::NormalizedFaceInfo LayoutBarrel::From2dToNormalizedFaceInfo(const CoordI& pixel) const
{
    //Equirec
    if (pixel.x < GetWidth()-2*m_equirecRatio)
    {
        auto f = Faces::Equirec;
        return NormalizedFaceInfo( CoordF(SCALAR(pixel.x)/(GetWidth()-2*m_equirecRatio),SCALAR(pixel.y)/GetHeight()), static_cast<int>(f));
    }
    //Top
    else if ( (std::pow((pixel.x-m_centerTop.x),2) + std::pow((pixel.y-m_centerTop.y),2)) <= m_equirecRatio*m_equirecRatio )
    {
        return NormalizedFaceInfo(CoordF((pixel.x-m_centerTop.x)/(2*m_equirecRatio)+0.5, (pixel.y-m_centerTop.y)/(2*m_equirecRatio)+0.5), static_cast<int>(Faces::Top));
    }
    //Bottom
    else if ( (std::pow((pixel.x-m_centerBottom.x),2) + std::pow((pixel.y-m_centerBottom.y),2)) <= m_equirecRatio*m_equirecRatio )
    {
        return NormalizedFaceInfo(CoordF((pixel.x-m_centerBottom.x)/(2*m_equirecRatio)+0.5, (pixel.y-m_centerBottom.y)/(2*m_equirecRatio)+0.5), static_cast<int>(Faces::Bottom));
    }
    //black
    else
    {
        return NormalizedFaceInfo(CoordF(0,0), static_cast<int>(Faces::Black));
    }
}

CoordF LayoutBarrel::FromNormalizedInfoTo2d(const LayoutBarrel::NormalizedFaceInfo& ni) const
{
    Faces f = static_cast<LayoutCubeMapBased::Faces>(ni.m_faceId);
    const CoordF& coord (ni.m_normalizedFaceCoordinate);
    if (f == Faces::Equirec)
    {
        return CoordF(coord.x*(GetWidth()-2*m_equirecRatio), coord.y*GetHeight());
    }
    else if (f == Faces::Top)
    {
        return CoordF((coord.x-0.5)*(2*m_equirecRatio)+m_centerTop.x, (coord.y-0.5)*(2*m_equirecRatio)+m_centerTop.y);
    }
    else
    {
        return CoordF((coord.x-0.5)*(2*m_equirecRatio)+m_centerBottom.x, (coord.y-0.5)*(2*m_equirecRatio)+m_centerBottom.y);
    }
}

LayoutBarrel::NormalizedFaceInfo LayoutBarrel::From3dToNormalizedFaceInfo(const Coord3dSpherical& sphericalCoord) const
{
   Coord3dSpherical p = Coord3dCart(Rotation(sphericalCoord, m_rotQuaternion.Inv())); 
   if (p.GetPhi() >= (PI() - PI()*m_equirecRatio)/2.0 && p.GetPhi() <= PI()-(PI() - PI()*m_equirecRatio)/2.0)
   {// EquiRec
        NormalizedFaceInfo(CoordF(p.GetTheta()/(2*PI())+0.5, (p.GetPhi()-(PI() - PI()*m_equirecRatio)/2.0)/(PI()*m_equirecRatio)), static_cast<int>(Faces::Equirec));
   }
   else if (p.GetPhi() < (PI() - PI()*m_equirecRatio)/2.0)
   {//Top
   }
   else
   {//Bottom
   }
}

Coord3dCart LayoutBarrel::FromNormalizedInfoTo3d(const LayoutBarrel::NormalizedFaceInfo& ni) const
{
    Faces f = static_cast<Faces>(ni.m_faceId);
    if (f == Faces::Black) {return Coord3dCart(0,0,0);}
    double i = (ni.m_normalizedFaceCoordinate.x - 0.5)*2.f;
    double j = (ni.m_normalizedFaceCoordinate.y - 0.5)*2.f;

    return Rotation(v, m_rotQuaternion);
}
