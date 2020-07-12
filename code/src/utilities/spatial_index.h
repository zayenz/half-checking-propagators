#ifndef HC_SPATIAL_INDEX_H
#define HC_SPATIAL_INDEX_H

#include <algorithm>
#include <deque>
#include "utilities/geometry.h"

#include <iostream>

namespace hc {
    /**
     * Simple immutable spatial index class.
     *
     * The spatial index is based on an R-tree, building the structure using a method inspired by the STR algorithm.
     */
    template <typename E>
    class SpatialIndex {
        struct Element {
            BoundingBox box_;
            E element_;

            Element(const BoundingBox &box, const E& element) : box_(box), element_(element) {}
        };

        class Node {
            BoundingBox box_;
            int a_;
            int b_;
            Node(const BoundingBox &box, int a, int b) : box_(box), a_(a), b_(b) {}
        public:
            [[nodiscard]] static Node make_leaf(const std::vector<Element>& elements, int from, int to) {
                assert(0 <= from && from <= to && to < elements.size());
                std::vector<BoundingBox> boxes;
                boxes.reserve(to - from + 1);
                for (int i = from; i <= to; ++i) {
                    boxes.emplace_back(elements[i].box_);
                }
                const BoundingBox box = BoundingBox::from(boxes);
                return std::move(Node(box, from, to));
            }

            [[nodiscard]] static Node make_internal(const std::vector<Node>& elements, int left, int right) {
                assert(0 <= left && left < elements.size());
                assert(0 <= right && right < elements.size());
                assert(left != right);
                const BoundingBox box = BoundingBox::from({elements[left].box_, elements[right].box_});
                return std::move(Node(box, -left, -right));
            }

            [[nodiscard]] bool leaf() const {
                return b_ >= 0;
            }

            [[nodiscard]] bool internal() const {
                return b_ < 0;
            }

            [[nodiscard]] int from() const {
                assert(leaf());
                return a_;
            }

            [[nodiscard]] int to() const {
                assert(leaf());
                return b_;
            }

            [[nodiscard]] int left() const {
                assert(internal());
                return -a_;
            }

            [[nodiscard]] int right() const {
                assert(internal());
                return -b_;
            }

            [[nodiscard]] const BoundingBox &box() const {
                return box_;
            }
        };

        std::vector<Element> data_;
        std::vector<Node> tree_;

        SpatialIndex(std::vector<Element> data, std::vector<Node> tree)
                : data_(std::move(data)), tree_(std::move(tree))
        {}

    private:

        [[nodiscard]] static SpatialIndex<E> create(std::vector<SpatialIndex<E>::Element> elements) {
            const int bucket_size = 8;
            const int buckets = ceil(static_cast<double>(elements.size()) / bucket_size);

            if (elements.size() <= bucket_size) {
                std::vector<Node> tree{Node::make_leaf(elements, 0, elements.size()-1)};
                return SpatialIndex<E>(elements, tree);
            }

            const auto x_comparator = [](const Element &a, const Element &b) {
                const int a_width = a.box_.width();
                const int b_width = b.box_.width();
                if (a_width != b_width) {
                    return a_width < b_width;
                } else {
                    return a.box_.max_x() < b.box_.max_x();
                }
            };
            const auto y_comparator = [](const Element &a, const Element &b) {
                const int a_height = a.box_.height();
                const int b_height = b.box_.height();
                if (a_height != b_height) {
                    return a_height < b_height;
                } else {
                    return a.box_.max_y() < b.box_.max_y();
                }
            };


            struct Work {
                bool sort_x;
                int from;
                int to;

                Work(bool sort_x, int from, int to) : sort_x(sort_x), from(from), to(to) {}
            };

            std::deque<Work> work_queue;
            std::deque<std::pair<int, Node>> results;

            work_queue.emplace_back(Work(true, 0, elements.size()));

            while (!work_queue.empty()) {
                Work work = work_queue.front(); work_queue.pop_front();
                if (work.to - work.from <= bucket_size) {
                    // Note -1 on to, since Work is half-open ranges, while the nodes are closed ranges
                    results.emplace_back(std::make_pair(1, Node::make_leaf(elements, work.from, work.to-1)));
                } else {
                    auto begin = elements.begin() + work.from;
                    auto end = elements.begin() + work.to;
                    if (work.sort_x) {
                        std::sort(begin, end, x_comparator);
                    } else {
                        std::sort(begin, end, y_comparator);
                    }
                    const int mid = ceil((static_cast<double>(work.to + work.from)) / 2);
                    work_queue.emplace_back(Work(!work.sort_x, work.from, mid));
                    work_queue.emplace_back(Work(!work.sort_x, mid, work.to));
                }
            }

            std::vector<Node> tree;
            tree.reserve(2 * results.size());

            while (!results.empty()) {
                if (results.size() == 1) {
                    tree.emplace_back(results.front().second);
                    results.pop_front();
                    break;
                }
                if (results[0].first < results[1].first){
                    // The first element is of a lower level than the next element
                    // Shuffle the element to the back so that it is included in the right side of the tree.
                    auto front = results.front(); results.pop_front();
                    results.emplace_back(front);
                }
                const auto& [level_left, node_left] = results.front(); results.pop_front();
                const auto& [level_right, node_right] = results.front(); results.pop_front();
                const int pos_left = tree.size();
                const int pos_right = tree.size()+1;
                tree.emplace_back(node_left);
                tree.emplace_back(node_right);
                results.emplace_back(std::make_pair(
                        std::max(level_left, level_right) + 1,
                        Node::make_internal(tree, pos_left, pos_right)
                ));
            }


            return SpatialIndex(std::move(elements), std::move(tree));
        }
    public:
        [[nodiscard]] static SpatialIndex<E> create(const std::vector<E>& elements) {
            std::vector<SpatialIndex<E>::Element> internal_elements;
            internal_elements.reserve(elements.size());
            for (int i = 0; i < elements.size(); ++i) {
                internal_elements.emplace_back(SpatialIndex<E>::Element(elements[i].bounding_box(), elements[i]));
            }

            return create(std::move(internal_elements));
        }

