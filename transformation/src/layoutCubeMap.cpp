#include "layoutCubeMap.hpp"

using namespace IMT;

constexpr bool inIntervalStrict(float x, float a, float b)
{//true if a < x < b
    return (a <= b) ? (x > a) && (x < b) : inIntervalStrict(x, b, a);
}
constexpr bool inInterval(float x, float a, float b)
{//true if a < x < b
    return (x == a) || (x == b) || inIntervalStrict(x ,a ,b);
}

static cv::Point3f Get3dPoint(int i, int j, float cubeEdge, int nbRowPixels, int nbColPixels, LayoutCubeMap::Face f)
{
    float normalizedI = ((2.f*float(i)/nbColPixels)-1.f)*cubeEdge;
    float normalizedJ = ((2.f*float(j)/nbRowPixels)-1.f)*cubeEdge;
    switch (f)
    {
        case LayoutCubeMap::Face::FRONT:
            return cv::Point3f(cubeEdge, normalizedI, normalizedJ);
        case LayoutCubeMap::Face::BACK:
            return cv::Point3f(-cubeEdge, -normalizedI, normalizedJ);
        case LayoutCubeMap::Face::BOTTOM:
            return cv::Point3f(normalizedJ, -normalizedI, cubeEdge);
        case LayoutCubeMap::Face::TOP:
            return cv::Point3f(-normalizedJ, -normalizedI, -cubeEdge);
        case LayoutCubeMap::Face::LEFT:
            return cv::Point3f(-normalizedI, cubeEdge, normalizedJ);
        case LayoutCubeMap::Face::RIGHT:
            return cv::Point3f(normalizedI, -cubeEdge, normalizedJ);
    }
}

LayoutCubeMap::Face LayoutCubeMap::from2dToFace(unsigned int i, unsigned int j) const
{
    //From
    if (i < m_cubeEdge && j < m_cubeEdge)
    {
        return Face::FRONT;
    }
    else if (i < m_cubeEdge && j < 2*m_cubeEdge)
    {
        return Face::BACK;
    }
    else if (i < 2*m_cubeEdge && j < m_cubeEdge)
    {
        return Face::LEFT;
    }
    else if (i < 2*m_cubeEdge && j < 2*m_cubeEdge)
    {
        return Face::RIGHT;
    }
    else if (i < 3*m_cubeEdge && j < m_cubeEdge)
    {
        return Face::TOP;
    }
    else if (i < 3*m_cubeEdge && j < 2*m_cubeEdge)
    {
        return Face::BOTTOM;
    }
}

cv::Point3f LayoutCubeMap::from2dTo3d(unsigned int i, unsigned int j) const
{
    Face f = from2dToFace(i,j);
    return Get3dPoint(i%m_cubeEdge, j%m_cubeEdge, 2.0f, m_cubeEdge, m_cubeEdge, f);
}

LayoutCubeMap::Face LayoutCubeMap::AnglesToFace(float theta, float phi) const
{// theta in [-pi, pi] and Phi in [0, pi]
    if (inInterval(theta, -PI()/4.f, PI()/4.f) && inInterval(phi, PI()/4.f, 3.f*PI()/4.f))
    {
        return Face::FRONT;
    }
    else if ((inInterval(theta, -PI(), -3.f*PI()/4.f) || inInterval(theta, 3*PI()/4.f, PI())) && inInterval(phi, PI()/4.f, 3.f*PI()/4.f))
    {
        return Face::BACK;
    }
    else if (inInterval(theta, PI()/4.f, 3.f*PI()/4.f) && inInterval(phi, PI()/4.f, 3.f*PI()/4.f))
    {
        return Face::LEFT;
    }
    else if (inInterval(theta, -3.f*PI()/4.f, -PI()/4.f) && inInterval(phi, PI()/4.f, 3.f*PI()/4.f))
    {
        return Face::RIGHT;
    }
    else if (inInterval(phi, 0.f, PI()/4.f))
    {
        return Face::TOP;
    }
    else
    {
        return Face::BOTTOM;
    }
}

CoordF LayoutCubeMap::fromSphereTo2d(float theta, float phi) const
{// theta in [-pi, pi] and Phi in [0, pi]
    auto f = AnglesToFace(theta, phi);
    float x(0.f), y(0.f), z(0.f);
    switch (f)
    {
        case Face::FRONT:
            y = std::tan(theta);
            z = std::tan(PI()/2+phi)/std::cos(theta);
            //std::cout << "FRONT: y = "<<y << " z = " << z << "(i,j)="<< m_cubeEdge*(y+1.f)/2.f << ", " <<m_cubeEdge*(z+1.f)/2.f << std::endl;
            return CoordF(m_cubeEdge*(y+1.f)/2.f, m_cubeEdge*(z+1.f)/2.f);
        case Face::BACK:
            y = -std::tan(-theta);
            z = -std::tan(PI()/2+phi)/std::cos(-theta);
            //std::cout << "BACK" << std::endl;
            return CoordF(m_cubeEdge*(y+1.f)/2.f, m_cubeEdge*(z+1.f)/2.f+m_cubeEdge);
        case Face::LEFT:
            x = -std::tan(PI()/2-theta);
            z = std::tan(PI()/2+phi)/std::sin(theta);
            //std::cout << "LEFT" << std::endl;
            return CoordF(m_cubeEdge*(x+1.f)/2.f+m_cubeEdge, m_cubeEdge*(z+1.f)/2.f);
        case Face::RIGHT:
            x = -std::tan(PI()/2-theta);
            z = -std::tan(PI()/2+phi)/std::sin(theta);
            //std::cout << "RIGHT" << std::endl;
            return CoordF(m_cubeEdge*(x+1.f)/2.f+m_cubeEdge, m_cubeEdge*(z+1.f)/2.f+m_cubeEdge);
        case Face::TOP:
            x = std::cos(-theta)*std::tan(-phi);
            y = std::sin(-theta)*std::tan(-phi);
            //std::cout << "TOP" << std::endl;
            return CoordF(m_cubeEdge*(x+1.f)/2.f+2*m_cubeEdge, m_cubeEdge*(y+1.f)/2.f);
        case Face::BOTTOM:
            x = -std::cos(theta)*std::tan(-phi); 
            y = -std::sin(theta)*std::tan(-phi);
            //std::cout << "BOTTOM" << std::endl;
            return CoordF(m_cubeEdge*(x+1.f)/2.f+2*m_cubeEdge, m_cubeEdge*(y+1.f)/2.f+m_cubeEdge);
    }
    return CoordF();
}
