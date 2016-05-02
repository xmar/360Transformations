#include "LayoutCubeMapBased.hpp"

using namespace IMT;

Coord3dCart LayoutCubeMapBased::FromNormalizedInfoTo3d(const Layout::NormalizedFaceInfo& ni) const
{
    Faces f = static_cast<Faces>(ni.m_faceId);
    if (f == Faces::Black) {return Coord3dCart(0,0,0);}
    double i = (ni.m_normalizedFaceCoordinate.x - 0.5)*2.f;
    double j = (0.5 - ni.m_normalizedFaceCoordinate.y)*2.f;
    Coord3dCart point(1, i, -j);
    return Rotation(point, m_rotMat*FaceToRotMat(f));
}


Layout::NormalizedFaceInfo LayoutCubeMapBased::From3dToNormalizedFaceInfo(const Coord3dSpherical& sphericalCoord) const
{
    //First we find the face with which we intersect
    auto f = Faces::Last;
    Coord3dCart inter;
    double minRho = std::numeric_limits<double>::max();
    Coord3dSpherical p = Rotation(sphericalCoord, m_rotMat.t());

    for (auto testF: get_range<LayoutCubeMapBased::Faces>())
    {
        try {
            auto plan = FromFaceToPlan(testF);
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
    Coord3dCart canonicPoint ( Rotation(inter, FaceToRotMat(f).t()) );
    Layout::NormalizedFaceInfo ni (CoordF((canonicPoint.y+1.0)/2.0, (canonicPoint.z+1.0)/2.0), static_cast<int>(f));
    return ni;
}

void LayoutCubeMapBased::InitFaceRotations(void)
{
    for (auto f: get_range<LayoutCubeMapBased::Faces>())
    {
        switch (f)
        {
            case Faces::Front:
                m_faceRotations[static_cast<unsigned>(f)] =
                    RotMat(1, 0, 0,
                           0, 1, 0,
                           0, 0, -1
                           );
                break;
            case Faces::Back:
                m_faceRotations[static_cast<unsigned>(f)] =
                    RotMat(-1, 0, 0,
                           0, -1, 0,
                           0, 0, -1
                           );
                break;
            case Faces::Left:
                m_faceRotations[static_cast<unsigned>(f)] =
                    RotMat(0, 1, 0,
                           -1, 0, 0,
                           0, 0, -1
                           );
                break;
            case Faces::Right:
                m_faceRotations[static_cast<unsigned>(f)] =
                    RotMat(0, -1, 0,
                           1, 0, 0,
                           0, 0, -1
                           );
                break;
            case Faces::Top:
                m_faceRotations[static_cast<unsigned>(f)] =
                    RotMat(0, 0, 1,
                           0, 1, 0,
                           1, 0, 0
                           );
                break;
            case Faces::Bottom:
                m_faceRotations[static_cast<unsigned>(f)] =
                    RotMat(0, 0, -1,
                           0, 1, 0,
                           -1, 0, 0
                           );
                break;
            case Faces::Last:
            case Faces::Black:
                continue;
        }
    }
}

const RotMat& LayoutCubeMapBased::FaceToRotMat(Faces f) const
{
    if (f == Faces::Last || f == Faces::Black)
    {
        throw std::invalid_argument("FaceToRotMat: Last is not a valid face");
    }
    return m_faceRotations[static_cast<unsigned>(f)];
}
