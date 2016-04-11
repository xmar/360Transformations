#include "layoutRhombicdodeca.hpp"

using namespace IMT;


Layout::NormalizedFaceInfo LayoutRhombicdodeca::From2dToNormalizedFaceInfo(const CoordI& pixel) const
{
    auto f = LayoutToFace(pixel.x, pixel.y);
    CoordF normalizedCoord(double(pixel.x%m_faceHeight) / m_faceHeight, double(pixel.y%m_faceHeight) / m_faceHeight);
    return Layout::NormalizedFaceInfo(normalizedCoord, static_cast<int>(f));
}
#define BORDER(x) (MAX(0,MIN(m_faceHeight,x)))
CoordF LayoutRhombicdodeca::FromNormalizedInfoTo2d(const Layout::NormalizedFaceInfo& ni) const
{
    auto f = static_cast<Face>(ni.m_faceId);
    double i = BORDER(ni.m_normalizedFaceCoordinate.x*m_faceHeight);
    double j = BORDER(ni.m_normalizedFaceCoordinate.x*m_faceHeight);
    return CanonicLayoutCoordToLayoutCoord(i, j, f);
}
Layout::NormalizedFaceInfo LayoutRhombicdodeca::From3dToNormalizedFaceInfo(const Coord3dSpherical& sphericalCoord) const
{
    auto f = Face::Last;
    Coord3dCart inter;
    double minRho = std::numeric_limits<double>::max();
    Coord3dSpherical p(1, sphericalCoord.y, sphericalCoord.z);


    for (auto testF: get_range<LayoutRhombicdodeca::Face>())
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

    Coord3dCart canonicCoordinates = Rotation(inter, FaceToRotMat(f).t()); //do the inverse rotation to go back to Face1

    double normalizedI = (canonicCoordinates.z -std::sqrt(2)*canonicCoordinates.y +3/2)/2;
    double normalizedJ = (canonicCoordinates.z +std::sqrt(2)*canonicCoordinates.y +3/2)/2;

    return Layout::NormalizedFaceInfo(CoordF(normalizedI, normalizedJ), static_cast<int>(f));
}
Coord3dCart LayoutRhombicdodeca::FromNormalizedInfoTo3d(const Layout::NormalizedFaceInfo& ni) const
{
    const double& normalizedI = ni.m_normalizedFaceCoordinate.x;
    const double& normalizedJ = ni.m_normalizedFaceCoordinate.y;
    auto f = static_cast<Face>(ni.m_faceId);
    Coord3dCart canonicCoordinates = Coord3dCart(1,0,-1) + Coord3dCart(0, -std::sqrt(2)/2, 1) * normalizedI + Coord3dCart(0, std::sqrt(2)/2, 1) * normalizedJ;
    return Rotation(canonicCoordinates, FaceToRotMat(f));
}


RotMat LayoutRhombicdodeca::FaceToRotMat(Face f) const
{
    RotMat m(3,3);
    m = cv::Mat::zeros(3,3, m.type());
    switch(f)
    {
        case Face::Face1:
            m(0,0) = 1;
            m(1,1) = 1;
            m(2,2) = 1;
            break;
        case Face::Face2:
            m(0,0) = 1.0/2;
            m(0,1) = std::sqrt(2)/2;
            m(0,2) = -1.0/2;
            m(1,0) = std::sqrt(2)/2;
            m(1,1) = 0;
            m(1,2) = std::sqrt(2)/2;
            m(2,0) = 1.0/2;
            m(2,1) = -std::sqrt(2)/2;
            m(2,2) = -1.0/2;
            break;
        case Face::Face3:
            m(0,0) = 1.0/2;
            m(0,1) = -std::sqrt(2)/2;
            m(0,2) = -1.0/2;
            m(1,0) = std::sqrt(2)/2;
            m(1,1) = 0;
            m(1,2) = std::sqrt(2)/2;
            m(2,0) = -1.0/2;
            m(2,1) = -std::sqrt(2)/2;
            m(2,2) = 1.0/2;
            break;
        case Face::Face4:
            m(0,0) = 1.0/2;
            m(0,1) = std::sqrt(2)/2;
            m(0,2) = 1.0/2;
            m(1,0) = -std::sqrt(2)/2;
            m(1,1) = 0;
            m(1,2) = std::sqrt(2)/2;
            m(2,0) = 1.0/2;
            m(2,1) = -std::sqrt(2)/2;
            m(2,2) = 1.0/2;
            break;
        case Face::Face5:
            m(0,0) = 1.0/2;
            m(0,1) = -std::sqrt(2)/2;
            m(0,2) = 1.0/2;
            m(1,0) = -std::sqrt(2)/2;
            m(1,1) = 0;
            m(1,2) = std::sqrt(2)/2;
            m(2,0) = -1.0/2;
            m(2,1) = -std::sqrt(2)/2;
            m(2,2) = -1.0/2;
            break;
        case Face::Face6:
            m(0,2) = 1;
            m(1,1) = 1;
            m(2,0) = -1;
            break;
        case Face::Face7:
            m(0,0) = -1.0/2;
            m(0,1) = std::sqrt(2)/2;
            m(0,2) = 1.0/2;
            m(1,0) = -std::sqrt(2)/2;
            m(1,1) = 0;
            m(1,2) = -std::sqrt(2)/2;
            m(2,0) = -1.0/2;
            m(2,1) = -std::sqrt(2)/2;
            m(2,2) = 1.0/2;
            break;
        case Face::Face8:
            m(0,0) = -1.0/2;
            m(0,1) = -std::sqrt(2)/2;
            m(0,2) = 1.0/2;
            m(1,0) = std::sqrt(2)/2;
            m(1,1) = 0;
            m(1,2) = std::sqrt(2)/2;
            m(2,0) = -1.0/2;
            m(2,1) = std::sqrt(2)/2;
            m(2,2) = 1.0/2;
            break;
        case Face::Face9:
            m(0,0) = -1.0/2;
            m(0,1) = -std::sqrt(2)/2;
            m(0,2) = -1.0/2;
            m(1,0) = std::sqrt(2)/2;
            m(1,1) = 0;
            m(1,2) = -std::sqrt(2)/2;
            m(2,0) = 1.0/2;
            m(2,1) = -std::sqrt(2)/2;
            m(2,2) = 1.0/2;
            break;
        case Face::Face10:
            m(0,0) = -1.0/2;
            m(0,1) = std::sqrt(2)/2;
            m(0,2) = -1.0/2;
            m(1,0) = -std::sqrt(2)/2;
            m(1,1) = 0;
            m(1,2) = std::sqrt(2)/2;
            m(2,0) = 1.0/2;
            m(2,1) = std::sqrt(2)/2;
            m(2,2) = 1.0/2;
            break;
        case Face::Face11:
            m(0,2) = -1;
            m(1,1) = 1;
            m(2,0) = 1;
            break;
        case Face::Face12:
            m(0,0) = -1;
            m(1,1) = -1;
            m(2,2) = 1;
            break;
        case Face::Last:
            throw std::invalid_argument("FaceToRotMat: Last is not a valid face");
    }
    return m;
}

