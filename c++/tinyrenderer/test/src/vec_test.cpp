#include <gtest/gtest.h>
#include "geometry.h"


TEST(Vec2, dot){
    Vec2i a(20, 34);
    Vec2i b(1, -4);

    Vec2i ans(21,30);

    bool res= ( (a+b) == ans);
    ASSERT_EQ(true, res);
}



TEST(Vec3, cross_1){
    Vec3i a(1, 0,0);
    Vec3i b(0, 1,0);

    Vec3i ans(0,0,1);

    bool res= ( a.cross(b) == ans);
    ASSERT_EQ(true, res);
}

TEST(Vec3, cross_2){
    Vec3i a(1, 2,3);
    Vec3i b(4, 5,6);

    Vec3i ans(-3,6,-3);

    bool res= ( a.cross(b) == ans);
    ASSERT_EQ(true, res);
}

TEST(Vec3, cross_3){
    Vec3i a(1, 2,3);
    Vec3i b(4, 5,6);

    Vec3i ans(3,-6,3);

    bool res= ( b.cross(a) == ans);
    ASSERT_EQ(true, res);
}