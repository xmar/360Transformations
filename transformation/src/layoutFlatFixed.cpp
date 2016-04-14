#include "layoutFlatFixed.hpp"

using namespace IMT;

Layout::NormalizedFaceInfo LayoutFlatFixed::From2dToNormalizedFaceInfo(const CoordI& pixel) const
{

    double normalizedI = (double(pixel.x)/m_outWidth); //between 0 and 1
    double normalizedJ = 1-(double(pixel.y)/m_outHeight); //between 0 and 1
    return Layout::NormalizedFaceInfo(CoordF(normalizedI, normalizedJ), 0);
}
CoordF LayoutFlatFixed::FromNormalizedInfoTo2d(const Layout::NormalizedFaceInfo& ni) const
{
    const CoordF& coord(ni.m_normalizedFaceCoordinate);
    return CoordF(coord.x*m_outWidth, (1-coord.y)*m_outHeight);
}
Layout::NormalizedFaceInfo LayoutFlatFixed::From3dToNormalizedFaceInfo(const Coord3dSpherical& sphericalCoord) const
{
    const double& theta(sphericalCoord.y);
    const double& phi(sphericalCoord.z);
    //First compute intersection with the plan
    double d = std::cos(m_yaw)*std::sin(m_pitch) * std::cos(theta)*std::sin(phi) + std::sin(m_yaw)*std::sin(m_pitch) * std::sin(theta)*std::sin(phi) + std::cos(m_pitch) * std::cos(phi);
    double x = std::cos(theta)*std::sin(phi) / d;
    double y = std::sin(theta) * std::sin(phi) / d;
    double z = std::cos(phi) / d;

    //Coordination of the intersection =
    Coord3dCart inter (x ,y , z);
    //Go back to the plan x=1
    auto rotInter = Rotation(inter, -m_yaw, -m_pitch, m_roll);

    double maxHDist = 2.f*std::tan(m_horizontalAngleOfVision/2);
    double maxVDist = 2.f*std::tan(m_verticalAngleOfVision/2);
    double normalizedI = rotInter.y/maxHDist + 0.5f; //between 0;1
    double normalizedJ = rotInter.z/maxVDist + 0.5f;
    return Layout::NormalizedFaceInfo(CoordF(normalizedI, normalizedJ), 0);
}
Coord3dCart LayoutFlatFixed::FromNormalizedInfoTo3d(const Layout::NormalizedFaceInfo& ni) const
{
    const double maxHDist = 2.f*std::tan(m_horizontalAngleOfVision/2);
    const double maxVDist = 2.f*std::tan(m_verticalAngleOfVision/2);
    const CoordF& coord(ni.m_normalizedFaceCoordinate);
    Coord3dCart coordBefRot(1.f, (coord.x-0.5)*maxHDist, (coord.y-0.5)*maxVDist);//coordinate in the plan x=1
    return Rotation(coordBefRot, m_yaw, m_pitch, m_roll);
}
