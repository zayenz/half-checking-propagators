#include "utilities/tsp.h"
#include "utilities/spatial_index.h"

#include <chrono>
#include <set>
#include <fstream>

using namespace std;

namespace {
    void read_label_colon(istream &in, string &label, string &colon) {
        in >> label;
        if (label == "NODE_COORD_SECTION") {
            colon = ":";
        } else if (*label.rbegin() == ':') {
            label.erase(label.size() - 1);
            colon = ':';
        } else {
            in >> colon;
        }
    }
}

namespace hc {
#define TSP_CHECK(str, expect) do { \
    if (str != expect) {\
        return Err(TSPReadError(\
            TSPReadError::Kind::WrongFormat,\
            "Expected to read \"" + std::string(expect) + "\", got \"" + str + "\" instead."\
        ));\
    }\
} while(false)

    Result<TSPInstance, TSPReadError> hc::TSPInstance::read_instance(const std::string &file_name) {
        ifstream in(file_name);

        if (!in.is_open()) {
            return Err(TSPReadError(
                    TSPReadError::Kind::NoFile,
                    "Could not open file \"" + file_name + "\"."
            ));
        }
        const auto &result = read_instance(in);

        return result;
    }

    Result<TSPInstance, TSPReadError> TSPInstance::read_instance(istream &in) {
        // The following code parses TSPLib files, mostly assuming that they are well-formed
        string label, colon;

        string name;
        string comment;
        string type;
        string edge_weight_type;
        int dimension = -1;

        // Read all the front-matter
        while (true) {
            read_label_colon(in, label, colon);
            TSP_CHECK(colon, ":");
            if (label == "NODE_COORD_SECTION") {
                // Finish reading initial values, start reading the actual coordinates
                break;
            }

            if (label == "NAME") {
                // NAME : a280
                in >> name;
            } else if (label == "COMMENT") {
                // COMMENT : drilling problem (Ludwig)
                string comment_line;
                getline(in, comment_line);
                if (comment.empty()) {
                    comment = comment_line;
                } else {
                    comment += "\n";
                    comment += comment_line;
                }
            } else if (label == "TYPE") {
                // TYPE : TSP
                in >> type;
                if (type != "TSP") {
                    return Err(TSPReadError(
                            TSPReadError::Kind::WrongType,
                            "Expected TSP type, instead got \"" + type + "\"."
                    ));
                }
            } else if (label == "DIMENSION") {
                // DIMENSION: 280
                in >> dimension;
            } else if (label == "EDGE_WEIGHT_TYPE") {
                // EDGE_WEIGHT_TYPE : EUC_2D
                in >> edge_weight_type;
                if (edge_weight_type != "EUC_2D") {
                    return Err(TSPReadError(
                            TSPReadError::Kind::WrongDistanceMeasure,
                            "Expected edge weight type EUC_2D type, instead got \"" + edge_weight_type + "\"."
                    ));
                }
            } else {
                return Err(TSPReadError(
                        TSPReadError::Kind::WrongFormat,
                        "unexpected label read: \"" + label + "\""
                ));
            }
        }

        // Read all the coordinates.
        //   1 288 149
        vector<Point> points;
        points.reserve(dimension);
        bool no_scaling_needed = true;
        vector<int> ids;
        vector<double> xs;
        vector<double> ys;
        ids.reserve(dimension);
        xs.reserve(dimension);
        ys.reserve(dimension);
        for (int i = 0; i < dimension; ++i) {
            int id;
            double x, y;
            in >> id >> x >> y;
            if (floor(x) != ceil(x) || floor(y) != ceil(y)) {
                no_scaling_needed = false;
            }
            ids.emplace_back(id);
            xs.emplace_back(x);
            ys.emplace_back(y);
        }
        for (int i = 0; i < dimension; ++i) {
            if (no_scaling_needed) {
                points.emplace_back(Point(
                        ids[i],
                        xs[i],
                        ys[i]
                ));

            } else {
                points.emplace_back(Point(
                        ids[i],
                        approximate_as_int(xs[i], 1),
                        approximate_as_int(ys[i], 1)
                ));
            }

        }

        // EOF
        in >> label;
        TSP_CHECK(label, "EOF");

        const auto &result = Ok(TSPInstance(name, points));
        return result;
    }

    const BoundingBox &TSPInstance::bounds() const {
        return bounds_;
    }

    const vector<std::vector<LineSegment>> &TSPInstance::lines() const {
        return lines_;
    }

