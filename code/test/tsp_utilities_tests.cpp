#include "extern/catch2.h"

#include <vector>
#include <iostream>
#include <sstream>

#include "utilities/geometry.h"
#include "utilities/tsp.h"

#include "test_util.h"

using namespace hc;
using namespace std;


TEST_CASE("dominating_in_euclidean_tsp", "[TSP]") {
    SECTION("Simplest base case") {
        const Point p11(1, 1);
        const Point p12(1, 2);
        const Point p21(2, 1);
        const Point p22(2, 2);
        const LineSegment l11_21(p11, p21);
        const LineSegment l12_22(p12, p22);
        const LineSegment l11_22(p11, p22);
        const LineSegment l12_21(p12, p21);

        SECTION("Checking domination") {
            REQUIRE(dominating_in_euclidean_tsp(l11_21, l12_22, l11_22, l12_21));
        }

        /* Diagram over the instance, with edges, node indices, and node locations.
         *
         *         2    3
         *   (2,1) X----X (2,2)
         *         | \ /|
         *         | / \|
         *   (1,1) X----X (1,2)
         *         0    1
         */
        TSPInstance instance("2-by-2 grid", {p11, p12, p21, p22});

        SECTION("Check dominated edges") {
            DominatedEdges &dominated_edges = instance.dominated_edges();
            REQUIRE(dominated_edges.dominated(Edge(0,1)).empty());
            REQUIRE(dominated_edges.dominated(Edge(1,3)).empty());
            REQUIRE(dominated_edges.dominated(Edge(3,2)).empty());
            REQUIRE(dominated_edges.dominated(Edge(2,0)).empty());

            REQUIRE(dominated_edges.dominated(Edge(0,3)) == vector<Edge>{Edge(1,2), Edge(2,1)});
            REQUIRE(dominated_edges.dominated(Edge(1,2)) == vector<Edge>{Edge(0,3), Edge(3,0)});
        }
    }

    SECTION("Asymmetric dominance in grid") {
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
                for (int s2 = 0; s2 < m.size(); ++s2) {
                    for (int e2 = 0; e2 < m.size(); ++e2) {
                        LineSegment x1(m[s1], m[e1]);
                        LineSegment y1(m[s2], m[e2]);
                        LineSegment x2(m[s1], m[e2]);
                        LineSegment y2(m[s2], m[e1]);
                        derr << "Checking asymmetric result of dominance of ("
                             << x1 << "," << y1 << ") vs (" << x2 << "," << y2 << ")"
                             << std::endl;
                        REQUIRE(
                                IMPLIES(
                                        dominating_in_euclidean_tsp(x1, y1, x2, y2),
                                        !dominating_in_euclidean_tsp(x2, y2, x1, y1)
                                )
                        );
                    }
                }
            }
        }
    }
}

TEST_CASE("Read tsp", "[TSP]") {
    const string berlin52truncated = "NAME: berlin52truncated\n"
                                     "TYPE: TSP\n"
                                     "COMMENT: 52 locations in Berlin (Groetschel)\n"
                                     "DIMENSION: 5\n"
                                     "EDGE_WEIGHT_TYPE: EUC_2D\n"
                                     "NODE_COORD_SECTION\n"
                                     "1 565.0 575.0\n"
                                     "2 25.0 185.0\n"
                                     "3 345.0 750.0\n"
                                     "4 945.0 685.0\n"
                                     "5 845.0 655.0\n"
                                     "EOF\n";
    istringstream b52trunc_in(berlin52truncated);
    const Result<TSPInstance, TSPReadError> &result = TSPInstance::read_instance(b52trunc_in);
    if (result.isErr()) {
        derr << result.unwrapErr().text << endl;
    }
    REQUIRE(result.isOk());
    const TSPInstance &instance = result.unwrap();
    REQUIRE(instance.locations() == 5);
    for (int l = 0; l < instance.locations(); ++l) {
        REQUIRE(instance.location(l).id() == l+1);
    }
    REQUIRE(instance.name() == "berlin52truncated");
}
