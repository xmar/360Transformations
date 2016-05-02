#include "LayoutFlatFixed.hpp"

using namespace IMT;

Layout::NormalizedFaceInfo LayoutFlatFixed::From2dToNormalizedFaceInfo(const CoordI& pixel) const
{

    double normalizedI = (double(pixel.x)/m_outWidth); //between 0 and 1
    double normalizedJ = (double(pixel.y)/m_outHeight); //between 0 and 1
    return Layout::NormalizedFaceInfo(CoordF(normalizedI, normalizedJ), 0);
}
CoordF LayoutFlatFixed::FromNormalizedInfoTo2d(const Layout::NormalizedFaceInfo& ni) const
{
    const CoordF& coord(ni.m_normalizedFaceCoordinate);
    return CoordF(coord.x*m_outWidth, (coord.y)*m_outHeight);
}
Layout::NormalizedFaceInfo LayoutFlatFixed::From3dToNormalizedFaceInfo(const Coord3dSpherical& sphericalCoord) const
{
    /*const double& theta(sphericalCoord.y);
    const double& phi(sphericalCoord.z);
    //First compute intersection with the plan
    double d = std::cos(m_yaw)*std::sin(m_pitch) * std::cos(theta)*std::sin(phi) + std::sin(m_yaw)*std::sin(m_pitch) * std::sin(theta)*std::sin(phi) + std::cos(m_pitch) * std::cos(phi);
    double x = std::cos(theta)*std::sin(phi) / d;
    double y = std::sin(theta) * std::sin(phi) / d;
    double z = std::cos(phi) / d;

    //Coordination of the intersection =
    Coord3dCart inter (x ,y , z);
    //Go back to the plan x=1
    auto rotInter = Rotation(inter, m_rotationMat.t());

    double normalizedI = rotInter.y/m_maxHDist + 0.5f; //between 0;1
    double normalizedJ = rotInter.z/m_maxVDist + 0.5f;
    return Layout::NormalizedFaceInfo(CoordF(normalizedI, normalizedJ), 0);*/
    Coord3dSpherical cardPosition = Rotation(sphericalCoord, m_rotationMat.t());
    cardPosition.y = 0.5+cardPosition.y/m_horizontalAngleOfVision;
    cardPosition.z = 0.5+(cardPosition.z - PI()/2)/m_verticalAngleOfVision;
    return Layout::NormalizedFaceInfo(CoordF(cardPosition.y, cardPosition.z), 0);
}
Coord3dCart LayoutFlatFixed::FromNormalizedInfoTo3d(const Layout::NormalizedFaceInfo& ni) const
{
    const CoordF& coord(ni.m_normalizedFaceCoordinate);
    // Coord3dCart coordBefRot(1.f, (coord.x-0.5)*m_maxHDist, (coord.y-0.5)*m_maxVDist);//coordinate in the plan x=1
    Coord3dSpherical coordBefRot(1.f, (coord.x-0.5)*m_horizontalAngleOfVision, PI()/2+(coord.y-0.5)*m_verticalAngleOfVision);
    return Rotation(coordBefRot, m_rotationMat);
}
