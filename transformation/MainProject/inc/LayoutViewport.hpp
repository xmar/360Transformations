#pragma once
#include "Layout.hpp"
#include "dynamicPosition.hpp"

namespace IMT {
class LayoutViewport: public Layout
{
    public:
        //yaw pitch = theta and phi of the center of the view port; horizontalAngleOfVision (in radian) is the total angle view by this view port. Vertical deduced from the width, height and horizontalAngleOfVision with the hypothesis we keep the same ratio angle per pixel for the vertical
        LayoutViewport(unsigned int width, unsigned int height, double horizontalAngleOfVision, double verticalAngleOfVision): Layout(width,height),
            m_horizontalAngleOfVision(horizontalAngleOfVision),
            m_verticalAngleOfVision(verticalAngleOfVision), m_maxHDist(std::tan(m_horizontalAngleOfVision/2)),
            m_maxVDist(std::tan(m_verticalAngleOfVision/2)) {}
        virtual ~LayoutViewport(void) = default;

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
        virtual std::shared_ptr<IMT::LibAv::VideoWriter> InitOutputVideoImpl(std::string pathToOutputVideo, std::string codecId, unsigned fps, unsigned gop_size, std::vector<int> bit_rateVect) override;
    private:
        double m_horizontalAngleOfVision;
        double m_verticalAngleOfVision;
        double m_maxHDist;
        double m_maxVDist;
};

class LayoutConfigParserViewport: public LayoutConfigParser
{
    public:
        LayoutConfigParserViewport(std::string key): LayoutConfigParser(key),
            m_horizontalAngleOfVision(this, "horizontalAngleOfVision", "(Float) Angle in degree of the horizontal Field of View of the viewport", false),
            m_verticalAngleOfVision(this, "verticalAngleOfVision", "(Float) Angle in degree of the vertical Field of View of the viewport", false)
        {}

    protected:
        std::shared_ptr<LayoutView> CreateImpl(std::string layoutSection, pt::ptree& ptree) const override
        {
            return std::make_shared<LayoutViewport>(m_width.GetValue(layoutSection, ptree), m_height.GetValue(layoutSection, ptree), m_horizontalAngleOfVision.GetValue(layoutSection, ptree)*PI()/180.f, m_verticalAngleOfVision.GetValue(layoutSection, ptree)*PI()/180.f);
        }
    private:
        KeyTypeDescription<SCALAR> m_horizontalAngleOfVision;
        KeyTypeDescription<SCALAR> m_verticalAngleOfVision;
};


}
