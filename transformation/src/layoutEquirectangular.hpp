#pragma once

#include "layout.hpp"

namespace IMT {
class LayoutEquirectangular: public Layout
{
    public:
        LayoutEquirectangular(unsigned int width, unsigned int height): Layout(width, height) {}
        virtual ~LayoutEquirectangular(void) = default;

        virtual Coord3dCart from2dTo3d(unsigned int i, unsigned int j) const override
        {
            auto coordSphe = from2dTo3dSpherical(i, j);
            auto& theta = coordSphe.y;
            auto& phi = coordSphe.z;
            return cv::Point3f(std::cos(theta)*std::sin(phi), std::sin(theta)*std::sin(phi), std::sin(theta));
        }

        virtual Coord3dSpherical from2dTo3dSpherical(unsigned int i, unsigned int j) const override
        {
            double theta = (double(i)/m_outWidth - 0.5)*2.0*PI();
            double phi = (1.0 - double(j)/m_outHeight)*PI();
            return cv::Point3f(1.0, theta, phi);
        }

        virtual CoordF fromSphereTo2d(double theta, double phi) const override
        {
            double i = ((theta / (2.0*PI())) +0.5)*m_outWidth;
            double j = (1.0 - phi / PI())*m_outHeight;
            return CoordF(i ,j);
        }

};
}
