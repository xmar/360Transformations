#include <limits.h>
#include "gtest/gtest.h"
#include "LayoutEquirectangular.hpp"
#include "LayoutCubeMap2.hpp"
#include "LayoutCubeMap.hpp"
#include "LayoutPyramidal.hpp"
#include "LayoutPyramidal2.hpp"
#include "LayoutEquirectangularTiles.hpp"
#include "Common.hpp"
#include <memory>
#include <tuple>
#include <functional>

using namespace IMT;

template<class T> std::tuple<std::shared_ptr<Layout>,std::shared_ptr<Layout>, CoordF> GetLayout(unsigned int w, unsigned int h, double vectRatio);

static unsigned int w = 0;
static unsigned int h = 0;
static double vectRatio = 0;
static CoordF centerOrigine = CoordF();
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
    std::tie(equirect, equirectOffset, centerOrigine) = GetLayout<T>(w, h, vectRatio);
  }

  virtual void TearDown()
  {}
};

template<>
std::tuple<std::shared_ptr<Layout>,std::shared_ptr<Layout>, CoordF> GetLayout<LayoutEquirectangular>(unsigned int w, unsigned int h, double vectRatio)
{
  std::shared_ptr<Layout> equirect = std::make_shared<LayoutEquirectangular>(w, h, Quaternion(1), 0);
  std::shared_ptr<Layout> equirectOffet = std::make_shared<LayoutEquirectangular>(w, h, Quaternion(1), vectRatio);
  return std::make_tuple(equirect, equirectOffet, CoordF(w/2, h/2));
}

template<>
std::tuple<std::shared_ptr<Layout>,std::shared_ptr<Layout>, CoordF> GetLayout<LayoutCubeMap2>(unsigned int w, unsigned int h, double vectRatio)
{
  std::shared_ptr<Layout> equirect = LayoutCubeMap2::GenerateLayout(Quaternion(1), false, 0, {{unsigned(w/4), unsigned(w/4), unsigned(w/4), unsigned(w/4), unsigned(w/4), unsigned(w/4)}});
  std::shared_ptr<Layout> equirectOffet = LayoutCubeMap2::GenerateLayout(Quaternion(1), false, vectRatio, {{unsigned(w/4), unsigned(w/4), unsigned(w/4), unsigned(w/4), unsigned(w/4), unsigned(w/4)}});
  return std::make_tuple(equirect, equirectOffet, CoordF(3*w/8, 3*w/8));
}

template<>
std::tuple<std::shared_ptr<Layout>,std::shared_ptr<Layout>, CoordF> GetLayout<LayoutCubeMap>(unsigned int w, unsigned int h, double vectRatio)
{
  std::shared_ptr<Layout> equirect = LayoutCubeMap::GenerateLayout(Quaternion(1), false, 0, {{unsigned(w/4), unsigned(w/4), unsigned(w/4), unsigned(w/4), unsigned(w/4), unsigned(w/4)}});
  std::shared_ptr<Layout> equirectOffet = LayoutCubeMap::GenerateLayout(Quaternion(1), false, vectRatio, {{unsigned(w/4), unsigned(w/4), unsigned(w/4), unsigned(w/4), unsigned(w/4), unsigned(w/4)}});
  return std::make_tuple(equirect, equirectOffet, CoordF(3*w/8, 3*w/8));
}

template<>
std::tuple<std::shared_ptr<Layout>,std::shared_ptr<Layout>, CoordF> GetLayout<LayoutPyramidal>(unsigned int w, unsigned int h, double vectRatio)
{
  std::shared_ptr<Layout> equirect = std::make_shared<LayoutPyramidal>(2.5, Quaternion(1), false, 0, w/4);
  std::shared_ptr<Layout> equirectOffet = std::make_shared<LayoutPyramidal>(2.5, Quaternion(1), false, vectRatio, w/4);
  return std::make_tuple(equirect, equirectOffet, CoordF(3*w/8, h/2));
}

template<>
std::tuple<std::shared_ptr<Layout>,std::shared_ptr<Layout>, CoordF> GetLayout<LayoutPyramidal2>(unsigned int w, unsigned int h, double vectRatio)
{
  std::shared_ptr<Layout> equirect = LayoutPyramidal2::GenerateLayout(2.5, Quaternion(1), false, 0, {{w/4, w/4, w/4, w/4, w/4}});
  std::shared_ptr<Layout> equirectOffet = LayoutPyramidal2::GenerateLayout(2.5, Quaternion(1), false, vectRatio, {{w/4, w/4, w/4, w/4, w/4}});
  return std::make_tuple(equirect, equirectOffet, CoordF(3*w/8, 3*w/8));
}

