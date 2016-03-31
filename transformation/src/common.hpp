#pragma once

#include <cmath>
#include <stdexcept>


namespace IMT {

typedef cv::Vec3b Pixel;
typedef cv::Point2d CoordF;
typedef cv::Point2i CoordI;
typedef cv::Point3d Coord3dCart;
typedef cv::Point3d Coord3dSpherical;
typedef cv::Vec4d Plan;// (a,b,c,d) a.x+b.y+c.z+d=0

constexpr double PI() {return std::atan(1)*4.0;}

constexpr bool inIntervalStrict(double x, double a, double b)
{//true if a < x < b
    return (a <= b) ? (x > a) && (x < b) : inIntervalStrict(x, b, a);
}

constexpr bool inInterval(double x, double a, double b)
{//true if a < x < b
    return (x == a) || (x == b) || inIntervalStrict(x ,a ,b);
}

inline Coord3dCart Rotation(const Coord3dCart& coordBefRot, double yaw, double pitch, double roll)
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

inline Coord3dSpherical CartToSherical(const Coord3dCart& coordCart)
{
    double rho = cv::norm(coordCart);
    if (rho > 0)
    {
        double theta = std::atan2(coordCart.y, coordCart.x); //between (-PI;PI)
        double phi = std::acos(coordCart.z / rho); //between (0, PI)
        return Coord3dSpherical(rho, theta, phi);
    }
    else
    {
        return Coord3dSpherical(0, 0, 0);
    }
}

inline Coord3dCart SphericalToCart(const Coord3dSpherical& coordSphe)
{
    double x = coordSphe.x*std::cos(coordSphe.y)*std::sin(coordSphe.z);
    double y = coordSphe.x*std::sin(coordSphe.y)*std::sin(coordSphe.z);
    double z = coordSphe.x*std::cos(coordSphe.z);
    return Coord3dCart(x,y,z);
}

inline Coord3dCart IntersectionPlan(const Plan& p, double theta, double phi)
{//Return the cartesian coordinate of the intersaction of the line v(1,theta, phi) in spherical coordinate as the dirtion vector and the plan a.x+b.y+c.z+d = 0
    //if no solution return (0,0,0)
    const double& a = p[0]; 
    const double& b = p[1];
    const double& c = p[2];
    const double& d = p[3];
    double cosT(std::cos(theta)), sinT(std::sin(theta)), cosP(std::cos(phi)), sinP(std::sin(phi));
    double u = a*cosT*sinP + b*sinT*sinP + c*cosP;
    if (u != 0)
    {
        double x = -d* cosT*sinP / u;
        double y = -d* sinT*sinP / u;
        double z = -d* cosP / u;
        return Coord3dCart(x, y, z);
    }
    else
    {
        throw std::logic_error("No intersection");
    }
}

inline Coord3dSpherical IntersectionPlanSpherical(const Plan& p, double theta, double phi)
{
    return CartToSherical(IntersectionPlan(p, theta, phi));
}

}    
     
     
 
     
