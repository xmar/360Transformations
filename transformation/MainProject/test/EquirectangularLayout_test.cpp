#include <limits.h>
#include "gtest/gtest.h"
#include "LayoutEquirectangular.hpp"
#include "Common.hpp"
#include <memory>

using namespace IMT;

class LayoutEquirectangularTest: public ::testing::Test
{
protected:
  virtual void SetUp()
  {}

  virtual void TearDown()
  {}
};

static bool
    AlmostEqual(const CoordF& x, const CoordF& y, int ulp = 5)
{
    // the machine epsilon has to be scaled to the magnitude of the values used
    // and multiplied by the desired precision in ULPs (units in the last place)
    return (std::abs(x.x-y.x) < std::numeric_limits<float>::epsilon() * std::abs(std::min(x.x,y.x)) * ulp
    // unless the result is subnormal
           || std::abs(x.x-x.y) < std::numeric_limits<float>::min()) &&
           (std::abs(x.y-y.y) < std::numeric_limits<float>::epsilon() * std::abs(std::min(x.y,y.y)) * ulp
           || std::abs(x.x-x.y) < std::numeric_limits<float>::min());
}

TEST_F(LayoutEquirectangularTest, withoutOffset)
{
  std::shared_ptr<Layout> equirect = std::make_shared<LayoutEquirectangular>(200, 100, Quaternion(1), 0);

  auto centerPixel = equirect->FromSphereTo2d(VectorSpherical(1, 0, PI()/2));
  EXPECT_TRUE(AlmostEqual(centerPixel, CoordF(100, 50))) << centerPixel << " != " << CoordF(100, 50);

  auto centerCoordinate = equirect->From2dTo3d(CoordF(100, 50));
  EXPECT_EQ(VectorSpherical(1, 0, PI()/2), centerCoordinate) << VectorSpherical(1, 0, PI()/2) << " != " << centerCoordinate;

  auto pixel1 = equirect->FromSphereTo2d(equirect->From2dTo3d(CoordF(10, 75)));
  EXPECT_TRUE(AlmostEqual(pixel1, CoordF(10, 75))) << CoordF(std::round(pixel1.x), std::round(pixel1.y)) << " != " << CoordF(10, 75);

  auto vector1 = equirect->From2dTo3d(equirect->FromSphereTo2d(VectorSpherical(1, PI()/2, PI()/4)));
  EXPECT_EQ(vector1, VectorSpherical(1, PI()/2, PI()/4)) << VectorSpherical(vector1) << " != " << VectorSpherical(1, PI()/2, PI()/4);
}

TEST_F(LayoutEquirectangularTest, withOffset)
{
  double vectRatio = 0.75;
  unsigned int w = 200;
  unsigned int h = 100;
  std::shared_ptr<Layout> equirect = std::make_shared<LayoutEquirectangular>(w, h, Quaternion(1), vectRatio);

  auto centerPixel = equirect->FromSphereTo2d(VectorSpherical(1, 0, PI()/2));
  EXPECT_TRUE(AlmostEqual(centerPixel, CoordF(w/2, h/2))) << centerPixel << " != " << CoordF(w/2, h/2);

  auto centerCoordinate = equirect->From2dTo3d(CoordF(w/2, h/2));
  centerCoordinate = centerCoordinate / centerCoordinate.Norm();
  EXPECT_EQ(VectorSpherical(1, 0, PI()/2), centerCoordinate) << VectorSpherical(1, 0, PI()/2) << " != " << VectorSpherical(centerCoordinate);

  VectorSpherical vectorOrigine(1, 3*PI()/4, PI()/4);
  auto vector0 = vectorOrigine - (vectRatio * VectorCartesian(1, 0, 0));
  vector0 = vector0 / vector0.Norm();
  auto pixel0 = equirect->FromSphereTo2d(vectorOrigine);
  auto vector0_fromPixel = VectorSpherical(1, 2*PI()*(double(pixel0.x)/w-0.5), PI()*double(pixel0.y)/h);
  EXPECT_EQ(vector0, vector0_fromPixel) << VectorSpherical(vector0) << " != " << vector0_fromPixel;

  auto vectorOrigine_bis = equirect->From2dTo3d(pixel0);
  vectorOrigine_bis = vectorOrigine_bis / vectorOrigine_bis.Norm();
  EXPECT_EQ(vectorOrigine, vectorOrigine_bis) << vectorOrigine << " != " << VectorSpherical(vectorOrigine_bis);

  auto pixel1 = equirect->FromSphereTo2d(equirect->From2dTo3d(CoordF(10, 75)));
  EXPECT_TRUE(AlmostEqual(pixel1, CoordF(10, 75))) << CoordF(std::round(pixel1.x), std::round(pixel1.y)) << " != " << CoordF(10, 75);

  auto vector1 = equirect->From2dTo3d(equirect->FromSphereTo2d(VectorSpherical(1, PI()/2, PI()/4)));
  vector1 = vector1 / vector1.Norm();
  EXPECT_EQ(vector1, VectorSpherical(1, PI()/2, PI()/4)) << VectorSpherical(vector1) << " != " << VectorSpherical(1, PI()/2, PI()/4);
}
