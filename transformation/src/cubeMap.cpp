#include "cubeMap.hpp"
#include "equirectangular.hpp"
#include <math.h> 
#include <iterator>

using namespace IMT;

CubeMap CubeMap::FromEquirectangular(const Equirectangular& er, LayoutCubeMap& lcm)
{
    CubeMap cm(lcm, er.GetMat().type());

    for (auto i = 0; i < cm.GetMat().cols; ++i)
    {
        for (auto j = 0; j < cm.GetMat().rows; ++j)
        {
            auto polarP = lcm.from2dTo3dSpherical(i,j);
            cm.SetValue(CoordI(i,j), er.GetPixelFromAngle(CoordF(polarP.y,polarP.z)));
        }
    }
    return cm;
}
