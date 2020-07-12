#include "extern/catch2.h"

#include <vector>
#include <iostream>

#include "utilities/geometry.h"
#include "utilities/spatial_index.h"

#include "test_util.h"

using namespace hc;
using namespace std;


TEST_CASE("Simple test cases", "[SpatialIndex]") {
    const int box_count = 100;
    const int box_size = 5;

    vector<BoundingBox> boxes;
    boxes.reserve(box_count);
    for (int i = 0; i < box_count; ++i) {
        boxes.emplace_back(BoundingBox(i, i, i + box_size, i + box_size));
    }

    const SpatialIndex<BoundingBox> &index = SpatialIndex<BoundingBox>::create(boxes);
    if (debug) index.print(std::cerr);

    const vector<reference_wrapper<const BoundingBox>> &hits =
            index.collect(BoundingBox(10, 10, 20, 20));

    for (const auto &hit : hits) {
        derr << "Hit: " << hit << endl;
    }

    REQUIRE(hits.size() == 16);
}