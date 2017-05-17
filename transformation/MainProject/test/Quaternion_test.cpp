#include <limits.h>
#include "gtest/gtest.h"
#include "Quaternion.hpp"
#include "Common.hpp"

using namespace IMT;

class QuaternionTest: public ::testing::Test
{
protected:
  virtual void SetUp()
  {}

  virtual void TearDown()
  {}
};


TEST_F(QuaternionTest, addition)
{
  ASSERT_EQ(Quaternion(2, VectorCartesian(4, 6, 8)), Quaternion(1, VectorCartesian(-2, 6, 7)) + Quaternion(1, VectorCartesian(6, 0, 1)));

  ASSERT_EQ(Quaternion(1, VectorCartesian(4, 6, 8)), 1 + Quaternion(0, VectorCartesian(4, 6, 8)));
}

TEST_F(QuaternionTest, rotation)
{
  ASSERT_EQ(VectorCartesian(-2, 1, 3), Quaternion(std::cos(PI()/4), std::sin(PI()/4)*VectorCartesian(0, 0, 1)).Rotation(VectorCartesian(1, 2, 3)));
  ASSERT_EQ(VectorCartesian(1, -2, -3), Quaternion::QuaternionFromAngleAxis(PI(), VectorCartesian(1, 0, 0)).Rotation(VectorCartesian(1, 2, 3)));
}

TEST_F(QuaternionTest, rotationComposition)
{
    auto globalRot = Quaternion::QuaternionFromAngleAxis(PI()/4, VectorCartesian(1, 1, 1));
    auto faceRot = Quaternion::QuaternionFromAngleAxis(PI(), VectorCartesian(0, 0, 1));

    VectorCartesian v(1, 0.5, 0.5);

    VectorCartesian v1 = faceRot.Rotation(v);
    VectorCartesian v2 = v1 + 0.75 * VectorCartesian(1, 0, 0);
    VectorCartesian v3 = globalRot.Rotation(v2);

    VectorCartesian v2_bis = globalRot.Inv().Rotation(v3);
    ASSERT_EQ(v2, v2_bis);
    VectorCartesian v1_bis = v2_bis - 0.75 * VectorCartesian(1, 0, 0);
    ASSERT_EQ(v1, v1_bis);
    VectorCartesian v_bis = faceRot.Inv().Rotation(v1_bis);
    ASSERT_EQ(v, v_bis);
}

TEST_F(QuaternionTest, DISABLED_rotationCompositionSpherical)
{
    auto globalRot = Quaternion::QuaternionFromAngleAxis(PI()/4, VectorCartesian(1, 1, 1));
    auto faceRot = Quaternion::QuaternionFromAngleAxis(PI(), VectorCartesian(0, 0, 1));

    VectorSpherical v(1, 0.005, PI()/2);

    VectorCartesian v1 = faceRot.Rotation(v);
    VectorCartesian v2 = v1 + 0.75 * VectorCartesian(1, 0, 0);
    VectorCartesian v3 = globalRot.Rotation(v2);

    VectorCartesian v2_bis = globalRot.Inv().Rotation(v3);
    EXPECT_EQ(v2, v2_bis) << v2 << " != " << v2_bis;
    VectorCartesian v1_bis = v2_bis - 0.75 * VectorCartesian(1, 0, 0);
    EXPECT_EQ(v1, v1_bis) << v1 << " != " << v1_bis;;
    VectorCartesian v_bis = faceRot.Inv().Rotation(v1_bis);
    ASSERT_EQ(v, v_bis) << v << " != " << VectorSpherical(v_bis);
}
