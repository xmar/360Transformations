#include "RotationDecorator.hpp"

using namespace IMT;

RotationDecorator::NormalizedFaceInfo RotationDecorator::From3dToNormalizedFaceInfo(const Coord3dSpherical& sphericalCoord) const
{
    return LayoutDecorator::From3dToNormalizedFaceInfo(Rotation(sphericalCoord, m_rotationQuaternion.Inv()));
}

Coord3dCart RotationDecorator::FromNormalizedInfoTo3d(const RotationDecorator::NormalizedFaceInfo& ni) const
{
    return Rotation(LayoutDecorator::FromNormalizedInfoTo3d(ni), m_rotationQuaternion);
}

REGISTER_LAYOUT_DECORATOR("rotation", DecoratorConfigParserRotation);
