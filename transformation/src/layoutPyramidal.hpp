#pragma once

#include "layoutPyramidalBased.hpp"

namespace IMT
{

class LayoutPyramidal: public LayoutPyramidalBased
{
    public:
        /** \brief Constructor of a LayoutPyramidalBased (compact layout)
         *
         *          3*h
         *   +-----+-----+-----+
         *   |    /|     |\    |
         *   |  /  |     |  \  |
         *   |/  L |  Ba |  R \|  h
         *   |\    |     |    /|
         *   |  \  |     |  /  |
         *   | T  \|     |/  Bo|
         *   +-----+-----+-----+
         * \param baseEdge double
         * \param yaw double
         * \param pitch double
         * \param roll double
         * \param height unsigned int Number of pixel of the height of the global picture
         *
         */
        LayoutPyramidal(double baseEdge,double yaw, double pitch, double roll, unsigned int height):
        LayoutPyramidalBased(baseEdge, yaw, pitch, roll, 3*height, height) {}
        virtual ~LayoutPyramidal(void) = default;


        virtual NormalizedFaceInfo From2dToNormalizedFaceInfo(const CoordI& pixel) const override;
        virtual CoordF FromNormalizedInfoTo2d(const NormalizedFaceInfo& ni) const override;


        //Face AngleToFace(double theta, double phi) const { AngleToFace(Coord3dSpherical(1,theta,phi));}
        //Face AngleToFace(const Coord3dSpherical& p) const {return AngleToFaceCart(SphericalToCart(p));}
        //Face AngleToFaceCart(const Coord3dCart& p) const;

    private:

};

}
