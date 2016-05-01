#pragma once

#include "Layout.hpp"

namespace IMT {
class LayoutEquirectangular: public Layout
{
    public:
        LayoutEquirectangular(unsigned int width, unsigned int height, double yaw, double pitch, double roll):
            Layout(width, height),  m_rotationMatrice(GetRotMatrice(yaw, pitch, roll)) {}
        virtual ~LayoutEquirectangular(void) = default;

        virtual CoordI GetReferenceResolution(void) override
        {
            return CoordI(GetWidth(), GetHeight());
        }

    protected:
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
            Coord3dSpherical rotCoord = Rotation(sphericalCoord, m_rotationMatrice.t());
            return NormalizedFaceInfo(CoordF(0.5+rotCoord.y/ (2.0*PI()), rotCoord.z / PI()), 0);
        }
        virtual Coord3dCart FromNormalizedInfoTo3d(const NormalizedFaceInfo& ni) const override
        {
            double theta = (ni.m_normalizedFaceCoordinate.x-0.5)*2.0*PI();
            double phi = (ni.m_normalizedFaceCoordinate.y)*PI();
            return Rotation(Coord3dSpherical(1, theta, phi), m_rotationMatrice);
        }
    private:
        RotMat m_rotationMatrice;
};
}
