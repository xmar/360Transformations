#pragma once

#include <opencv2/opencv.hpp>
#include <cmath>
#include <memory>
#include "Picture.hpp"
#include "VideoReader.hpp"
#include "VideoWriter.hpp"

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
        Layout(void): m_outWidth(0), m_outHeight(0), m_isInit(false), m_inputVideoPtr(nullptr), m_outputVideoPtr(nullptr) {};
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


        unsigned int GetWidth(void) const {return m_outWidth;}
        unsigned int GetHeight(void) const {return m_outHeight;}

        /** \brief Return the reference width and height of this layout. The reference width and height is what is used to compute relative size from on picture to an other
         */
        virtual CoordI GetReferenceResolution(void) = 0;

        //transform the layoutPic that is a picture in the current layout into a picture with the layout destLayout with the dimention (width, height)
        std::shared_ptr<Picture> ToLayout(const Picture& layoutPic, const Layout& destLayout) const;
        std::shared_ptr<Picture> FromLayout(const Picture& picFromOtherLayout, const Layout& originalLayout) const
        {return originalLayout.ToLayout(picFromOtherLayout, *this);}

        void InitInputVideo(std::string pathToInputVideo, unsigned nbFrame)
        {
            std::cout << "Call input init frm base layout" << std::endl;
            if (m_inputVideoPtr == nullptr)
            {
                std::cout << "Call input init frm base layout: step 2" << std::endl;
                m_inputVideoPtr = InitInputVideoImpl(pathToInputVideo, nbFrame);
            }
        }
        void InitOutputVideo(std::string pathToOutputVideo, std::string codecId, unsigned fps, unsigned gop_size, std::vector<unsigned> bit_rateVect)
        {
            if (m_outputVideoPtr == nullptr)
            {
                m_outputVideoPtr = InitOutputVideoImpl(pathToOutputVideo, codecId, fps, gop_size, bit_rateVect);
            }
        }
        std::shared_ptr<Picture> ReadNextPictureFromVideo(void)
        {
            if (m_inputVideoPtr != nullptr)
            {
                return ReadNextPictureFromVideoImpl();
            }
            else
            {
                return nullptr;
            }
        }
        void WritePictureToVideo(std::shared_ptr<Picture> pic)
        {
            if (m_outputVideoPtr != nullptr)
            {
                WritePictureToVideoImpl(pic);
            }
        }

    protected:
        unsigned int m_outWidth;
        unsigned int m_outHeight;
        bool m_isInit;
        std::shared_ptr<IMT::LibAv::VideoReader> m_inputVideoPtr;
        std::shared_ptr<IMT::LibAv::VideoWriter> m_outputVideoPtr;

        /** \brief Protected function called by Init to initialized the layout object. Can be override. By default do nothing.
         */
        virtual void InitImpl(void) {}
        virtual std::shared_ptr<Picture> ReadNextPictureFromVideoImpl(void)  = 0;
        virtual void WritePictureToVideoImpl(std::shared_ptr<Picture>)  = 0;
        virtual std::shared_ptr<IMT::LibAv::VideoReader> InitInputVideoImpl(std::string pathToInputVideo, unsigned nbFrame) = 0;
        virtual std::shared_ptr<IMT::LibAv::VideoWriter> InitOutputVideoImpl(std::string pathToOutputVideo, std::string codecId, unsigned fps, unsigned gop_size, std::vector<unsigned> bit_rateVect) = 0;
        void SetWidth(unsigned int w) {m_outWidth = w;}
        void SetHeight(unsigned int h) {m_outHeight = h;}

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
    private:
};


}
