#pragma once

#include "Layout.hpp"

namespace IMT {
class LayoutEquirectangular: public Layout
{
    public:
        LayoutEquirectangular(unsigned int width, unsigned int height): Layout(width, height) {}
        virtual ~LayoutEquirectangular(void) = default;

        virtual NormalizedFaceInfo From2dToNormalizedFaceInfo(const CoordI& pixel) const override
        {
            return NormalizedFaceInfo(CoordF(double(pixel.x)/GetWidth(), double(pixel.y)/GetHeight()), 0);
        }
        virtual CoordF FromNormalizedInfoTo2d(const NormalizedFaceInfo& ni) const override
        {
            return CoordF(ni.m_normalizedFaceCoordinate.x*GetWidth(), ni.m_normalizedFaceCoordinate.y * GetHeight());
        }
        virtual NormalizedFaceInfo From3dToNormalizedFaceInfo(const Coord3dSpherical& sphericalCoord) const override
        {
            return NormalizedFaceInfo(CoordF(0.5+sphericalCoord.y/ (2.0*PI()), sphericalCoord.z / PI()), 0);
        }
        virtual Coord3dCart FromNormalizedInfoTo3d(const NormalizedFaceInfo& ni) const override
        {
            double theta = (ni.m_normalizedFaceCoordinate.x-0.5)*2.0*PI();
            double phi = (ni.m_normalizedFaceCoordinate.y)*PI();
            return Coord3dSpherical(1, theta, phi);
        }
};
}
