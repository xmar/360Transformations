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

        virtual std::shared_ptr<Picture> ReadNextPictureFromVideoImpl(void) override
        {
            auto matptr = m_inputVideoPtr->GetNextPicture(0);
            return std::make_shared<Picture>(matptr->clone());
        }

        virtual void WritePictureToVideoImpl(std::shared_ptr<Picture> pict) override
        {
            m_outputVideoPtr->Write( pict->GetMat(), 0);
        }

        virtual std::shared_ptr<IMT::LibAv::VideoReader> InitInputVideoImpl(std::string pathToInputVideo, unsigned nbFrame) override
        {
            std::shared_ptr<IMT::LibAv::VideoReader> vrPtr = std::make_shared<IMT::LibAv::VideoReader>(pathToInputVideo);
            vrPtr->Init(nbFrame);
            if (vrPtr->GetNbStream() != 1)
            {
                std::cout << "Unsupported number of stream for Equirectangular input video: "<<vrPtr->GetNbStream() <<" instead of 1" << std::endl;
                return nullptr;
            }
            //we could add some other check for instance on the width, height of each stream
            return vrPtr;
        }

        virtual std::shared_ptr<IMT::LibAv::VideoWriter> InitOutputVideoImpl(std::string pathToOutputVideo, std::string codecId, unsigned fps, unsigned gop_size, std::vector<unsigned> bit_rateVect) override
        {
            std::shared_ptr<IMT::LibAv::VideoWriter> vwPtr = std::make_shared<IMT::LibAv::VideoWriter>(pathToOutputVideo);
            vwPtr->Init<1>(codecId, {{m_outWidth}}, {{m_outHeight}}, fps, gop_size, {{bit_rateVect[0]}});
            return vwPtr;
        }
    private:
        RotMat m_rotationMatrice;
};
}
