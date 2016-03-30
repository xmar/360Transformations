#include "layoutPyramidal.hpp"
#include <limits>

using namespace IMT;


Coord3dCart LayoutPyramidal::from2dTo3d(unsigned int i, unsigned int j) const
{
}

CoordF LayoutPyramidal::fromSphereTo2d(double theta, double phi) const
{
   auto f = Face::Base;
   double minRho = std::numeric_limits<double>::max();



   for (auto testF: get_range<LayoutPyramidal::Face>())
   {
      try {
      } catch ( std::logic_error& le )
      { //no intersection with this face
         continue;
      }
   }
}

//LayoutPyramidal::Face LayoutPyramidal::AngleToFaceCart(const Coord3dCart& p) const
//{
//   //Rotate to canonical form
//   auto canonicP = CartToSherical(Rotation(p, -m_yaw, -m_pitch, -m_roll));
//
//   //Check where we are
//   auto theta = canonicP.y;
//   auto phi = canonicP.z;
//
//   double baseDemiAngle = std::atan2(m_baseEdge/2.0,1.0);
//
//   if (inInterval(theta, -baseDemiAngle, baseDemiAngle) && inInterval(phi, baseDemiAngle, PI()-baseDemiAngle))
//   {
//      return Face::Base;
//   }
//   //Else we test directly the intersection of each face:
//}

