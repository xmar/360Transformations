#include "DynamicRotationDecorator.hpp"

using namespace IMT;

DynamicRotationDecorator::NormalizedFaceInfo DynamicRotationDecorator::From3dToNormalizedFaceInfo(const Coord3dSpherical& sphericalCoord) const
{
    return LayoutDecorator::From3dToNormalizedFaceInfo(Rotation(sphericalCoord, m_dynamicPosition.GetNextPosition().Inv()));
}

Coord3dCart DynamicRotationDecorator::FromNormalizedInfoTo3d(const DynamicRotationDecorator::NormalizedFaceInfo& ni) const
{
    return Rotation(LayoutDecorator::FromNormalizedInfoTo3d(ni), m_dynamicPosition.GetNextPosition());
}

void DynamicRotationDecorator::NextStepImpl(double relatifTimestamp)
{
    m_dynamicPosition.SetNextPosition(relatifTimestamp);
}

REGISTER_LAYOUT_DECORATOR("dynamic_rotation", DecoratorConfigParserDynamicRotation);
