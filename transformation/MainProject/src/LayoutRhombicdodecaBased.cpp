#include "LayoutRhombicdodecaBased.hpp"

using namespace IMT;


Layout::NormalizedFaceInfo LayoutRhombicdodecaBased::From3dToNormalizedFaceInfo(const Coord3dSpherical& sphericalCoord) const
{
    Coord3dSpherical sc = Rotation(sphericalCoord, m_rotQuaternion.Inv()); //Go back to the normalized rhombic

    FaceToPlanFct<Faces> lambda = [this] (Faces f) {return this->FaceToPlan(f);};
    auto rtr = IntersectionCart(lambda, sc);
    Coord3dCart inter = std::get<0>(rtr);
    Faces f = std::get<1>(rtr);

    Coord3dCart canonicCoordinates = Rotation(inter, FaceToRotQuaternion(f).Inv()); //do the inverse rotation to go back to Face1

    double normalizedI = (canonicCoordinates.GetZ() -std::sqrt(2)*canonicCoordinates.GetY() +3/2)/2;
    double normalizedJ = (canonicCoordinates.GetZ() +std::sqrt(2)*canonicCoordinates.GetY() +3/2)/2;

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
    return Rotation(canonicCoordinates, m_rotQuaternion*FaceToRotQuaternion(f));
}

void LayoutRhombicdodecaBased::InitFaceRotations(void)
{
    for (auto f: get_range<LayoutRhombicdodecaBased::Faces>())
    {
        Quaternion m;
        switch(f)
        {
            case Faces::Face1:
                m_faceRotations[static_cast<unsigned>(f)] = Quaternion(1);
                break;
            case Faces::Face2:
                m_faceRotations[static_cast<unsigned>(f)] =
                    // RotMat ( 1.0/2, std::sqrt(2)/2, -1.0/2,
                    //         std::sqrt(2)/2, 0, std::sqrt(2)/2,
                    //         1.0/2, -std::sqrt(2)/2, -1.0/2
                    //         );
                    Quaternion(0.5, VectorCartesian(std::sqrt(2)/2, 0.5, 0));
                break;
            case Faces::Face3:
                m_faceRotations[static_cast<unsigned>(f)] =
                    // RotMat ( 1.0/2, -std::sqrt(2)/2, -1.0/2,
                    //         std::sqrt(2)/2, 0, std::sqrt(2)/2,
                    //         -1.0/2, -std::sqrt(2)/2, 1.0/2
                    //         );
                    Quaternion(std::sqrt(2)/2, VectorCartesian(0.5, 0, 0.5));
                break;
            case Faces::Face4:
                m_faceRotations[static_cast<unsigned>(f)] =
                    // RotMat ( 1.0/2, std::sqrt(2)/2, 1.0/2,
                    //         -std::sqrt(2)/2, 0, std::sqrt(2)/2,
                    //         1.0/2, -std::sqrt(2)/2, 1.0/2
                    //         );
                    Quaternion(2.0/3.0, VectorCartesian(0.4714045207886067, 1.0/3.0, 0.4714045207886067));
                break;
            case Faces::Face5:
                m_faceRotations[static_cast<unsigned>(f)] =
                    // RotMat ( 1.0/2, -std::sqrt(2)/2, 1.0/2,
                    //         -std::sqrt(2)/2, 0, std::sqrt(2)/2,
                    //         -1.0/2, -std::sqrt(2)/2, -1.0/2
                    //         );
                    Quaternion(0.5, VectorCartesian(std::sqrt(2)/2, -0.5, 0));
                break;
            case Faces::Face6:
                m_faceRotations[static_cast<unsigned>(f)] =
                    // RotMat ( 0, 0, -1,
                    //         0, 1, 0,
                    //         -1, 0, 0
                    //         );
                    Quaternion(std::sqrt(2)/2, VectorCartesian(0, -std::sqrt(2)/2, 0));
                break;
            case Faces::Face7:
                m_faceRotations[static_cast<unsigned>(f)] =
                    // RotMat ( -1.0/2, std::sqrt(2)/2, 1.0/2,
                    //         -std::sqrt(2)/2, 0, -std::sqrt(2)/2,
                    //         -1.0/2, -std::sqrt(2)/2, 1.0/2
                    //         );
                    Quaternion(0.5, VectorCartesian(0, -0.5, std::sqrt(2)/2));
                break;
            case Faces::Face8:
                m_faceRotations[static_cast<unsigned>(f)] =
                    // RotMat ( -1.0/2, -std::sqrt(2)/2, 1.0/2,
                    //         std::sqrt(2)/2, 0, std::sqrt(2)/2,
                    //         -1.0/2, std::sqrt(2)/2, 1.0/2
                    //         );
                    Quaternion(-0.5, VectorCartesian(0, 0.5, std::sqrt(2)/2));
                break;
            case Faces::Face9:
                m_faceRotations[static_cast<unsigned>(f)] =
                    // RotMat ( -1.0/2, -std::sqrt(2)/2, -1.0/2,
                    //         std::sqrt(2)/2, 0, -std::sqrt(2)/2,
                    //         1.0/2, -std::sqrt(2)/2, 1.0/2
                    //         );
                    Quaternion(-0.5, VectorCartesian(0, -0.5, std::sqrt(2)/2));
                break;
            case Faces::Face10:
                m_faceRotations[static_cast<unsigned>(f)] =
                    // RotMat ( -1.0/2, std::sqrt(2)/2, -1.0/2,
                    //         -std::sqrt(2)/2, 0, std::sqrt(2)/2,
                    //         1.0/2, std::sqrt(2)/2, 1.0/2
                    //         );
                    Quaternion(0.5, VectorCartesian(0, 0.5, std::sqrt(2)/2));
                break;
            case Faces::Face11:
                m_faceRotations[static_cast<unsigned>(f)] =
                    // RotMat ( 0, 0, -1,
                    //         0, 1, 0,
                    //         1, 0, 0
                    //         );
                    Quaternion(std::sqrt(2)/2, VectorCartesian(0, std::sqrt(2)/2, 0));
                break;
            case Faces::Face12:
                m_faceRotations[static_cast<unsigned>(f)] =
                    // RotMat ( -1, 0, 0,
                    //         0, -1, 0,
                    //         0, 0, 1
                    //         );
                    Quaternion(0, VectorCartesian(0, 0, 1));
                break;
            case Faces::Black:
            case Faces::Last:
                continue;
        }
    }
}

const Quaternion& LayoutRhombicdodecaBased::FaceToRotQuaternion(Faces f) const
{
    if (f == Faces::Black)
    {
        throw std::invalid_argument("FaceToRotQuaternion: Black is not a valid face");
    }
    if (f == Faces::Last)
    {
        throw std::invalid_argument("FaceToRotQuaternion: Last is not a valid face");
    }
    return m_faceRotations[static_cast<unsigned>(f)];
}
