#include <limits.h>
#include "gtest/gtest.h"
#include "Vector.hpp"
#include "Common.hpp"

using namespace IMT;

class VectorTest: public ::testing::Test
{
protected:
  virtual void SetUp()
  {}

  virtual void TearDown()
  {}
};

TEST_F(VectorTest, additionCartesian)
{
  ASSERT_EQ(VectorCartesian(0,0,0), VectorCartesian(1,-2,3) + VectorCartesian(-1,2,-3));
  ASSERT_EQ(VectorCartesian(1,0,1), VectorCartesian(1,0,0) + VectorCartesian(0,0,1));
}

TEST_F(VectorTest, SphericalToCartesian)
{
  ASSERT_EQ(VectorCartesian(1,0,1), VectorCartesian(VectorSpherical(std::sqrt(2), 0, PI()/4))) << VectorCartesian(1,0,1) << " is not equal to "<< VectorSpherical(std::sqrt(2), 0, PI()/4) <<  " = " <<VectorCartesian(VectorSpherical(std::sqrt(2), 0, PI()/4));
  ASSERT_EQ(VectorCartesian(42,-21,13), VectorCartesian(VectorSpherical(std::sqrt(2374), std::atan2(-21, 42), std::acos(13/std::sqrt(2374))))) << VectorCartesian(42,-21,13) << " is not equal to "<< VectorSpherical(std::sqrt(2374), -0.463647609, 1.56532031) <<  " = " <<VectorCartesian(VectorSpherical(std::sqrt(2374), -0.463647609, 1.56532031));
}

TEST_F(VectorTest, CartesianToSpherical)
{
  ASSERT_EQ(VectorSpherical(std::sqrt(2374), std::atan2(-21, 42), std::acos(13/std::sqrt(2374))), VectorSpherical(VectorCartesian(42,-21,13))) << VectorSpherical(std::sqrt(2374), std::atan2(-21, 42), std::acos(13/std::sqrt(2374))) << " is not equal to "<< VectorCartesian(42,-21,13) <<  " = " <<VectorSpherical(VectorCartesian(42,-21,13));
}

TEST_F(VectorTest, additionSpherical)
{
  ASSERT_EQ(VectorCartesian(1,0,1), VectorSpherical(1,0,PI()/2) + VectorCartesian(0,0,1));
}

TEST_F(VectorTest, dotProduct)
{
  ASSERT_EQ(2, VectorCartesian(1,0,1) * VectorCartesian(1,0,1));
  ASSERT_EQ(4, VectorSpherical(2,0,PI()/2) * VectorSpherical(2,0,PI()/2));
}

TEST_F(VectorTest, minus)
{
  ASSERT_EQ(VectorCartesian(-1,-2,-3), -VectorCartesian(1,2,3));
  // EXPECT_EQ(VectorCartesian(-4.5,-4.5,-9.0*std::sqrt(2.0)/2), -VectorSpherical(9, PI()/2, 3*PI()/4)) << VectorCartesian(-4.5,-4.5, 9.0*std::sqrt(2.0)/2) << " != " << -VectorSpherical(9, PI()/4, 3*PI()/4);
}

TEST_F(VectorTest, norm)
{
  ASSERT_EQ(std::sqrt(2), VectorCartesian(1,0,1).Norm());
  ASSERT_EQ(2, VectorSpherical(2,0,PI()/2).Norm());
}

TEST_F(VectorTest, multiplyByAScalar)
{
  ASSERT_EQ(VectorCartesian(8,-16,24), 8*VectorCartesian(1,-2,3));
  ASSERT_EQ(VectorCartesian(8,-16,24), VectorCartesian(1,-2,3)*8);
  ASSERT_EQ(VectorSpherical(16,0,PI()/2), 8*VectorSpherical(2,0,PI()/2));
  ASSERT_EQ(VectorSpherical(16,0,PI()/2), VectorSpherical(2,0,PI()/2)*8);
}

TEST_F(VectorTest, vectorialProduct)
{
  ASSERT_EQ(VectorCartesian(-13,8,-1), VectorCartesian(1,2,3) ^ VectorCartesian(3,5,1));
}
