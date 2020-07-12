#include <memory>

#include <gecode/int.hh>
#include <utility>
#include <utilities/tsp.h>

#include "utilities/graph.h"

using namespace hc;
using namespace Gecode;
using namespace std;

// From https://stackoverflow.com/a/17299623/33965
template <typename T>
std::vector<T> flatten(const std::vector<std::vector<T>>& v) {
    std::size_t total_size = 0;
    for (const auto& sub : v) {
        total_size += sub.size();
    }
    std::vector<T> result;
    result.reserve(total_size);
    for (const auto& sub : v) {
        result.insert(result.end(), sub.begin(), sub.end());
    }
    return result;
}

template <typename T>
std::vector<T> flatten(const std::vector<std::optional<T>>& v) {
    std::size_t total_size = 0;
    for (const auto& opt : v) {
        total_size += opt.has_value();
    }
    std::vector<T> result;
    result.reserve(total_size);
    for (const auto& opt : v) {
        if (opt.has_value()) {
            result.emplace_back(opt.value());
        }
    }
    return result;
}

typedef MixNaryOnePropagator<Int::IntView, Int::PC_INT_DOM, Int::IntView, PC_GEN_NONE> Base;

class HKOneTreePropagator : public Propagator {
protected:
    ViewArray<Int::IntView> succ_;
    ViewArray<Int::IntView> pred_;
    Int::IntView cost_;
    shared_ptr<const TSPInstance> instance_;
    vector<bool> assigned_collected_;
    // Each sub-vector will eventually contain two line segments, the incoming (first) and the outgoing (last)
    vector<optional<LineSegment>> assigned_out_;
    vector<optional<LineSegment>> assigned_in_;
public:
    // posting
    HKOneTreePropagator(Space &home,
                        ViewArray<Int::IntView>& successors,
                        ViewArray<Int::IntView>& predecessors,
                        Int::IntView cost,
                        shared_ptr<const TSPInstance> instance)
            : Propagator(home),
              succ_(successors),
              pred_(predecessors),
              cost_(cost),
              instance_(std::move(instance)),
              assigned_collected_(succ_.size(), false),
              assigned_in_(succ_.size(), optional<LineSegment>()),
              assigned_out_(succ_.size(), optional<LineSegment>())
    {
        home.notice(*this, AP_WEAKLY);
        home.notice(*this, AP_DISPOSE);

        // We only need to schedule the successors, since the predecessors are functionally determined from these
        succ_.subscribe(home, *this, Int::PC_INT_DOM, true);
    }

    static ExecStatus post(Space &home,
                           ViewArray<Int::IntView>& successors,
                           ViewArray<Int::IntView>& predecessors,
                           Int::IntView cost,
                           shared_ptr<const TSPInstance> instance) {
        auto *propagator = new(home) HKOneTreePropagator(home, successors, predecessors, cost, std::move(instance));
        return ES_OK;
    }

    // disposal
    size_t dispose(Space &home) override {
        home.ignore(*this, AP_DISPOSE);
        home.ignore(*this, AP_WEAKLY);
        succ_.cancel(home, *this, Int::PC_INT_DOM);
        instance_.~shared_ptr();
        assigned_collected_.~vector();
        assigned_in_.~vector();
        assigned_out_.~vector();
        (void) Propagator::dispose(home);
        return sizeof(*this);
    }

    void reschedule(Space &home) override {
        succ_.reschedule(home, *this, Int::PC_INT_DOM);
    }

    // copying
    HKOneTreePropagator(Space &home, HKOneTreePropagator &p)
            : Propagator(home, p),
              instance_(p.instance_),
              assigned_collected_(p.assigned_collected_),
              assigned_in_(p.assigned_in_),
              assigned_out_(p.assigned_out_) {
        succ_.update(home, p.succ_);
        pred_.update(home, p.pred_);
        cost_.update(home, p.cost_);
    }

    Propagator *copy(Space &home) override {
        return new(home) HKOneTreePropagator(home, *this);
    }

    // cost computation
    [[nodiscard]] PropCost cost(const Space &, const ModEventDelta &) const override {
        return PropCost::crazy(PropCost::Mod::HI, succ_.size());
    }

    void collect_assigned_lines() {
        for (int i = 0; i < succ_.size(); ++i) {
            if (!assigned_collected_[i] && succ_[i].assigned()) {
                assigned_collected_[i] = true;
                const LineSegment &line = instance_->line(i, succ_[i].val());
                assigned_out_[line.start_id()].emplace(line);
                assigned_in_[line.end_id()].emplace(line);
            }
        }
    }

