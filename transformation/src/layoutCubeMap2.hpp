#pragma once

#include "layoutCubeMapBased.hpp"

namespace IMT {

class LayoutCubeMap2: public LayoutCubeMapBased
{
	public:
		LayoutCubeMap2(unsigned int outWidth):
		            LayoutCubeMapBased(outWidth, 3*outWidth/4), m_cubeEdge(outWidth/4) {}
        virtual ~LayoutCubeMap2(void) = default;


		Faces From2dToFace(unsigned int i, unsigned int j) const;

        virtual NormalizedFaceInfo From2dToNormalizedFaceInfo(const CoordI& pixel) const override;
        virtual CoordF FromNormalizedInfoTo2d(const NormalizedFaceInfo& ni) const override;


		private:
			unsigned int m_cubeEdge;
};

}
