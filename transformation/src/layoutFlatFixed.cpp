#include "layoutFlatFixed.hpp"

using namespace IMT;

Coord3dCart Rotation(cv::Point3f& coordBefRot, double yaw, double pitch, double roll)
{
    double cosY = std::cos(yaw);
    double sinY = std::sin(yaw);
    double cosP = std::cos(pitch);
    double sinP = std::sin(pitch);
    double cosR = std::cos(roll);
    double sinR = std::sin(roll);
    double rotX = coordBefRot.x * cosP * cosY + coordBefRot.y*(cosY*sinP*sinR -sinY*cosR) + coordBefRot.z*(cosY*sinP*cosR + sinY * sinR);
    double rotY = coordBefRot.x * sinY*cosP + coordBefRot.y * cosY * cosR + coordBefRot.z *( sinY*sinP*cosR - sinR * cosY);
    double rotZ = coordBefRot.x * (-sinP) - coordBefRot.y * cosP * sinR + coordBefRot.z * cosP * cosR;

    return cv::Point3f(rotX, rotY, rotZ);
}

Coord3dCart LayoutFlatFixed::from2dTo3d(unsigned int i, unsigned int j) const
{
    //First we work on the plan x=1 we will do a rotation later
    //Horizontal limit:
    double maxHDist = 2.f*std::tan(m_horizontalAngleOfVision/2);
    double maxVDist = 2.f*std::tan(m_verticalAngleOfVision/2);
    double normalizedI = (double(i)/m_outWidth)-0.5f; //between -0.5 and 0.5
    double normalizedJ = (double(j)/m_outHeight)-0.5f; //between -0.5 and 0.5

    cv::Point3f coordBefRot(1.f, normalizedI*maxHDist, normalizedJ*maxVDist);//coordinate in the plan x=1
     
    return Rotation(coordBefRot, m_yaw, m_pitch, m_roll);
}

CoordF LayoutFlatFixed::fromSphereTo2d(double theta, double phi) const
{
    //First compute intersection with the plan
    double d = std::cos(m_yaw)*std::sin(m_pitch) * std::cos(theta)*std::sin(phi) + std::sin(m_yaw)*std::sin(m_pitch) * std::sin(theta)*std::sin(phi) + std::cos(m_pitch) * std::cos(phi);
    double x = std::cos(theta)*std::sin(phi) / d;
    double y = std::sin(theta) * std::sin(phi) / d;
    double z = std::cos(phi) / d;

    //Coordination of the intersection =
    cv::Point3f inter (x ,y , z);
    //Go back to the plan x=1
    auto rotInter = Rotation(inter, -m_yaw, -m_pitch, m_roll);

    double maxHDist = 2.f*std::tan(m_horizontalAngleOfVision/2);
    double maxVDist = 2.f*std::tan(m_verticalAngleOfVision/2);
    double normalizedI = rotInter.y/maxHDist + 0.5f; //between 0;1
    double normalizedJ = rotInter.z/maxVDist + 0.5f; //between 0;1
    return CoordF(normalizedI*m_outWidth, normalizedJ*m_outHeight);
}
