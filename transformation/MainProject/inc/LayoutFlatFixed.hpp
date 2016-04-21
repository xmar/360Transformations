#pragma once
#include "Layout.hpp"

namespace IMT {
class LayoutFlatFixed: public Layout
{
    public:
        //yaw pitch = theta and phi of the center of the view port; horizontalAngleOfVision (in radian) is the total angle view by this view port. Vertical deduced from the width, height and horizontalAngleOfVision with the hypothesis we keep the same ratio angle per pixel for the vertical
        LayoutFlatFixed(double yaw, double pitch, double roll, unsigned int width, unsigned int height, double horizontalAngleOfVision): Layout(width,height), m_yaw(yaw), m_roll(roll), m_pitch(pitch),
            m_horizontalAngleOfVision(horizontalAngleOfVision), m_verticalAngleOfVision(height*horizontalAngleOfVision/width) {}
        virtual ~LayoutFlatFixed(void) = default;

        virtual NormalizedFaceInfo From2dToNormalizedFaceInfo(const CoordI& pixel) const override;
        virtual CoordF FromNormalizedInfoTo2d(const NormalizedFaceInfo& ni) const override;
        virtual NormalizedFaceInfo From3dToNormalizedFaceInfo(const Coord3dSpherical& sphericalCoord) const override;
        virtual Coord3dCart FromNormalizedInfoTo3d(const NormalizedFaceInfo& ni) const override;
    private:
        double m_yaw;
        double m_pitch;
        double m_roll;
        double m_horizontalAngleOfVision;
        double m_verticalAngleOfVision;
};
}
