#pragma once

#include "Layout.hpp"

namespace IMT {
class LayoutEquirectangular: public Layout
{
    public:
        LayoutEquirectangular(unsigned int width, unsigned int height, Quaternion rotationQuaternion, std::shared_ptr<VectorialTrans> vectorialTrans):
            Layout(width, height, vectorialTrans),  m_rotationQuaternion(rotationQuaternion) {}
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
          // Coord3dSpherical rotCoord = Rotation(sphericalCoord, m_rotationQuaternion.Inv());
            Coord3dSpherical rotCoord = Rotation(Coord3dCart(sphericalCoord/sphericalCoord.Norm()), m_rotationQuaternion.Inv());
            // if (m_vectorOffsetRatio != 0)
            // {
            //   auto theta = rotCoord.GetTheta();
            //   auto phi = rotCoord.GetPhi();
            //   double thetaBis = std::atan2(std::sin(phi)*std::sin(theta), -m_vectorOffsetRatio + std::sin(phi)*std::cos(theta));
            //   double phiBis = std::acos(std::cos(phi)/std::sqrt(m_vectorOffsetRatio*(m_vectorOffsetRatio - std::sin(phi)*std::cos(theta))+1));
            //   return NormalizedFaceInfo(CoordF(0.5+thetaBis/ (2.0*PI()), phiBis/ PI()), 0);
            // }
            // else
            // {
              return NormalizedFaceInfo(CoordF(0.5+rotCoord.GetTheta()/ (2.0*PI()), rotCoord.GetPhi() / PI()), 0);
            // }
        }
        virtual Coord3dCart FromNormalizedInfoTo3d(const NormalizedFaceInfo& ni) const override
        {
            double theta = 2.0*PI()*(ni.m_normalizedFaceCoordinate.x-0.5);
            double phi = PI()*(ni.m_normalizedFaceCoordinate.y);
            // Coord3dSpherical p(1, theta, phi);
            // Coord3dSpherical v = Coord3dCart(p)+m_vectorOffsetRatio*Coord3dCart(1, 0, 0);
            // double thetaBis = std::atan2(std::sin(phi)*std::sin(theta), m_vectorOffsetRatio + std::sin(phi)*std::cos(theta));
            // double phiBis = std::acos(std::cos(phi)/std::sqrt(m_vectorOffsetRatio*m_vectorOffsetRatio + m_vectorOffsetRatio*std::sin(phi)*std::cos(theta)+1));
            // std::cout << Rotation(Coord3dSpherical(1, theta, phi)+m_vectorOffsetRatio*Coord3dCart(1, 0, 0), m_rotationQuaternion) << "; "<< Rotation(Coord3dSpherical(1, thetaBis, phiBis), m_rotationQuaternion) << std::endl;
            // return Rotation(Coord3dSpherical(1, thetaBis, phiBis), m_rotationQuaternion);
            Coord3dSpherical v0 = Coord3dSpherical(1, theta, phi);
            // + m_vectorOffsetRatio*Coord3dCart(1, 0, 0);

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

        virtual std::shared_ptr<IMT::LibAv::VideoWriter> InitOutputVideoImpl(std::string pathToOutputVideo, std::string codecId, unsigned fps, unsigned gop_size, std::vector<unsigned> bit_rateVect) override
        {
            std::shared_ptr<IMT::LibAv::VideoWriter> vwPtr = std::make_shared<IMT::LibAv::VideoWriter>(pathToOutputVideo);
            vwPtr->Init<1>(codecId, {{m_outWidth}}, {{m_outHeight}}, fps, gop_size, {{bit_rateVect[0]}});
            return vwPtr;
        }
    private:
        Quaternion m_rotationQuaternion;
};
}
