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
    auto f = static_cast<LayoutBarrel::Faces>(ni.m_faceId);
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
   Coord3dSpherical p = sphericalCoord; 
   if (p.GetPhi() >= (PI() - PI()*m_equirecRatio)/2.0 && p.GetPhi() <= PI()-(PI() - PI()*m_equirecRatio)/2.0)
   {// EquiRec
        return NormalizedFaceInfo(CoordF(p.GetTheta()/(2*PI())+0.5, (p.GetPhi()-(PI() - PI()*m_equirecRatio)/2.0)/(PI()*m_equirecRatio)), static_cast<int>(Faces::Equirec));
   }
   else if (p.GetPhi() < (PI() - PI()*m_equirecRatio)/2.0)
   {//Top
        Coord3dCart pp = p/Coord3dCart(p).GetZ();
        return NormalizedFaceInfo(CoordF(std::tan(m_equirecRatio*PI()/2)*pp.GetX()+0.5, std::tan(m_equirecRatio*PI()/2)*pp.GetY()+0.5), static_cast<int>(Faces::Top));
   }
   else
   {//Bottom
        Coord3dCart pp = p/(-Coord3dCart(p).GetZ());
        return NormalizedFaceInfo(CoordF(std::tan(m_equirecRatio*PI()/2)*pp.GetX()+0.5, std::tan(m_equirecRatio*PI()/2)*pp.GetY()+0.5), static_cast<int>(Faces::Bottom));
   }
}

Coord3dCart LayoutBarrel::FromNormalizedInfoTo3d(const LayoutBarrel::NormalizedFaceInfo& ni) const
{
    Faces f = static_cast<Faces>(ni.m_faceId);
    Coord3dCart v;
    if (f == Faces::Black) {return Coord3dCart(0,0,0);}
    if (f == Faces::Equirec)
    {
        v =  Coord3dSpherical(1, (ni.m_normalizedFaceCoordinate.x-0.5)*2*PI(), ni.m_normalizedFaceCoordinate.y*PI()*m_equirecRatio+(PI() - PI()*m_equirecRatio)/2.0);
    }
    if (f == Faces::Top)
    {
        v = Coord3dCart((ni.m_normalizedFaceCoordinate.x-0.5)/std::tan(m_equirecRatio*PI()/2), (ni.m_normalizedFaceCoordinate.y-0.5)/std::tan(m_equirecRatio*PI()/2), 1);
        v /= v.Norm();
    }
    if (f == Faces::Bottom)
    {
        v = Coord3dCart((ni.m_normalizedFaceCoordinate.x-0.5)/std::tan(m_equirecRatio*PI()/2), (ni.m_normalizedFaceCoordinate.y-0.5)/std::tan(m_equirecRatio*PI()/2), -1);
        v /= v.Norm();
    }
    return v;
}

REGISTER_LAYOUT("barrel", LayoutConfigParserBarrel);