    int choose_excluded_node() {
        // Just grabs the first available without any mandatory edges
        for (int i = 0; i < succ_.size(); ++i) {
            if (!assigned_out_[i].has_value() && !assigned_in_[i].has_value()) {
                return i;
            }
        }

        // If all nodes have some mandatory edge, then choose the first non-assigned
        for (int i = 0; i < succ_.size(); ++i) {
            if (!succ_[i].assigned()) {
                assert(assigned_out_[i].has_value() != assigned_in_[i].has_value());
                return i;
            }
        }

        GECODE_NEVER;
    }

    OneTree make_one_tree() {
        const vector<LineSegment> &mandatory = flatten(assigned_out_);
        const int excluded_node = choose_excluded_node();

        unsigned int dom_sum = 0;
        for (const auto &node : succ_) {
            dom_sum += node.size();
        }
        // Magic number for when to use all edges vs collect current - value from tests using berlin52.tsp.
        if (dom_sum > 0.25 * (succ_.size() * succ_.size())) {
            return kruskal_1_tree(succ_.size(),
                                  excluded_node,
                                  mandatory,
                                  instance_->lines_length_ordered(),
                                  [&](const LineSegment &edge) {
                                      // This is a O(n) operation in the worst case, but as long as most variables are ranges it is quick, since
                                      // the n here is the number of ranges in the variable, not the domain size
                                      return succ_[edge.start_id()].in(edge.end_id());
                                  });
        } else {
            std::vector<LineSegment> lines;
            for (int start = 0; start < succ_.size(); ++start) {
                if (!succ_[start].assigned()) {
                    Int::ViewValues iv(succ_[start]);
                    while (iv()) {
                        int end = iv.val();
                        lines.emplace_back(instance_->line(start, end));
                        ++iv;
                    }
                }
            }
            std::sort(lines.begin(), lines.end(), [](LineSegment& a, LineSegment& b){
                return a.length() < b.length();
            });
            return kruskal_1_tree(succ_.size(),
                                  excluded_node,
                                  mandatory,
                                  instance_->lines_length_ordered(),
                                  [&](const LineSegment &edge) { return true; });
        }
    }

// propagation
    ExecStatus propagate(Space &home, const ModEventDelta &) override {
        if (succ_.assigned()) {
            // No need to remove anything for assignments,
            // checking is handled by the circuit propagator
            return home.ES_SUBSUMED(*this);
        }

        collect_assigned_lines();
        OneTree one_tree = make_one_tree();

        GECODE_ME_CHECK(cost_.gq(home, one_tree.size()));

        bool is_circuit = true;
        for (int i = 0; i < succ_.size(); ++i) {
            if (one_tree.edges_at(i).size() != 2) {
                is_circuit = false;
                break;
            }
        }

        if (is_circuit) {
            // The one-tree is actually an optimal solution!
            GECODE_ME_CHECK(cost_.eq(home, one_tree.size()));

            // We don't care which way the lines go, so each node is restricted
            // to be either the one pointing in or out from it. Later propagation
            // will, using the symmetry breaking, decide on the order
            for (int i = 0; i < succ_.size(); ++i) {
                int neighbor_1 = one_tree.edges_at(i)[0].id_not(i);
                int neighbor_2 = one_tree.edges_at(i)[1].id_not(i);
                int to_keep[2] = {min(neighbor_1, neighbor_2), max(neighbor_1, neighbor_2)};
                auto to_keep_iterator = Iter::Values::Array(to_keep, 2);
                GECODE_ME_CHECK(succ_[i].inter_v(home, to_keep_iterator, false));
            }

            return home.ES_SUBSUMED(*this);
        }

        int degree = 2;
        int start_node = -1;
        int end_node = -1;
        for (int i = 0; i < succ_.size(); ++i) {
            if (one_tree.edges_at(i).size() > degree) {
                degree = one_tree.edges_at(i).size();
                start_node = i;
                int worst_length = -1;
                for (const auto &edge : one_tree.edges_at(i)) {
                    if (edge.length() > worst_length) {
                        worst_length = edge.length();
                        end_node = edge.end_id();
                    }
                }
            }
        }

        if (start_node != -1) {
            GECODE_ME_CHECK(succ_[start_node].nq(home, end_node));
            GECODE_ME_CHECK(pred_[end_node].nq(home, start_node));
        }

        return ES_FIX;
    }
};

namespace hc {
    void hk_1tree(Home home, std::shared_ptr<const TSPInstance> instance, const IntVarArgs& successors_var,  const IntVarArgs& predecessors_var, const IntVar cost_var) {
        ViewArray<Int::IntView> successors(home, successors_var);
        ViewArray<Int::IntView> predecessors(home, predecessors_var);
        Int::IntView cost(cost_var);

        if (HKOneTreePropagator::post(home, successors, predecessors, cost, std::move(instance)) != ES_OK) {
            home.fail();
        }
    }
}