template<>
std::tuple<std::shared_ptr<Layout>,std::shared_ptr<Layout>, CoordF> GetLayout<LayoutEquirectangularTiles<2,2>>(unsigned int w, unsigned int h, double vectRatio)
{
  std::shared_ptr<Layout> equirect = std::make_shared<LayoutEquirectangularTiles<2,2>>(LayoutEquirectangularTiles<2,2>::ScaleTilesMap({{ {{1, 1}}, {{1, 1}} }}), std::make_tuple(std::array<double, 2>({{0.5, 0.5}}), std::array<double, 2>({{0.5, 0.5}})),  Quaternion(1),  std::make_tuple(w,h), false, false, 0);
  std::shared_ptr<Layout> equirectOffet = std::make_shared<LayoutEquirectangularTiles<2,2>>(LayoutEquirectangularTiles<2,2>::ScaleTilesMap({{ {{1, 1}}, {{1, 1}}}}), std::make_tuple(std::array<double, 2>({{0.5, 0.5}}), std::array<double, 2>({{0.5, 0.5}})),  Quaternion(1),  std::make_tuple(w,h), false, false, vectRatio);
  return std::make_tuple(equirect, equirectOffet, CoordF(w/2, h/2));
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

typedef ::testing::Types<LayoutEquirectangular, LayoutCubeMap2, LayoutCubeMap, LayoutPyramidal, LayoutPyramidal2, LayoutEquirectangularTiles<2,2>> MyTypes;
TYPED_TEST_CASE(LayoutTest, MyTypes);

TYPED_TEST(LayoutTest, withoutOffset)
{
  auto centerPixel = equirect->FromSphereTo2d(VectorSpherical(1, 0, PI()/2));
  EXPECT_TRUE(AlmostEqual(centerPixel, centerOrigine)) << centerPixel << " != " << centerOrigine;

  auto centerCoordinate = equirect->From2dTo3d(centerOrigine);
  EXPECT_LT((VectorSpherical(1, 0, PI()/2) - centerCoordinate).Norm(), 0.05) << VectorSpherical(1, 0, PI()/2) << " != " << VectorSpherical(centerCoordinate);

  auto pixel1 = equirect->FromSphereTo2d(equirect->From2dTo3d(CoordF(10, 75)));
  EXPECT_TRUE(AlmostEqual(pixel1, CoordF(10, 75))) << CoordF(std::round(pixel1.x), std::round(pixel1.y)) << " != " << CoordF(10, 75);

  auto vector1 = equirect->From2dTo3d(equirect->FromSphereTo2d(VectorSpherical(1, PI()/2, PI()/4)));
  EXPECT_LT(( vector1 - VectorSpherical(1, PI()/2, PI()/4)).Norm(), 0.05) << VectorSpherical(vector1) << " != " << VectorSpherical(1, PI()/2, PI()/4);
}

TYPED_TEST(LayoutTest, withOffset)
{
  auto centerPixel = equirectOffset->FromSphereTo2d(VectorSpherical(1, 0, PI()/2));
  EXPECT_TRUE(AlmostEqual(centerPixel, centerOrigine)) << centerPixel << " != " << centerOrigine;

  auto centerCoordinate = equirectOffset->From2dTo3d(centerOrigine);
  centerCoordinate = centerCoordinate / centerCoordinate.Norm();
  EXPECT_LT((VectorSpherical(1, 0, PI()/2) - centerCoordinate).Norm(), 0.05) << VectorSpherical(1, 0, PI()/2) << " != " << VectorSpherical(centerCoordinate);

  VectorSpherical vectorOrigine(1, 3*PI()/4, PI()/4);
  auto vector0 = vectorOrigine - (vectRatio * VectorCartesian(1, 0, 0));
  vector0 = vector0 / vector0.Norm();
  auto pixel0 = equirectOffset->FromSphereTo2d(vectorOrigine);
  auto vector0_fromPixel = VectorSpherical(1, 2*PI()*(double(pixel0.x)/w-0.5), PI()*double(pixel0.y)/h);
  EXPECT_LT((vector0 - vector0_fromPixel).Norm(), 0.05) << VectorSpherical(vector0) << " != " << vector0_fromPixel;

  auto vectorOrigine_bis = equirectOffset->From2dTo3d(pixel0);
  vectorOrigine_bis = vectorOrigine_bis / vectorOrigine_bis.Norm();
  EXPECT_LT( (vectorOrigine - vectorOrigine_bis ).Norm(), 0.05) << vectorOrigine << " != " << VectorSpherical(vectorOrigine_bis);

  auto pixel1 = equirectOffset->FromSphereTo2d(equirectOffset->From2dTo3d(CoordF(10, 75)));
  EXPECT_TRUE(AlmostEqual(pixel1, CoordF(10, 75))) << CoordF(std::round(pixel1.x), std::round(pixel1.y)) << " != " << CoordF(10, 75);

  auto vector1 = equirectOffset->From2dTo3d(equirectOffset->FromSphereTo2d(VectorSpherical(1, PI()/2, PI()/4)));
  vector1 = vector1 / vector1.Norm();
  EXPECT_LT( (vector1 - VectorSpherical(1, PI()/2, PI()/4)).Norm(), 0.05) << VectorSpherical(vector1) << " != " << VectorSpherical(1, PI()/2, PI()/4);
}
