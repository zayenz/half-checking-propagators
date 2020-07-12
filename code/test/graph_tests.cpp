#include "extern/catch2.h"

#include <vector>
#include <iostream>

#include "utilities/tsp.h"
#include "utilities/geometry.h"
#include "utilities/graph.h"
#include "utilities/spatial_index.h"

#include "test_util.h"

using namespace hc;
using namespace std;


TEST_CASE("Base MST OneTree and Hierholzer cases", "[Graph]") {
    int size = 2;
    int id = 1;
    vector<Point> grid_points;
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            grid_points.emplace_back(Point(id++, i, j));
        }
    }
    TSPInstance grid("Grid", grid_points);

//    SECTION("Kruskal") {
//        MST mst = hc::kruskal(
//                grid.locations(),
//                vector<LineSegment>(),
//                grid.lines_length_ordered(),
//                [](const LineSegment &) { return true; });
//
//        int expected_edge_count = (size * size) - 1;
//        REQUIRE(mst.size() == 100 * expected_edge_count);
//        REQUIRE(mst.edges().size() == expected_edge_count);
//    }
//
//    SECTION("Hierholzer") {
        vector<LineSegment> edges;
        for (const auto &edge : grid.lines_length_ordered()) {
            if (edge.start_id() < edge.end_id()) {
                edges.emplace_back(edge);
            }
        }
        const vector<int> &path = hierholzer_path(grid.locations(), edges);
        REQUIRE(path.size() == edges.size());
//    }
}