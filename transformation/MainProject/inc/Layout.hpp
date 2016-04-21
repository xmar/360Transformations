#pragma once

#include <opencv2/opencv.hpp>
#include <cmath>
#include <memory>
#include "Picture.hpp"

#include "Common.hpp"

namespace IMT {
class Layout
{
    /* This virtual class is used to define any kind of output layout */
    public:
        /**< Structure to store normalized information about a face */
        struct NormalizedFaceInfo
        {
            NormalizedFaceInfo(CoordF coord, int faceId): m_normalizedFaceCoordinate(coord), m_faceId(faceId) {}
            /**< m_normalizedFaceCoordinate values MUST always be between 0 and 1 */
            CoordF m_normalizedFaceCoordinate;
            int m_faceId;
        };
        Layout(void): m_outWidth(0), m_outHeight(0), m_isInit(false) {};
        Layout(unsigned int outWidth, unsigned int outHeight): m_outWidth(outWidth), m_outHeight(outHeight), m_isInit(false) {};
        virtual ~Layout(void) = default;

        /*Return the 3D coordinate cartesian of the point corresponding to the pixel with coordinate pixelCoord on the 2d layout*/
        Coord3dCart From2dTo3d(const CoordI& pixelCoord) const;

        /*Return the coordinate of the 2d layout that correspond to the point on the sphere in shperical coordinate sphericalCoord*/
        CoordF FromSphereTo2d(const Coord3dSpherical& sphericalCoord) const;

        /** \brief Function called to init the layout object (have to be called before using the layout object). Call the private virtual function InitImpl.
         */
        void Init(void) {InitImpl(); m_isInit = true;}

        /** \brief If we need a dynamic layout that evolve with the time, this function should be overide to apply the evolution. By default do nothing.
         */
        virtual void NextStep(void) {}

        /** \brief return the normalized face information corresponding to the given coordinate of a pixel on the 2d layout
         *
         * \param pixel Coordinate of the pixel on the 2d layout
         * \return A NormalizedFaceInfo contening the normalized information about this pixel
         *
         */
        virtual NormalizedFaceInfo From2dToNormalizedFaceInfo(const CoordI& pixel) const = 0;

        /** \brief Compute the normalized face info corresponding to a given point on the unit sphere
         *
         * \param sphericalCoord Coord3dSpherical& the coordinate of the point in the spherical space
         * \return virtual NormalizedFaceInfo The normalized face info
         *
         */
        virtual NormalizedFaceInfo From3dToNormalizedFaceInfo(const Coord3dSpherical& sphericalCoord) const = 0;
        /** \brief Get the coordinate on the 2d layout from the normalized face info
         *
         * \param ni const NormalizedFaceInfo& The normalized info from which we want to find the pixel on the 2d layout
         * \return CoordF The coordinate in floating value of the corresponding pixel on the 2d layout
         *
         */
        virtual CoordF FromNormalizedInfoTo2d(const NormalizedFaceInfo& ni) const = 0;
        /** \brief Get the 3D cartesian coordinate from the normalized face info
         *
         * \param ni const NormalizedFaceInfo& the normalized face info
         * \return virtual Coord3dCart The coordinate of the corresponding point in the 3D cartesian space
         *
         */
        virtual Coord3dCart FromNormalizedInfoTo3d(const NormalizedFaceInfo& ni) const = 0;
        /** \brief Protect the border of the normalized
         *
         * \param ni const NormalizedFaceInfo&
         * \return virtual NormalizedFaceInfo
         *
         */

        unsigned int GetWidth(void) const {return m_outWidth;}
        unsigned int GetHeight(void) const {return m_outHeight;}


        //transform the layoutPic that is a picture in the current layout into a picture with the layout destLayout with the dimention (width, height)
        std::shared_ptr<Picture> ToLayout(const Picture& layoutPic, const Layout& destLayout) const;
        std::shared_ptr<Picture> FromLayout(const Picture& picFromOtherLayout, const Layout& originalLayout) const
        {return originalLayout.ToLayout(picFromOtherLayout, *this);}

    protected:
        unsigned int m_outWidth;
        unsigned int m_outHeight;
        bool m_isInit;

        /** \brief Protected function called by Init to initialized the layout object. Can be override. By default do nothing.
         */
        virtual void InitImpl(void) {}
        void SetWidth(unsigned int w) {m_outWidth = w;}
        void SetHeight(unsigned int h) {m_outHeight = h;}
    private:
};


}
