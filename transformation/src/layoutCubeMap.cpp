#include "layoutCubeMap.hpp"

using namespace IMT;


static Coord3dCart Get3dPoint(int i, int j, double cubeEdge, int nbRowPixels, int nbColPixels, LayoutCubeMap::Face f)
{
    double normalizedI = ((2.f*double(i)/nbColPixels)-1.f)*cubeEdge;
    double normalizedJ = ((2.f*double(j)/nbRowPixels)-1.f)*cubeEdge;
    switch (f)
    {
        case LayoutCubeMap::Face::FRONT:
            return cv::Point3f(cubeEdge, normalizedI, normalizedJ);
        case LayoutCubeMap::Face::BACK:
            return cv::Point3f(-cubeEdge, -normalizedI, normalizedJ);
        case LayoutCubeMap::Face::BOTTOM:
            return cv::Point3f(-normalizedJ, normalizedI, cubeEdge);
        case LayoutCubeMap::Face::TOP:
            return cv::Point3f(normalizedJ, -normalizedI, -cubeEdge);
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

Coord3dCart LayoutCubeMap::from2dTo3d(unsigned int i, unsigned int j) const
{
    Face f = from2dToFace(i,j);
    return Get3dPoint(i%m_cubeEdge, j%m_cubeEdge, 2.0f, m_cubeEdge, m_cubeEdge, f);
}

LayoutCubeMap::Face LayoutCubeMap::AnglesToFace(double theta, double phi) const
{// theta in [-pi, pi] and Phi in [0, pi]
    if (inInterval(theta, -PI()/4.0, PI()/4.0) && inInterval(phi, PI()/4.0, 3.0*PI()/4.0))
    {
        return Face::FRONT;
    }
    else if ((inInterval(theta, -PI(), -3.0*PI()/4.0) || inInterval(theta, 3*PI()/4.0, PI())) && inInterval(phi, PI()/4.0, 3.0*PI()/4.0))
    {
        return Face::BACK;
    }
    else if (inInterval(theta, PI()/4.0, 3.0*PI()/4.0) && inInterval(phi, PI()/4.0, 3.0*PI()/4.0))
    {
        return Face::LEFT;
    }
    else if (inInterval(theta, -3.0*PI()/4.0, -PI()/4.0) && inInterval(phi, PI()/4.0, 3.0*PI()/4.0))
    {
        return Face::RIGHT;
    }
    else if (inInterval(phi, 0.0, PI()/4.0))
    {
        return Face::BOTTOM;
    }
    else
    {
        return Face::TOP;
    }
}


CoordF LayoutCubeMap::fromSphereTo2d(double theta, double phi) const
{// theta in [-pi, pi] and Phi in [0, pi]
    auto f = AnglesToFace(theta, phi);
    double x(0), y(0), z(0);
    switch (f)
    {
        case Face::FRONT:
            y = std::tan(theta);
            z = std::tan(PI()/2-phi)/std::cos(theta);
            //std::cout << "FRONT: y = "<<y << " z = " << z << "(i,j)="<< m_cubeEdge*(y+1.0)/2.0 << ", " <<m_cubeEdge*(z+1.0)/2.0 << std::endl;
            return CoordF(m_cubeEdge*((y+1.0)/2.0), m_cubeEdge*((z+1.0)/2.0));
        case Face::BACK:
            y = std::tan(theta);
            z = -std::tan(PI()/2-phi)/std::cos(theta);
            //std::cout << "BACK" << std::endl;
            return CoordF(m_cubeEdge*(y+1.0)/2.0, m_cubeEdge*(z+1.0)/2.0+m_cubeEdge);
        case Face::LEFT:
            x = -std::tan(PI()/2-theta);
            z = std::tan(PI()/2-phi)/std::sin(theta);
            //std::cout << "LEFT" << std::endl;
            return CoordF(m_cubeEdge*(x+1.0)/2.0+m_cubeEdge, m_cubeEdge*(z+1.0)/2.0);
        case Face::RIGHT:
            x = -std::tan(PI()/2-theta);
            z = -std::tan(PI()/2-phi)/std::sin(theta);
            //std::cout << "RIGHT" << std::endl;
            return CoordF(m_cubeEdge*(x+1.0)/2.0+m_cubeEdge, m_cubeEdge*(z+1.0)/2.0+m_cubeEdge);
        case Face::TOP:
            x = -std::cos(theta)*std::tan(phi);
            y = std::sin(theta)*std::tan(phi);
            //std::cout << "TOP" << std::endl;
            return CoordF(m_cubeEdge*(y+1.0)/2.0+2*m_cubeEdge, m_cubeEdge*(x+1.0)/2.0);
        case Face::BOTTOM:
            x = -std::cos(theta)*std::tan(phi); 
            y = std::sin(theta)*std::tan(phi);
            //std::cout << "BOTTOM" << std::endl;
            return CoordF(m_cubeEdge*(y+1.0)/2.0+2*m_cubeEdge, m_cubeEdge*(x+1.0)/2.0+m_cubeEdge);
    }
    return CoordF();
}
