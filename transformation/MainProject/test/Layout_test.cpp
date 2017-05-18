#include <limits.h>
#include "gtest/gtest.h"
#include "LayoutEquirectangular.hpp"
#include "LayoutCubeMap2.hpp"
#include "LayoutCubeMap.hpp"
#include "Common.hpp"
#include <memory>
#include <tuple>
#include <functional>

using namespace IMT;

template<class T> std::tuple<std::shared_ptr<Layout>,std::shared_ptr<Layout>> GetLayout(unsigned int w, unsigned int h, double vectRatio);

static unsigned int w = 0;
static unsigned int h = 0;
static double vectRatio = 0;
static std::shared_ptr<Layout> equirect = nullptr;
static std::shared_ptr<Layout> equirectOffset = nullptr;

template <class T>
class LayoutTest: public ::testing::Test
{
protected:
  virtual void SetUp()
  {
    w = 200;
    h = 100;
    vectRatio = 0.75;
    std::tie(equirect, equirectOffset) = GetLayout<T>(w, h, vectRatio);
  }

  virtual void TearDown()
  {}
};

template<>
std::tuple<std::shared_ptr<Layout>,std::shared_ptr<Layout>> GetLayout<LayoutEquirectangular>(unsigned int w, unsigned int h, double vectRatio)
{
  std::shared_ptr<Layout> equirect = std::make_shared<LayoutEquirectangular>(w, h, Quaternion(1), 0);
  std::shared_ptr<Layout> equirectOffet = std::make_shared<LayoutEquirectangular>(w, h, Quaternion(1), vectRatio);
  return std::make_tuple(equirect, equirectOffet);
}

template<>
std::tuple<std::shared_ptr<Layout>,std::shared_ptr<Layout>> GetLayout<LayoutCubeMap2>(unsigned int w, unsigned int h, double vectRatio)
{
  std::shared_ptr<Layout> equirect = LayoutCubeMap2::GenerateLayout(Quaternion(1), false, 0, {{unsigned(w/4), unsigned(w/4), unsigned(w/4), unsigned(w/4), unsigned(w/4), unsigned(w/4)}});
  std::shared_ptr<Layout> equirectOffet = LayoutCubeMap2::GenerateLayout(Quaternion(1), false, vectRatio, {{unsigned(w/4), unsigned(w/4), unsigned(w/4), unsigned(w/4), unsigned(w/4), unsigned(w/4)}});
  return std::make_tuple(equirect, equirectOffet);
}

template<>
std::tuple<std::shared_ptr<Layout>,std::shared_ptr<Layout>> GetLayout<LayoutCubeMap>(unsigned int w, unsigned int h, double vectRatio)
{
  std::shared_ptr<Layout> equirect = LayoutCubeMap::GenerateLayout(Quaternion(1), false, 0, {{unsigned(w/4), unsigned(w/4), unsigned(w/4), unsigned(w/4), unsigned(w/4), unsigned(w/4)}});
  std::shared_ptr<Layout> equirectOffet = LayoutCubeMap::GenerateLayout(Quaternion(1), false, vectRatio, {{unsigned(w/4), unsigned(w/4), unsigned(w/4), unsigned(w/4), unsigned(w/4), unsigned(w/4)}});
  return std::make_tuple(equirect, equirectOffet);
}

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

typedef ::testing::Types<LayoutEquirectangular, LayoutCubeMap2, LayoutCubeMap> MyTypes;
TYPED_TEST_CASE(LayoutTest, MyTypes);

TYPED_TEST(LayoutTest, withoutOffset)
{
  // auto centerPixel = equirect->FromSphereTo2d(VectorSpherical(1, 0, PI()/2));
  // EXPECT_TRUE(AlmostEqual(centerPixel, CoordF(w/2, h/2))) << centerPixel << " != " << CoordF(w/2, h/2);
  //
  // auto centerCoordinate = equirect->From2dTo3d(CoordF(w/2, h/2));
  // EXPECT_EQ(VectorSpherical(1, 0, PI()/2), centerCoordinate) << VectorSpherical(1, 0, PI()/2) << " != " << centerCoordinate;

  auto pixel1 = equirect->FromSphereTo2d(equirect->From2dTo3d(CoordF(10, 75)));
  EXPECT_TRUE(AlmostEqual(pixel1, CoordF(10, 75))) << CoordF(std::round(pixel1.x), std::round(pixel1.y)) << " != " << CoordF(10, 75);

  auto vector1 = equirect->From2dTo3d(equirect->FromSphereTo2d(VectorSpherical(1, PI()/2, PI()/4)));
  EXPECT_EQ(vector1, VectorSpherical(1, PI()/2, PI()/4)) << VectorSpherical(vector1) << " != " << VectorSpherical(1, PI()/2, PI()/4);
}

TYPED_TEST(LayoutTest, withOffset)
{
  // auto centerPixel = equirectOffset->FromSphereTo2d(VectorSpherical(1, 0, PI()/2));
  // EXPECT_TRUE(AlmostEqual(centerPixel, CoordF(w/2, h/2))) << centerPixel << " != " << CoordF(w/2, h/2);
  //
  // auto centerCoordinate = equirectOffset->From2dTo3d(CoordF(w/2, h/2));
  // centerCoordinate = centerCoordinate / centerCoordinate.Norm();
  // EXPECT_EQ(VectorSpherical(1, 0, PI()/2), centerCoordinate) << VectorSpherical(1, 0, PI()/2) << " != " << VectorSpherical(centerCoordinate);

  VectorSpherical vectorOrigine(1, 3*PI()/4, PI()/4);
  auto vector0 = vectorOrigine - (vectRatio * VectorCartesian(1, 0, 0));
  vector0 = vector0 / vector0.Norm();
  auto pixel0 = equirectOffset->FromSphereTo2d(vectorOrigine);
  auto vector0_fromPixel = VectorSpherical(1, 2*PI()*(double(pixel0.x)/w-0.5), PI()*double(pixel0.y)/h);
  EXPECT_EQ(vector0, vector0_fromPixel) << VectorSpherical(vector0) << " != " << vector0_fromPixel;

  auto vectorOrigine_bis = equirectOffset->From2dTo3d(pixel0);
  vectorOrigine_bis = vectorOrigine_bis / vectorOrigine_bis.Norm();
  EXPECT_LT( (vectorOrigine - vectorOrigine_bis ).Norm(), 0.05) << vectorOrigine << " != " << VectorSpherical(vectorOrigine_bis);

  auto pixel1 = equirectOffset->FromSphereTo2d(equirectOffset->From2dTo3d(CoordF(10, 75)));
  EXPECT_TRUE(AlmostEqual(pixel1, CoordF(10, 75))) << CoordF(std::round(pixel1.x), std::round(pixel1.y)) << " != " << CoordF(10, 75);

  auto vector1 = equirectOffset->From2dTo3d(equirectOffset->FromSphereTo2d(VectorSpherical(1, PI()/2, PI()/4)));
  vector1 = vector1 / vector1.Norm();
  EXPECT_LT( (vector1 - VectorSpherical(1, PI()/2, PI()/4)).Norm(), 0.05) << VectorSpherical(vector1) << " != " << VectorSpherical(1, PI()/2, PI()/4);
}
