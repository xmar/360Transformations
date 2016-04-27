#include "LayoutRhombicdodecaBased.hpp"

using namespace IMT;


Layout::NormalizedFaceInfo LayoutRhombicdodecaBased::From3dToNormalizedFaceInfo(const Coord3dSpherical& sphericalCoord) const
{
    auto f = Faces::Last;
    Coord3dCart inter;
    double minRho = std::numeric_limits<double>::max();

    Coord3dSpherical sc = Rotation(sphericalCoord, m_rotMat.t()); //Go back to the normalized rhombic

    for (auto testF: get_range<LayoutRhombicdodecaBased::Faces>())
    {
        try {
            auto plan = FaceToPlan(testF);
            auto interSphe = IntersectionPlanSpherical(plan, sc); //raise exception if no intersection
            if (minRho > interSphe.x && AlmostEqual(interSphe.y, sc.y)) //check direction
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
Coord3dCart LayoutRhombicdodecaBased::FromNormalizedInfoTo3d(const Layout::NormalizedFaceInfo& ni) const
{
    const double& normalizedI = ni.m_normalizedFaceCoordinate.x;
    const double& normalizedJ = ni.m_normalizedFaceCoordinate.y;
    auto f = static_cast<Faces>(ni.m_faceId);
    if (f == Faces::Black)
    {
        return Coord3dCart(0,0,0);
    }
    Coord3dCart canonicCoordinates = Coord3dCart(1,0,-1) + Coord3dCart(0, -std::sqrt(2)/2, 1) * normalizedI + Coord3dCart(0, std::sqrt(2)/2, 1) * normalizedJ;
    return Rotation(canonicCoordinates, m_rotMat*FaceToRotMat(f));
}

void LayoutRhombicdodecaBased::InitFaceRotations(void)
{
    for (auto f: get_range<LayoutRhombicdodecaBased::Faces>())
    {
        RotMat m(3,3);
        m = cv::Mat::zeros(3,3, m.type());
        switch(f)
        {
            case Faces::Face1:
                m(0,0) = 1;
                m(1,1) = 1;
                m(2,2) = 1;
                break;
            case Faces::Face2:
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
            case Faces::Face3:
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
            case Faces::Face4:
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
            case Faces::Face5:
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
            case Faces::Face6:
                m(0,2) = 1;
                m(1,1) = 1;
                m(2,0) = -1;
                break;
            case Faces::Face7:
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
            case Faces::Face8:
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
            case Faces::Face9:
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
            case Faces::Face10:
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
            case Faces::Face11:
                m(0,2) = -1;
                m(1,1) = 1;
                m(2,0) = 1;
                break;
            case Faces::Face12:
                m(0,0) = -1;
                m(1,1) = -1;
                m(2,2) = 1;
                break;
            case Faces::Black:
            case Faces::Last:
                continue;
        }
        m_faceRotations[static_cast<unsigned>(f)] = m;
    }
}

const RotMat& LayoutRhombicdodecaBased::FaceToRotMat(Faces f) const
{
    if (f == Faces::Black)
    {
        throw std::invalid_argument("FaceToRotMat: Black is not a valid face");
    }
    if (f == Faces::Last)
    {
        throw std::invalid_argument("FaceToRotMat: Last is not a valid face");
    }
    return m_faceRotations[static_cast<unsigned>(f)];
}
