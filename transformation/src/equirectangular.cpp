#include "equirectangular.hpp"
#include <cmath>

using namespace IMT;

Pixel Equirectangular::GetPixelFromAngle(const CoordF& cf) const
{
    /* cf is (theta, phi) in radian (theta between [-pi,pi] and phi between [0,pi])
     *theta -> -pi              0                  pi
     *    0   +-----------------*-------------------+ 0
     *  phi   |                                     | y
     *        |                                     |
     *        |                                     |
     *        |                                     |
     *        |                                     |
     *        |                                     |
     *        |                                     |
     *   pi   +-----------------+-------------------+ H
     *   x -> 0                                     W = 2.H
     */
    float theta = cf.x;
    float phi = PI() - cf.y;
    //if (phi > PI()/2)
    //{
    //    phi -= PI();
    //}
    float x = m_pictMat.cols*(theta + PI())/(2*PI());
    float y = m_pictMat.rows*(phi/PI());
    return  GetInterPixel(CoordF(x,y));
}
