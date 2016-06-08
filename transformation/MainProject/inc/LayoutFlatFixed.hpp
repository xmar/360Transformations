#pragma once
#include "Layout.hpp"

namespace IMT {
class LayoutFlatFixed: public Layout
{
    public:
        //yaw pitch = theta and phi of the center of the view port; horizontalAngleOfVision (in radian) is the total angle view by this view port. Vertical deduced from the width, height and horizontalAngleOfVision with the hypothesis we keep the same ratio angle per pixel for the vertical
        LayoutFlatFixed(double yaw, double pitch, double roll, unsigned int width, unsigned int height, double horizontalAngleOfVision): Layout(width,height), m_yaw(yaw), m_roll(roll), m_pitch(pitch),
            m_horizontalAngleOfVision(horizontalAngleOfVision), m_verticalAngleOfVision(height*horizontalAngleOfVision/width),
            m_rotationMat(GetRotMatrice(yaw,pitch,roll)), m_maxHDist(2.f*std::tan(m_horizontalAngleOfVision/2)),
            m_maxVDist(2.f*std::tan(m_verticalAngleOfVision/2)) {}
        virtual ~LayoutFlatFixed(void) = default;

        virtual CoordI GetReferenceResolution(void) override
        {
            return CoordI(GetWidth(), GetHeight());
        }
    protected:
        virtual NormalizedFaceInfo From2dToNormalizedFaceInfo(const CoordI& pixel) const override;
        virtual CoordF FromNormalizedInfoTo2d(const NormalizedFaceInfo& ni) const override;
        virtual NormalizedFaceInfo From3dToNormalizedFaceInfo(const Coord3dSpherical& sphericalCoord) const override;
        virtual Coord3dCart FromNormalizedInfoTo3d(const NormalizedFaceInfo& ni) const override;

        virtual std::shared_ptr<Picture> ReadNextPictureFromVideoImpl(void) override;
        virtual void WritePictureToVideoImpl(std::shared_ptr<Picture>) override;
        virtual std::shared_ptr<IMT::LibAv::VideoReader> InitInputVideoImpl(std::string pathToInputVideo, unsigned nbFrame) override;
        virtual std::shared_ptr<IMT::LibAv::VideoWriter> InitOutputVideoImpl(std::string pathToOutputVideo, std::string codecId, unsigned fps, unsigned gop_size, std::vector<unsigned> bit_rateVect) override;
    private:
        double m_yaw;
        double m_pitch;
        double m_roll;
        double m_horizontalAngleOfVision;
        double m_verticalAngleOfVision;
        RotMat m_rotationMat;
        double m_maxHDist;
        double m_maxVDist;
};
}
