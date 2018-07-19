#pragma once

#include "Common.hpp"
#include "LayoutDecorator.hpp"

namespace IMT {
class VectorialTrans: public LayoutDecorator
{
    public:
        VectorialTrans(std::shared_ptr<LayoutView> baseLayout): LayoutDecorator(baseLayout) {};
        virtual ~VectorialTrans(void) = default;

    protected:
        NormalizedFaceInfo From3dToNormalizedFaceInfo(const Coord3dSpherical& sphericalCoord) const  final
        {
            return LayoutDecorator::From3dToNormalizedFaceInfo(FromAfterTrans3dToBeforeTrans3d(sphericalCoord));
        }
        Coord3dCart FromNormalizedInfoTo3d(const NormalizedFaceInfo& ni) const final
        {
            return FromBeforeTrans3dToAfterTrans3d(LayoutDecorator::FromNormalizedInfoTo3d(ni));
        }

    private:
        //FromBeforeTrans3dToAfterTrans3d From 2D to 3D
        virtual Coord3dCart FromBeforeTrans3dToAfterTrans3d(Coord3dCart vectBefore) const = 0;
        //FromAfterTrans3dToBeforeTrans3d From 3D to 2D
        virtual Coord3dCart FromAfterTrans3dToBeforeTrans3d(Coord3dCart vectAfter) const = 0;

};
}