        template<typename F>
        [[nodiscard]] static SpatialIndex<E> create(const std::vector<E>& elements, const F& bounding_box_extractor) {
            std::vector<SpatialIndex<E>::Element> internal_elements;
            internal_elements.reserve(elements.size());
            for (int i = 0; i < elements.size(); ++i) {
                internal_elements.emplace_back(SpatialIndex<E>::Element(bounding_box_extractor(elements[i]), elements[i]));
            }

            return create(std::move(internal_elements));
        }
        
        template<typename F>
        void visit(const BoundingBox& box, F visitor) const {
            visit(box, visitor, tree_.size()-1);
        }

        std::vector<std::reference_wrapper<const E>> collect(const BoundingBox& box) const {
            std::vector<std::reference_wrapper<const E>> result;
            visit(box, [&](const E& element) { result.emplace_back(std::ref(element)); });
            return result;
        }
    private:
        template<typename F>
        void visit(const BoundingBox& box, F visitor, int index) const {
            if (index < 0 || index >= tree_.size()) {
                return;
            }

            const Node &node = tree_[index];
            if (node.box().intersects(box)) {
                if (node.leaf()) {
                    const int from = node.from();
                    const int to = node.to();
                    for (int i = from; i <= to; ++i) {
                        if (data_[i].box_.intersects(box)) {
                            visitor(data_[i].element_);
                        }
                    }
                } else { // node.internal()
                    visit(box, visitor, tree_[index].left());
                    visit(box, visitor, tree_[index].right());
                }
            }
        }

        [[nodiscard]] int left(int index) const {
            return index * 2;
        }

        [[nodiscard]] int right(int index) const {
            return (index * 2) + 1;
        }

    public:
        void print(std::ostream& out) const {
            print(out, tree_.size()-1, 0);
        }
    private:
        void print(std::ostream& out, int pos, int tabs) const {
            for (int t = 0; t < tabs; ++t) {
                out << "\t";
            }
            if (tree_[pos].leaf()) {
                out << "Leaf " << pos << " " << tree_[pos].box() << " [";
                const int from = tree_[pos].from();
                const int to = tree_[pos].to();
                for (int element = from; element <= to; ++element) {
                    out << data_[element].box_;
                    if (element < to) {
                        out << ", ";
                    }
                }
                out << "]" << std::endl;
            } else {
                out << "Node " << pos << " " << tree_[pos].box() << std::endl;
                print(out, tree_[pos].left(),  tabs+1);
                print(out, tree_[pos].right(), tabs+1);
            }
        }
    };
}

#endif //HC_SPATIAL_INDEX_H
