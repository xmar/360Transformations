#pragma once

#include <opencv2/opencv.hpp>
#include <cmath>
#include <memory>
#include "picture.hpp"

#include "common.hpp"

namespace IMT {
class Equirectangular;
class Layout
{
    /* This virtual class is used to define any kind of output layout */
    public:
        Layout(unsigned int outWidth, unsigned int outHeight): m_outWidth(outWidth), m_outHeight(outHeight) {};
        virtual ~Layout(void) = default;

        /*Return the 3D coordinate cartesian of the point corresponding to the pixel with coordinate (i,j) on the 2d layout*/
        virtual Coord3dCart from2dTo3d(unsigned int i, unsigned int j) const = 0;        
        
        /* Same as from2dTo3d except it return spherical coordinate */
        virtual Coord3dSpherical from2dTo3dSpherical(unsigned int i, unsigned int j) const
        {
            auto cartP = from2dTo3d(i,j);
            return CartToSpherical(cartP);
        }
   
        /*Return the coordinate of the 2d layout that correspond to the point on the sphere in shperical coordinate (1, theta, phi)*/
        virtual CoordF fromSphereTo2d(double theta, double phi) const = 0;
        CoordF fromSphereTo2d(const Coord3dSpherical& sphericalCoord) const { return fromSphereTo2d(sphericalCoord.y, sphericalCoord.z);}

        unsigned int GetWidth(void) const {return m_outWidth;}
        unsigned int GetHeight(void) const {return m_outHeight;}

        std::shared_ptr<Equirectangular> ToEquirectangular(const Picture& layoutPic, unsigned int width, unsigned int height) const;
        std::shared_ptr<Picture> FromEquirectangular(const Equirectangular& eq, unsigned int width, unsigned int height) const;
        //transform the layoutPic that is a picture in the current layout into a picture with the layout destLayout with the dimention (width, height)
        std::shared_ptr<Picture> ToLayout(const Picture& layoutPic, const Layout& destLayout) const;
        std::shared_ptr<Picture> FromLayout(const Picture& picFromOtherLayout, const Layout& originalLayout) const
        {return originalLayout.ToLayout(picFromOtherLayout, *this);}

    protected:
        unsigned int m_outWidth;
        unsigned int m_outHeight;
    private:
};


}
