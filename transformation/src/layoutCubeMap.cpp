#include "layoutCubeMap.hpp"

using namespace IMT;

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
            return cv::Point3f(-normalizedJ, normalizedI, cubeEdge);
        case LayoutCubeMap::Face::TOP:
            return cv::Point3f(normalizedJ, normalizedI, -cubeEdge);
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

CoordF LayoutCubeMap::fromSphereTo2d(float theta, float phi) const
{
   return CoordF();
}
