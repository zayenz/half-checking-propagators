#ifndef HC_TSP_H
#define HC_TSP_H

#include <string>
#include <utility>
#include <vector>
#include <ostream>
#include <optional>

#include "extern/result.h"
#include "utilities/geometry.h"

namespace hc {
    struct TSPReadError {
        enum class Kind {
            NoFile,
            WrongDistanceMeasure,
            WrongType,
            WrongFormat,
        };

        Kind kind;
        std::string text;

        TSPReadError(Kind kind, std::string text) : kind(kind), text(std::move(text)) {}
    };

    class Edge {
        // While from_ and to_ are technically const (an Edge is an immutable POD),
        // it is unfortunately not possible to have them as const members since then
        // the type is not move-assignable, which precludes the use of sort on a collection
        // of Edge instances, since sort uses swap and swap requires move-assignable.

        int from_;
        int to_;
    public:

        Edge(const int from, const int to) : from_(from), to_(to) {}

        Edge(Edge& other) = default;
        Edge(Edge&& other) = default;
        Edge(const Edge& other) = default;
        Edge& operator=(Edge&& other) = default;


        [[nodiscard]] const int from() const {
            return from_;
        }

        [[nodiscard]] const int to() const {
            return to_;
        }

        bool operator==(const Edge &rhs) const {
            return from_ == rhs.from_ &&
                   to_ == rhs.to_;
        }

        bool operator!=(const Edge &rhs) const {
            return !(rhs == *this);
        }

        bool operator<(const Edge &rhs) const {
            if (from_ < rhs.from_)
                return true;
            if (rhs.from_ < from_)
                return false;
            return to_ < rhs.to_;
        }

        bool operator>(const Edge &rhs) const {
            return rhs < *this;
        }

        bool operator<=(const Edge &rhs) const {
            return !(rhs < *this);
        }

        bool operator>=(const Edge &rhs) const {
            return !(*this < rhs);
        }

        friend std::ostream &operator<<(std::ostream &os, const Edge &edge) {
            os << edge.from_ << "->" << edge.to_;
            return os;
        }
    };

    class TSPInstance;

    class DominatedEdges {
        /// Domainated edges indexed by dominated_[from][to]
        const std::vector<std::vector<std::vector<Edge>>> dominated_;
        
        explicit DominatedEdges(std::vector<std::vector<std::vector<Edge>>> dominated) 
                : dominated_(std::move(dominated)) {}
    public:
        static DominatedEdges make_for_instance_spatial_index(const TSPInstance& instance);
        static DominatedEdges make_for_instance_all_vs_all(const TSPInstance& instance);
        const std::vector<Edge>& dominated(Edge edge) {
            return dominated_[edge.from()][edge.to()];
        }
    };


    class TSPInstance {
        const std::string name_;
        const std::vector<Point> locations_;
        const std::vector<std::vector<LineSegment>> lines_;
        const std::vector<LineSegment> lines_length_ordered_;
        const std::vector<int> max_costs_;
        const int max_cost_;
        const BoundingBox bounds_;
        mutable std::optional<DominatedEdges> dominated_edges_;

        static std::vector<std::vector<LineSegment>> compute_lines(const std::vector<Point>& locations) {
            std::vector<std::vector<LineSegment>> result(locations.size(), std::vector<LineSegment>());

            for (std::size_t i = 0; i < locations.size(); ++i) {
                result[i].reserve(locations.size());
                for (std::size_t j = 0; j < locations.size(); ++j) {
                    result[i].emplace_back(LineSegment(locations[i], locations[j]));
                }
            }

            return result;
        }

        static std::vector<LineSegment> compute_lines_length_ordered(const std::vector<std::vector<LineSegment>>& line_segments) {
            std::vector<LineSegment> result;

            int size = 0;
            for (const auto & lines : line_segments) {
                size += lines.size();
            }
            result.reserve(size);

            for (const auto & lines : line_segments) {
                result.insert(result.end(), lines.begin(), lines.end());
            }

            std::sort(result.begin(), result.end(), [](LineSegment& a, LineSegment& b){
                return a.length() < b.length();
            });

            return result;
        }

