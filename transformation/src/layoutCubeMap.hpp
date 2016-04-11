#pragma once

#include "layoutCubeMapBased.hpp"

namespace IMT {

class LayoutCubeMap: public LayoutCubeMapBased
{
    public:
        LayoutCubeMap(unsigned int outWidth):
        LayoutCubeMapBased(outWidth, 2*outWidth/3), m_cubeEdge(outWidth/3) {}
        virtual ~LayoutCubeMap(void) = default;


        Faces From2dToFace(unsigned int i, unsigned int j) const;

        virtual NormalizedFaceInfo From2dToNormalizedFaceInfo(const CoordI& pixel) const override;
        virtual CoordF FromNormalizedInfoTo2d(const NormalizedFaceInfo& ni) const override;


    private:
        unsigned int m_cubeEdge;

};

}
