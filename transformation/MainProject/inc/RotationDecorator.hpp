#pragma once

#include "LayoutDecorator.hpp"

namespace IMT
{
class RotationDecorator: public LayoutDecorator
{
    public:
        RotationDecorator(std::shared_ptr<LayoutView> baseLayout, Quaternion rotationQuaternion): LayoutDecorator(baseLayout),
            m_rotationQuaternion(std::move(rotationQuaternion)) {}
    protected:

        NormalizedFaceInfo From3dToNormalizedFaceInfo(const Coord3dSpherical& sphericalCoord) const  override;
        Coord3dCart FromNormalizedInfoTo3d(const NormalizedFaceInfo& ni) const override;
    private:
        Quaternion m_rotationQuaternion;
};

class DecoratorConfigParserRotation: public DecoratorConfigParserBase
{
    public:
        DecoratorConfigParserRotation(std::string key): DecoratorConfigParserBase(key),
            m_rotationQuaternion(this, "rotation", "Rotation applied on the projection. [Default = no rotation]", true, Quaternion(1))
         {}

        std::shared_ptr<LayoutView> Create(std::shared_ptr<LayoutView> baseLayout, std::string layoutSection, pt::ptree& ptree) const override
        {
            auto rot = m_rotationQuaternion.GetRotation(layoutSection, ptree);
            std::cout << "Create rotation decorator with rot " << rot << std::endl;
            return std::make_shared<RotationDecorator>(baseLayout, m_rotationQuaternion.GetRotation(layoutSection, ptree));
        }
    private:
        KeyRotationDescription m_rotationQuaternion;
};

}
