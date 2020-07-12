#ifndef HC_GRAPH_H
#define HC_GRAPH_H

#include "geometry.h"
#include "disjoint-set.h"
#include "tsp.h"

#include <utility>
#include <vector>


namespace hc {
    class MST {
        std::vector<LineSegment> edges_;
        std::vector<std::vector<LineSegment>> edges_by_node_;
        int size_;

        static std::vector<std::vector<LineSegment>> split_by_node(const std::vector<LineSegment> &edges) {
            int max_node = 0;
            for (const auto &edge : edges) {
                max_node = std::max(std::max(edge.start_id(), edge.end_id()), max_node);
            }
            std::vector<std::vector<LineSegment>> result(max_node + 1, std::vector<LineSegment>());

            for (const auto &edge : edges) {
                result[edge.start_id()].emplace_back(edge);
                result[edge.end_id()].emplace_back(edge);
            }

            return result;
        }

    public:

        explicit MST(std::vector<LineSegment> edges)
                : edges_(std::move(edges)),
                  edges_by_node_(split_by_node(edges_)),
                  size_(sum_line_lengths(edges_)) {}

        [[nodiscard]] const std::vector<LineSegment> &edges() const {
            return edges_;
        }

        [[nodiscard]] const std::vector<LineSegment> &edges_at(int node) const {
            return edges_by_node_.at(node);
        }

        [[nodiscard]] int size() const {
            return size_;
        }
    };

    class OneTree {
        int nodes_;
        int extra_node_;
        std::pair<LineSegment, LineSegment> extra_edges_;
        MST mst_;
        std::vector<std::vector<LineSegment>> edges_by_node_;
        int size_;
    public:
        OneTree(int nodes, int extra_node, std::pair<LineSegment, LineSegment> extra_edges,
                std::vector<LineSegment> mst_edges)
                : nodes_(nodes),
                  extra_node_(extra_node),
                  extra_edges_(std::move(extra_edges)),
                  mst_(std::move(mst_edges)),
                  edges_by_node_(),
                  size_(mst_.size() + extra_edges_.first.length() + extra_edges_.second.length()) {
            for (int i = 0; i < nodes_; ++i) {
                edges_by_node_.emplace_back(mst_.edges_at(i));
            }
            edges_by_node_[extra_edges_.first.start_id()].emplace_back(extra_edges_.first);
            edges_by_node_[extra_edges_.first.end_id()].emplace_back(extra_edges_.first);
            edges_by_node_[extra_edges_.second.start_id()].emplace_back(extra_edges_.second);
            edges_by_node_[extra_edges_.second.end_id()].emplace_back(extra_edges_.second);
        }

        [[nodiscard]] int extra_node() const {
            return extra_node_;
        }

        [[nodiscard]] const std::pair<LineSegment, LineSegment> &extra_edges() const {
            return extra_edges_;
        }

        [[nodiscard]] const MST &mst() const {
            return mst_;
        }

        [[nodiscard]] const std::vector<LineSegment> &edges_at(int node) const {
            return edges_by_node_.at(node);
        }

        [[nodiscard]] int size() const {
            return size_;
        }
    };


    MST kruskal(int nodes,
                const std::vector<LineSegment> &mandatory_edges,
                const std::vector<LineSegment> &edges,
                const std::function<bool(const LineSegment &)> &filter);


    OneTree kruskal_1_tree(int nodes,
                           int excluded_node,
                           const std::vector<LineSegment> &mandatory_edges,
                           const std::vector<LineSegment> &edges,
                           const std::function<bool(const LineSegment &)> &filter);


    std::optional<std::vector<LineSegment>> christofides(std::shared_ptr<const TSPInstance> instance,
                                          int nodes,
                                          const std::vector<LineSegment> &mandatory_edges,
                                          const std::vector<LineSegment> &edges,
                                          const std::function<bool(const LineSegment &)> &filter);

    std::vector<int> hierholzer_path(int nodes, const std::vector<LineSegment>& vector);


}

#endif //HC_GRAPH_H
