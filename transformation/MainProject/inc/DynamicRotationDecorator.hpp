#pragma once

#include "LayoutDecorator.hpp"
#include "dynamicPosition.hpp"

namespace IMT
{
class DynamicRotationDecorator: public LayoutDecorator
{
    public:
        DynamicRotationDecorator(std::shared_ptr<LayoutView> baseLayout, DynamicPosition dynamicPosition): LayoutDecorator(baseLayout),
            m_dynamicPosition(std::move(dynamicPosition)) {}
    protected:

        NormalizedFaceInfo From3dToNormalizedFaceInfo(const Coord3dSpherical& sphericalCoord) const  override;
        Coord3dCart FromNormalizedInfoTo3d(const NormalizedFaceInfo& ni) const override;
        void NextStepImpl(double relatifTimestamp) override;
    private:
        DynamicPosition m_dynamicPosition;
};

class DecoratorConfigParserDynamicRotation: public DecoratorConfigParserBase
{
    public:
        DecoratorConfigParserDynamicRotation(std::string key): DecoratorConfigParserBase(key),
            m_pathToHeadMovemetTrace(this, "pathToHeadMovemetTrace", "path to a head movement trace file. Same format as here: http://dash.ipv6.enstb.fr/headMovements/", false, "")
         {}

        std::shared_ptr<LayoutView> Create(std::shared_ptr<LayoutView> baseLayout, std::string layoutSection, pt::ptree& ptree) const override
        {
            auto dynaRot = m_pathToHeadMovemetTrace.GetValue(layoutSection, ptree);
            return std::make_shared<DynamicRotationDecorator>(baseLayout, DynamicPosition(dynaRot));
        }
    private:
        KeyTypeDescription<std::string> m_pathToHeadMovemetTrace;
};

}
