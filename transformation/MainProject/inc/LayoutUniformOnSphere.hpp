#pragma once

#include "Layout.hpp"
#include "sphere_655362.hpp"

namespace IMT {
class LayoutUniformOnSphere: public Layout
{
    public:
        LayoutUniformOnSphere(Quaternion rotationQuaternion, std::shared_ptr<VectorialTrans> vectorialTrans):
            Layout(1584, 414, vectorialTrans),  m_rotationQuaternion(rotationQuaternion) {}
        virtual ~LayoutUniformOnSphere(void) = default;

        virtual CoordI GetReferenceResolution(void) override
        {
            return CoordI(GetWidth(), GetHeight());
        }

    protected:
        virtual NormalizedFaceInfo From2dToNormalizedFaceInfo(const CoordI& pixel) const override
        {
            return NormalizedFaceInfo(CoordF(double(pixel.x)/(GetWidth()-1), double(pixel.y)/GetHeight()), 0);
        }
        virtual CoordF FromNormalizedInfoTo2d(const NormalizedFaceInfo& ni) const override
        {
            return CoordF(ni.m_normalizedFaceCoordinate.x*GetWidth(), ni.m_normalizedFaceCoordinate.y * GetHeight());
        }
        virtual NormalizedFaceInfo From3dToNormalizedFaceInfo(const Coord3dSpherical& sphericalCoord) const override
        {
            return NormalizedFaceInfo(CoordF(0.5, 0.5), 0);
        }
        virtual Coord3dCart FromNormalizedInfoTo3d(const NormalizedFaceInfo& ni) const override
        {
            int i = ni.m_normalizedFaceCoordinate.x * (GetWidth()-1);
            int j = ni.m_normalizedFaceCoordinate.y * GetHeight();
            int p = i + j*GetWidth();
            //int p = i*GetHeight()+j;
            Coord3dSpherical v0(1, uniformPointOneSphere[2*p+1]*PI()/180.f, uniformPointOneSphere[2*p]*PI()/180.f+PI()/2.f);

            auto v = Rotation(v0, m_rotationQuaternion);
            return v;
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

        virtual std::shared_ptr<IMT::LibAv::VideoWriter> InitOutputVideoImpl(std::string pathToOutputVideo, std::string codecId, unsigned fps, unsigned gop_size, std::vector<int> bit_rateVect) override
        {
            std::shared_ptr<IMT::LibAv::VideoWriter> vwPtr = std::make_shared<IMT::LibAv::VideoWriter>(pathToOutputVideo);
            vwPtr->Init<1>(codecId, {{m_outWidth}}, {{m_outHeight}}, fps, gop_size, {{bit_rateVect[0]}});
            return vwPtr;
        }
    private:
        Quaternion m_rotationQuaternion;
};
}