LayoutRhombicdodeca::Face LayoutRhombicdodeca::LayoutToFace(unsigned int i, unsigned int j) const
{
    if (inInterval(i, 0, m_faceHeight) && inInterval(j, 0, m_faceHeight))
    {//Face1
        return Face::Face1;
    }
    else if (inInterval(i, 0, m_faceHeight) && inInterval(j, m_faceHeight, 2*m_faceHeight))
    {//Face2
        return Face::Face2;
    }
    else if (inInterval(i, m_faceHeight, 2*m_faceHeight) && inInterval(j, 0, m_faceHeight))
    {//Face3
        return Face::Face3;
    }
    else if (inInterval(i, m_faceHeight, 2*m_faceHeight) && inInterval(j, m_faceHeight, 2*m_faceHeight))
    {//Face4
        return Face::Face4;
    }
    else if (inInterval(i, 2*m_faceHeight, 3*m_faceHeight) && inInterval(j, 0, m_faceHeight))
    {//Face5
        return Face::Face5;
    }
    else if (inInterval(i, 2*m_faceHeight, 3*m_faceHeight) && inInterval(j, m_faceHeight, 2*m_faceHeight))
    {//Face6
        return Face::Face6;
    }
    else if (inInterval(i, 3*m_faceHeight, 4*m_faceHeight) && inInterval(j, 0, m_faceHeight))
    {//Face7
        return Face::Face7;
    }
    else if (inInterval(i, 3*m_faceHeight, 4*m_faceHeight) && inInterval(j, m_faceHeight, 2*m_faceHeight))
    {//Face8
        return Face::Face8;
    }
    else if (inInterval(i, 4*m_faceHeight, 5*m_faceHeight) && inInterval(j, 0, m_faceHeight))
    {//Face9
        return Face::Face9;
    }
    else if (inInterval(i, 4*m_faceHeight, 5*m_faceHeight) && inInterval(j, m_faceHeight, 2*m_faceHeight))
    {//Face10
        return Face::Face10;
    }
    else if (inInterval(i, 5*m_faceHeight, 6*m_faceHeight) && inInterval(j, 0, m_faceHeight))
    {//Face11
        return Face::Face11;
    }
    else if (inInterval(i, 5*m_faceHeight, 6*m_faceHeight) && inInterval(j, m_faceHeight, 2*m_faceHeight))
    {//Face12
        return Face::Face12;
    }
}

CoordF LayoutRhombicdodeca::CanonicLayoutCoordToLayoutCoord(double i, double j, Face f) const
{
    switch (f)
    {
        case Face::Face1:
            return CoordF(i,j);
        case Face::Face2:
            return CoordF(i,j+m_faceHeight);
        case Face::Face3:
            return CoordF(i+m_faceHeight,j);
        case Face::Face4:
            return CoordF(i+m_faceHeight,j+m_faceHeight);
        case Face::Face5:
            return CoordF(i+2*m_faceHeight,j);
        case Face::Face6:
            return CoordF(i+2*m_faceHeight,j+m_faceHeight);
        case Face::Face7:
            return CoordF(i+3*m_faceHeight,j);
        case Face::Face8:
            return CoordF(i+3*m_faceHeight,j+m_faceHeight);
        case Face::Face9:
            return CoordF(i+4*m_faceHeight,j);
        case Face::Face10:
            return CoordF(i+4*m_faceHeight,j+m_faceHeight);
        case Face::Face11:
            return CoordF(i+5*m_faceHeight,j);
        case Face::Face12:
            return CoordF(i+5*m_faceHeight,j+m_faceHeight);
        case Face::Last:
            throw std::invalid_argument("CanonicLayoutCoordToLayoutCoord: Last is not a valid face");
    }

}
