#include "extern/catch2.h"

#include <vector>
#include <iostream>

#include "utilities/geometry.h"

#include "test_util.h"

using namespace hc;
using namespace std;

TEST_CASE("Line segment length", "[LineSegment]") {
    Point p0101(1, 1);
    Point p0102(1, 2);
    Point p0202(2, 2);
    LineSegment l0101_0102(p0101, p0102);
    LineSegment l0202_0102(p0202, p0102);
    LineSegment l0101_0202(p0101, p0202);

    REQUIRE(l0101_0102.length() == 100);
    REQUIRE(l0202_0102.length() == 100);
    REQUIRE(l0101_0202.length() == 141);
}

TEST_CASE("Line segment intersection", "[LineSegment]") {
    SECTION("Manual cases") {
        /*
         * Approximate diagram of the points with the line segements marked
             10 20     11 21
                *------*
               /\   / /
              / -\-  /
             / /  \ /
            *------*
           1 1     2 1
         */

        Point p0101(1, 1);
        Point p0201(2, 1);
        Point p1020(10, 20);
        Point p1121(11, 21);

        // Segment upwards and to the left
        LineSegment l0101_0201(p0101, p0201);
        LineSegment l0101_1020(p0101, p1020);
        LineSegment l0201_1121(p0201, p1121);
        LineSegment l0101_1121(p0101, p1121);
        LineSegment l0201_1020(p0201, p1020);
        LineSegment l1020_1121(p1020, p1121);

        // Segments downwards and to the right
        LineSegment l0201_0101(p0201, p0101);
        LineSegment l1020_0101(p1020, p0101);
        LineSegment l1121_0201(p1121, p0201);
        LineSegment l1121_0101(p1121, p0101);
        LineSegment l1020_0201(p1020, p0201);
        LineSegment l1121_1020(p1020, p1121);

        // The following macro is used with two pairs of point coordinate arguments
        // to test all directions of line segments against the expected result.
#define TEST_SEGMENTS(x1,x2,y1,y2,result) do { \
        REQUIRE(intersects( l##x1##_##x2 , l##y1##_##y2 ) == result);\
        REQUIRE(intersects( l##x2##_##x1 , l##y1##_##y2 ) == result);\
        REQUIRE(intersects( l##x1##_##x2 , l##y2##_##y1 ) == result);\
        REQUIRE(intersects( l##x2##_##x1 , l##y2##_##y1 ) == result);\
    } while (false)

        // Lines 0101<->1020 with all other lines
        TEST_SEGMENTS(0101,1020, 0101, 0201, true);
        TEST_SEGMENTS(0101,1020, 0101, 1020, true);
        TEST_SEGMENTS(0101,1020, 0101, 1121, true);
        TEST_SEGMENTS(0101,1020, 0201, 1020, true);
        TEST_SEGMENTS(0101,1020, 0201, 1121, false);
        TEST_SEGMENTS(0101,1020, 1020, 1121, true);

        // Lines 0101<->0201 with all other lines
        TEST_SEGMENTS(0101,0201, 0101, 0201, true);
        TEST_SEGMENTS(0101,0201, 0101, 1020, true);
        TEST_SEGMENTS(0101,0201, 0101, 1121, true);
        TEST_SEGMENTS(0101,0201, 0201, 1020, true);
        TEST_SEGMENTS(0101,0201, 0201, 1121, true);
        TEST_SEGMENTS(0101,0201, 1020, 1121, false);

        // Lines 0101<->1121 with all other lines
        TEST_SEGMENTS(0101,1121, 0101, 0201, true);
        TEST_SEGMENTS(0101,1121, 0101, 1020, true);
        TEST_SEGMENTS(0101,1121, 0101, 1121, true);
        TEST_SEGMENTS(0101,1121, 0201, 1020, true);
        TEST_SEGMENTS(0101,1121, 0201, 1121, true);
        TEST_SEGMENTS(0101,1121, 1020, 1121, true);

        // Lines 0201<->1020 with all other lines
        TEST_SEGMENTS(0201,1020, 0101, 0201, true);
        TEST_SEGMENTS(0201,1020, 0101, 1020, true);
        TEST_SEGMENTS(0201,1020, 0101, 1121, true);
        TEST_SEGMENTS(0201,1020, 0201, 1020, true);
        TEST_SEGMENTS(0201,1020, 0201, 1121, true);
        TEST_SEGMENTS(0201,1020, 1020, 1121, true);

        // Lines 0201<->1121 with all other lines
        TEST_SEGMENTS(0201,1121, 0101, 0201, true);
        TEST_SEGMENTS(0201,1121, 0101, 1020, false);
        TEST_SEGMENTS(0201,1121, 0101, 1121, true);
        TEST_SEGMENTS(0201,1121, 0201, 1020, true);
        TEST_SEGMENTS(0201,1121, 0201, 1121, true);
        TEST_SEGMENTS(0201,1121, 1020, 1121, true);
    }

    SECTION ("Symmetric results in grid") {
        const int grid_size = 3;
        const int scale = 2;
        vector<Point> m;
        m.reserve(grid_size * grid_size);
        for (int i = 0; i < grid_size; ++i) {
            for (int j = 0; j < grid_size; ++j) {
                m.emplace_back(Point(i * scale, j * scale));
            }
        }

        for (int s1 = 0; s1 < m.size(); ++s1) {
            for (int e1 = 0; e1 < m.size(); ++e1) {
                LineSegment l1(m[s1], m[e1]);
                for (int s2 = 0; s2 < m.size(); ++s2) {
                    for (int e2 = 0; e2 < m.size(); ++e2) {
                        LineSegment l2(m[s2], m[e2]);
                        derr << "Checking symmetric result of intersection of " << l1 << " and " << l2
                             << std::endl;
                        // TODO: write an oracle for the simple grid intersection case and test aginst that
                        REQUIRE(intersects(l1, l2) == intersects(l2, l1));
                    }
                }
            }
        }
    }
}
