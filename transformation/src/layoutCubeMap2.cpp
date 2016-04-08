#include "layoutCubeMap2.hpp"

using namespace IMT;

static Coord3dCart Get3dPoint(int i, int j, double cubeEdge, int nbRowPixels, int nbColPixels, LayoutCubeMap2::Face f)
{
    double normalizedI = ((2.f*double(i)/nbColPixels)-1.f)*cubeEdge;
    double normalizedJ = ((2.f*double(j)/nbRowPixels)-1.f)*cubeEdge;
    switch (f)
    {
        case LayoutCubeMap2::Face::FRONT:
            return cv::Point3f(cubeEdge, normalizedI, normalizedJ);
        case LayoutCubeMap2::Face::BACK:
            return cv::Point3f(-cubeEdge, -normalizedI, normalizedJ);
        case LayoutCubeMap2::Face::BOTTOM:
            return cv::Point3f(-normalizedJ, normalizedI, cubeEdge);
        case LayoutCubeMap2::Face::TOP:
            return cv::Point3f(normalizedJ, -normalizedI, -cubeEdge);
        case LayoutCubeMap2::Face::LEFT:
            return cv::Point3f(-normalizedI, cubeEdge, normalizedJ);
        case LayoutCubeMap2::Face::RIGHT:
            return cv::Point3f(normalizedI, -cubeEdge, normalizedJ);
        case LayoutCubeMap2::Face::BLACK:
            return cv::Point3f(0, 0, 0);
    }
}

LayoutCubeMap2::Face LayoutCubeMap2::from2dToFace(unsigned int i, unsigned int j) const
{
    //From
	if ( i < m_cubeEdge && j > m_cubeEdge && j < 2*m_cubeEdge)
    {
        return Face::RIGHT;
    }
    else if (i > m_cubeEdge && i < 2*m_cubeEdge && j < m_cubeEdge)
    {
        return Face::TOP;
    }
    else if (i > m_cubeEdge && i < 2*m_cubeEdge && j > m_cubeEdge && j < 2*m_cubeEdge)
    {
        return Face::FRONT;
    }
    else if (i > m_cubeEdge && i < 2*m_cubeEdge && j > 2*m_cubeEdge && j < 3*m_cubeEdge)
    {
        return Face::BOTTOM;
    }
    else if (i > 2*m_cubeEdge && i < 3*m_cubeEdge && j > m_cubeEdge && j < 2*m_cubeEdge)
    {
        return Face::LEFT;
    }
    else if (i > 3*m_cubeEdge && i < 4*m_cubeEdge && j > m_cubeEdge && j < 2*m_cubeEdge)
    {
        return Face::BACK;
    }
    else {
        return Face::BLACK;
    }
}

Coord3dCart LayoutCubeMap2::from2dTo3d(unsigned int i, unsigned int j) const
{
    Face f = from2dToFace(i,j);
    return Get3dPoint(i%m_cubeEdge, j%m_cubeEdge, 2.0f, m_cubeEdge, m_cubeEdge, f);
}

LayoutCubeMap2::Face LayoutCubeMap2::AnglesToFace(double theta, double phi) const
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

#define MIN(a,b) ( (a<=b) ? a : b)
#define MAX(a,b) ( (a>=b) ? a : b)
#define BORDER(x) ( MAX(1, MIN(m_cubeEdge-1, x)))

CoordF LayoutCubeMap2::fromSphereTo2d(double theta, double phi) const
{
    auto f = Face::Last;
    Coord3dCart inter;
    double minRho = std::numeric_limits<double>::max();
    Coord3dSpherical p(1, theta, phi);


    for (auto testF: get_range<LayoutCubeMap2::Face>())
    {
        try {
            auto plan = FaceToPlan(testF);
            auto interSphe = IntersectionPlanSpherical(plan, p); //raise exception if no intersection
            if (minRho > interSphe.x && AlmostEqual(interSphe.y, p.y)) //check direction
            {
                minRho = interSphe.x;
                inter = SphericalToCart(interSphe);
                f = testF;
            }
        } catch ( std::logic_error& le )
        { //no intersection with this face
            continue;
        }
    }
    switch (f)
    {
        case Face::FRONT:
            return CoordF(BORDER(m_cubeEdge*((inter.y+1.0)/2.0))+m_cubeEdge, BORDER(m_cubeEdge*((inter.z+1.0)/2.0))+m_cubeEdge);
        case Face::BACK:
            return CoordF(BORDER(m_cubeEdge*(-inter.y+1.0)/2.0)+3*m_cubeEdge, BORDER(m_cubeEdge*(inter.z+1.0)/2.0)+m_cubeEdge);
        case Face::LEFT:
            return CoordF(BORDER(m_cubeEdge*(1.0+inter.x)/2.0), BORDER(m_cubeEdge*(inter.z+1.0)/2.0)+m_cubeEdge);
        case Face::RIGHT:
            return CoordF(BORDER(m_cubeEdge*(-inter.x+1.0)/2.0)+2*m_cubeEdge, BORDER(m_cubeEdge*(inter.z+1.0)/2.0)+m_cubeEdge);
        case Face::TOP:
            return CoordF(BORDER(m_cubeEdge*(-inter.y+1.0)/2.0)+m_cubeEdge, BORDER(m_cubeEdge*(inter.x+1.0)/2.0));
        case Face::BOTTOM:
            return CoordF(BORDER(m_cubeEdge*(inter.y+1.0)/2.0)+m_cubeEdge, BORDER(m_cubeEdge*(-inter.x+1.0)/2.0)+2*m_cubeEdge);
        case Face::BLACK:
        	return CoordF(-1,-1);
    }
}

