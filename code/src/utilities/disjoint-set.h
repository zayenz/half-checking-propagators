#ifndef HC_DISJOINT_SET_H
#define HC_DISJOINT_SET_H

#include <vector>
#include <utility>

namespace hc {
    /**
     * Classic class for representing a set of disjoint sets, with efficient join and same_set checks.
     *
     * The sets are store as their indices in [0..n). If another data-type is needed, an external mapping
     * should be used.
     */
    class UnionFind {
        /// The sets structure, a tree of parent-pointers.
        /// These are negative when they are the root of a set, and the magnitude then represents the size of the set.
        /// When non-negative, then they point to a parent closer to the root.
        /// Note that the parent-pointers will change using path-compression in find.
        std::vector<int> sets_;

        int set_count_;
    public:

        /// Create UnionFind-structure for n elements
        explicit UnionFind(int n)
                : set_count_(n) {
            sets_.assign(n, -1);
        }

        /// True iff a and b are in the same set
        [[nodiscard]] bool same_set(int a, int b){
            return find(a) == find(b);
        }

        /// Join the sets for a and b
        void join(int a, int b) {
            int root_a = find(a);
            int root_b = find(b);
            if (root_a == root_b) {
                // Already same sets, no need to do anything more
                return;
            }
            if (sets_[root_a] > sets_[root_b]) {
                // Swap so that root_a is the larger set
                std::swap(root_a, root_b);
            }
            sets_[root_a] += sets_[root_b];
            sets_[root_b] = root_a;
            set_count_ -= 1;
        }

        /// Find set−head with path−compression
        [[nodiscard]] int find(int x) {
            if (sets_[x] < 0) {
                // Found a root
                return x;
            } else {
                int x_root = find(sets_[x]);
                // Compress path
                sets_[x] = x_root;
                return x_root;
            }
        }

        /// The number of sets
        [[nodiscard]] int set_count() const {
            return set_count_;
        }

        /// The size of a set
        [[nodiscard]] int set_size(int x) {
            return -sets_[find(x)];
        }


    };

}

#endif //HC_DISJOINT_SET_H