        static std::vector<int> compute_max_costs(const std::vector<std::vector<LineSegment>>& graph) {
            std::vector<int> result;
            result.reserve(graph.size());
            for (const auto &node_lines : graph) {
                int max_cost = -1;
                for (const auto &line : node_lines) {
                    max_cost = std::max(max_cost, line.length());
                }
                result.emplace_back(max_cost);
            }

            return result;
        }
    public:
        TSPInstance(std::string name, std::vector<Point> locations)
                : name_(std::move(name)),
                  locations_(std::move(locations)),
                  lines_(compute_lines(locations_)),
                  lines_length_ordered_(compute_lines_length_ordered(lines_)),
                  max_costs_(compute_max_costs(lines_)),
                  max_cost_(*std::max_element(max_costs_.begin(), max_costs_.end())),
                  bounds_(BoundingBox::from(locations_))
        {
//            assert(locations.size() > 0);
        }

        static Result<TSPInstance, TSPReadError> read_instance(const std::string& file_name);
        static Result<TSPInstance, TSPReadError> read_instance(std::istream& file_name);

        [[nodiscard]] const std::string& name() const {
            return name_;
        }

        [[nodiscard]] const std::vector<int>& max_costs() const {
            return max_costs_;
        }

        [[nodiscard]] int max_cost() const {
            return max_cost_;
        }

        [[nodiscard]] int max_total_cost() const {
            int total_cost = 0;
            for (const auto &cost : max_costs_) {
                total_cost += cost;
            }
            return total_cost;
        }

        [[nodiscard]] int locations() const {
            return locations_.size();
        }

        [[nodiscard]] const Point& location(int i) const {
            return locations_[i];
        }

        [[nodiscard]] const LineSegment& line(int i, int j) const {
            return lines_.at(i).at(j);
        }

        const std::vector<std::vector<LineSegment>> &lines() const;

        const std::vector<LineSegment> &lines_length_ordered() const {
            return lines_length_ordered_;
        }

        void compute_dominated_edges() const {
            if (dominated_edges_.has_value()) {
                return;
            }

            const DominatedEdges &dominated_edges = DominatedEdges::make_for_instance_spatial_index(*this);
//            const DominatedEdges &dominated_edges = DominatedEdges::make_for_instance_all_vs_all(*this);
            dominated_edges_.emplace(dominated_edges);
        }

        [[nodiscard]] DominatedEdges& dominated_edges() const {
            compute_dominated_edges();

            return dominated_edges_.value();
        }

        const BoundingBox &bounds() const;
    };

    /**
     * Line segment pair x1,y1 dominates pair x2,y2 in any solution to an euclidean TSP iff the following
     * conditions hold
     *   - The starting points are the same but the ending points are switched
     *   - The distance when using x1,y1 is smaller than when using x2,,y2
     *   - The lines x1,y1 do not intersect, while the lines x2,y2 intersect
     * 
     * @param x1 A line (s1, e1)
     * @param y1 A line (s2, e2)
     * @param x2 The line (s1, e2)
     * @param y2 The line (s2, e1)
     * @return True iff the pair of line segments x1 and y1 dominates x2 and y2 
     *         in any optimal TSP solution using euclidean distances. 
     */
    inline bool dominating_in_euclidean_tsp(const LineSegment& x1, const LineSegment& y1,
                                            const LineSegment& x2, const LineSegment& y2) {
        // First, check that the line segments have the correct set up
        const bool matching_points =
                x1.start() == x2.start() && y1.start() == y2.start() && x1.end() == y2.end() && y1.end() == x2.end();
        if (!matching_points) {
            return false;
        }

        // Second, check that the distance is actually improving (or rather, not becoming worse)
        // by using (x1,y1) instead of (x2,y2).
        // In particular, this might not always be the case even though it should for some pairs due
        // to the approximation of distances used.
        const bool improving_distance = (x1.length() + y1.length()) <= (x2.length() + y2.length());
        if (!improving_distance) {
            return false;
        }

        // Finally, check that (x1,y1) does not intersect and that (x2,y2) actually does intersect.
        const bool intersecting1 = intersects(x1, y1);
        const bool intersecting2 = intersects(x2, y2);

        return !intersecting1 && intersecting2;
    }
}

#endif //HC_TSP_H
