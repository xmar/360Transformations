#pragma once

#include "Layout.hpp"

namespace IMT {
class LayoutBarrel: public Layout
{
    public:
        LayoutBarrel(unsigned int width, unsigned int height, Quaternion rotationQuaternion, std::shared_ptr<VectorialTrans> vectorialTrans):
            Layout(width, height, vectorialTrans),  m_rotationQuaternion(rotationQuaternion), m_equirecRatio(1.0/3.0), m_circleRadius(SCALAR(height)/4.0), m_centerTop(width-m_circleRadius,m_circleRadius),
            m_centerBottom(width-m_circleRadius,3*m_circleRadius) {}
        virtual ~LayoutBarrel(void) = default;

        virtual CoordI GetReferenceResolution(void) override
        {
            return CoordI(GetWidth(), GetHeight());
        }

    protected:
        enum class Faces: int {Equirec, Top, Bottom, Black, Last, First=Equirec};

        virtual NormalizedFaceInfo From2dToNormalizedFaceInfo(const CoordI& pixel) const override;
        virtual CoordF FromNormalizedInfoTo2d(const NormalizedFaceInfo& ni) const override;
        virtual NormalizedFaceInfo From3dToNormalizedFaceInfo(const Coord3dSpherical& sphericalCoord) const override;
        virtual Coord3dCart FromNormalizedInfoTo3d(const NormalizedFaceInfo& ni) const override;

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

        virtual std::shared_ptr<IMT::LibAv::VideoWriter> InitOutputVideoImpl(std::string pathToOutputVideo, std::string codecId, unsigned fps, unsigned gop_size, std::vector<int> bit_rateVect) override
        {
            std::shared_ptr<IMT::LibAv::VideoWriter> vwPtr = std::make_shared<IMT::LibAv::VideoWriter>(pathToOutputVideo);
            vwPtr->Init<1>(codecId, {{m_outWidth}}, {{m_outHeight}}, fps, gop_size, {{bit_rateVect[0]}});
            return vwPtr;
        }
    private:
        Quaternion m_rotationQuaternion;
        SCALAR m_circleRadius;
        SCALAR m_equirecRatio;
        CoordF m_centerTop;
        CoordF m_centerBottom;
};
}