#undef TSP_CHECK

    DominatedEdges DominatedEdges::make_for_instance_spatial_index(const TSPInstance &instance) {
        // Clock function used.
        auto now = [] { return std::chrono::steady_clock::now(); };
        const auto de_start = now();

        const auto si_start = now();
        vector<pair<int, int>> all_pairs;
        all_pairs.reserve(instance.locations() * instance.locations());
        for (int start = 0; start < instance.locations(); ++start) {
            for (int end = start+1; end < instance.locations(); ++end) {
                all_pairs.emplace_back(make_pair(start, end));
            }
        }

        const SpatialIndex<pair<int, int>> &index = SpatialIndex<pair<int, int>>::create(
                all_pairs,
                [&](const pair<int, int>& ls) {
                    return instance.line(ls.first, ls.second).bounding_box();
                });

        const auto si_end = now();
        const std::chrono::duration<double, std::milli> si_duration =
                si_end - si_start;
//        std::cout << "Ran spatial index construction in " << si_duration.count()
//                  << std::endl;

        vector<vector<vector<Edge>>> result(instance.locations(), vector<vector<Edge>>(instance.locations(), vector<Edge>()));

        for (int start1 = 0; start1 < instance.locations(); ++start1) {
            for (int end1 = start1+1; end1 < instance.locations(); ++end1) {
                const LineSegment &s1e1 = instance.line(start1, end1);
                index.visit(
                        s1e1.bounding_box(),
                        [&](const pair<int, int>& other) {
                            int start2 = other.first;
                            int end2 = other.second;
                            if (start2 <= start1 || start2 == end1 || end2 == start1 || end2 == end1) {
                                // Only non-symmetric pairs with 4 different points should be checked
                                return;
                            }
                            const LineSegment &s2e2 = instance.line(start2, end2);
                            if (intersects(s1e1, s2e2)) {
                                const LineSegment &s2e1 = instance.line(start2, end1);
                                const LineSegment &s1e2 = instance.line(start1, end2);
                                if (dominating_in_euclidean_tsp(s1e2, s2e1, s1e1, s2e2)) {
                                    // Since s1e2 combined with s2e1 dominates s1e1 and s2e2, all combinations
                                    // of edges in the latter two are incompatible.
                                    result[start1][end1].emplace_back(Edge(start2, end2));
                                    result[start1][end1].emplace_back(Edge(end2, start2));
                                    result[end1][start1].emplace_back(Edge(start2, end2));
                                    result[end1][start1].emplace_back(Edge(end2, start2));

                                    result[start2][end2].emplace_back(Edge(start1, end1));
                                    result[start2][end2].emplace_back(Edge(end1, start1));
                                    result[end2][start2].emplace_back(Edge(start1, end1));
                                    result[end2][start2].emplace_back(Edge(end1, start1));
                                }
                            }
                        }
                );
            }
        }

        const auto de_end = now();
        const std::chrono::duration<double, std::milli> de_duration =
                de_end - de_start;
//        std::cout << "Ran dominated edges construction in " << de_duration.count()
//                  << std::endl;

        return DominatedEdges(std::move(result));
    }

    DominatedEdges DominatedEdges::make_for_instance_all_vs_all(const TSPInstance &instance) {
        // Clock function used.
        auto now = [] { return std::chrono::steady_clock::now(); };
        const auto de_start = now();

        // TODO: The following code is wasteful, since it will re-generate lots and lots of edges.
        // TODO: This should be fixed. For example just generating all pairs of non-symemtric crossing
        // TODO: edges and checking if their corresponding non-crossing pair dominates them.
        // Fins for some initial testing though.

        vector<vector<vector<Edge>>> result(instance.locations(), vector<vector<Edge>>(instance.locations(), vector<Edge>()));

        for (int start1 = 0; start1 < instance.locations(); ++start1) {
            for (int end1 = start1+1; end1 < instance.locations(); ++end1) {
                const LineSegment& s1e1 = instance.line(start1, end1);
                for (int start2 = start1+1; start2 < instance.locations(); ++start2) {
                    if (start2 != end1) { // Note: start2 != start1 by construction
                        for (int end2 = start2+1; end2 < instance.locations(); ++end2) {
                            if (end2 != end1) {// Note: end2 != start1 by construction
                                const LineSegment& s2e2 = instance.line(start2, end2);

                                const LineSegment& s1e2 = instance.line(start1, end2);
                                const LineSegment& s2e1 = instance.line(start2, end1);

                                if (dominating_in_euclidean_tsp(s1e1, s2e2, s1e2, s2e1)) {
                                    // Since s1e1 combined with s2e2 dominates s1e2 and s2e1, all combinations
                                    // of edges in the latter two are incompatible
                                    result[start1][end2].emplace_back(Edge(start2, end1));
                                    result[start1][end2].emplace_back(Edge(end1, start2));
                                    result[end2][start1].emplace_back(Edge(start2, end1));
                                    result[end2][start1].emplace_back(Edge(end1, start2));

                                    result[start2][end1].emplace_back(Edge(start1, end2));
                                    result[start2][end1].emplace_back(Edge(end2, start1));
                                    result[end1][start2].emplace_back(Edge(start1, end2));
                                    result[end1][start2].emplace_back(Edge(end2, start1));
                                } else if (dominating_in_euclidean_tsp(s1e2, s2e1, s1e1, s2e2)) {
                                    // Since s1e2 combined with s2e1 dominates s1e1 and s2e2, all combinations
                                    // of edges in the latter two are incompatible.
                                    result[start1][end1].emplace_back(Edge(start2, end2));
                                    result[start1][end1].emplace_back(Edge(end2, start2));
                                    result[end1][start1].emplace_back(Edge(start2, end2));
                                    result[end1][start1].emplace_back(Edge(end2, start2));

                                    result[start2][end2].emplace_back(Edge(start1, end1));
                                    result[start2][end2].emplace_back(Edge(end1, start1));
                                    result[end2][start2].emplace_back(Edge(start1, end1));
                                    result[end2][start2].emplace_back(Edge(end1, start1));
                                }
                            }
                        }
                    }
                }
            }
        }

        for (int i = 0; i < instance.locations(); ++i) {
            for (int j = 0; j < instance.locations(); ++j) {
                sort(result[i][j].begin(), result[i][j].end());
                result[i][j].erase(unique(result[i][j].begin(), result[i][j].end()), result[i][j].end());
            }
        }

        const auto de_end = now();
        const std::chrono::duration<double, std::milli> de_duration =
                de_end - de_start;

        std::cout << "Ran dominated edges construction in " << de_duration.count()
                  << std::endl;


        return DominatedEdges(std::move(result));
    }
}
