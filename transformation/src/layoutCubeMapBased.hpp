#pragma once
#include "layout.hpp"
#include <stdexcept>

namespace IMT {

class LayoutCubeMapBased : public Layout
{
    public:
        enum class Faces: int {Front, Back, Top, Bottom, Left, Right, Black, Last, First=Front};
        LayoutCubeMapBased(unsigned int outWidth, unsigned int outHeight): Layout(outWidth, outHeight) {};
        virtual ~LayoutCubeMapBased(void) = default;

        virtual NormalizedFaceInfo From2dToNormalizedFaceInfo(const CoordI& pixel) const = 0;

        virtual CoordF FromNormalizedInfoTo2d(const NormalizedFaceInfo& ni) const = 0;

        virtual NormalizedFaceInfo From3dToNormalizedFaceInfo(const Coord3dSpherical& sphericalCoord) const final;

        virtual Coord3dCart FromNormalizedInfoTo3d(const NormalizedFaceInfo& ni) const final;

        Plan FromFaceToPlan(Faces f) const
        {
            switch(f)
            {
                case Faces::Front:
                    return Plan(-1,0,0,1);
                case Faces::Back:
                    return Plan(1,0,0,1);
                case Faces::Top:
                    return Plan(0,0,-1,1);
                case Faces::Bottom:
                    return Plan(0,0,1,1);
                case Faces::Right:
                    return Plan(0,-1,0,1);
                case Faces::Left:
                    return Plan(0,1,0,1);
                case Faces::Black:
                    return Plan(0,0,0,0);
                case Faces::Last:
                    throw std::invalid_argument("FaceToPlan: Last is not a valid face");

            }
        }

        RotMat FaceToRotMat(Faces f) const;
    protected:

    private:
};

}
