#include <memory>

#include <gecode/int.hh>
#include <utility>
#include "utilities/tsp.h"
#include "utilities/graph.h"

using namespace hc;
using namespace Gecode;
using namespace std;

typedef MixNaryOnePropagator<Int::IntView, Int::PC_INT_DOM, Int::IntView, PC_GEN_NONE> Base;

class ChristofidesBounds : public Base {
protected:
    using Base::x;
    using Base::y;
    shared_ptr<const TSPInstance> instance_;
public:
    // posting
    ChristofidesBounds(Space &home, ViewArray<Int::IntView>& successors, Int::IntView cost, shared_ptr<const TSPInstance> instance)
            : Base(home, successors, cost),
              instance_(std::move(instance)) {
        home.notice(*this, AP_WEAKLY);
        home.notice(*this, AP_DISPOSE);
    }

    static ExecStatus post(Space &home,
                           ViewArray<Int::IntView>& successors,
                           Int::IntView cost,
                           shared_ptr<const TSPInstance> instance) {
        auto *propagator = new(home) ChristofidesBounds(home, successors, cost, std::move(instance));
        return ES_OK;
    }

    // disposal
    size_t dispose(Space &home) override {
        home.ignore(*this, AP_DISPOSE);
        home.ignore(*this, AP_WEAKLY);
        instance_.~shared_ptr();
        (void) Base::dispose(home);
        return sizeof(*this);
    }

    // copying
    ChristofidesBounds(Space &home, ChristofidesBounds &p)
            : Base(home, p),
              instance_(p.instance_) {
    }

    Propagator *copy(Space &home) override {
        return new(home) ChristofidesBounds(home, *this);
    }

    // cost computation
    [[nodiscard]] PropCost cost(const Space &, const ModEventDelta &) const override {
        return PropCost::crazy(PropCost::Mod::HI, x.size());
    }


    // propagation
    ExecStatus propagate(Space &home, const ModEventDelta &) override {
        if (x.assigned()) {
            // No need to remove anything for assignments,
            // checking is handled by the circuit propagator
            return home.ES_SUBSUMED(*this);
        }

        vector<LineSegment> mandatory;
        mandatory.reserve(x.size());
        for (int node = 0; node < x.size(); ++node) {
            if (x[node].assigned()) {
                mandatory.emplace_back(instance_->line(node, x[node].val()));
            }
        }

        unsigned int dom_sum = 0;
        for (const auto &node : x) {
            dom_sum += node.size();
        }
        // Magic number for when to use all edges vs collect current - value from tests using berlin52.tsp for hk_1tree.
        vector<LineSegment> circuit;
        if (dom_sum > 0.25 * (x.size() * x.size())) {
            auto opt = christofides(instance_,
                    x.size(),
                    mandatory,
                    instance_->lines_length_ordered(),
                                   [&](const LineSegment &edge) {
                                       // This is a O(n) operation in the worst case, but as long as most variables are ranges it is quick, since
                                       // the n here is the number of ranges in the variable, not the domain size
                                       return x[edge.start_id()].in(edge.end_id());
                                   });
            if (opt.has_value()) {
                circuit = opt.value();
            } else {
                // Could not create circuit
                return ES_FIX;
            }
        } else {
            std::vector<LineSegment> lines;
            for (int start = 0; start < x.size(); ++start) {
                if (!x[start].assigned()) {
                    Int::ViewValues iv(x[start]);
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
            auto opt = christofides(instance_,
                                    x.size(),
                                    mandatory,
                                    lines,
                                    [&](const LineSegment &edge) { return true; });
            if (opt.has_value()) {
                circuit = opt.value();
            } else {
                // Could not create circuit
                return ES_FIX;
            }
        }

        int cost = 0;
        for (const auto &edge : circuit) {
            cost += edge.length();
        }

        GECODE_ME_CHECK(y.lq(home, cost));

        return ES_FIX;
    }
};

namespace hc {
    void christofides(Home home, std::shared_ptr<const TSPInstance> instance, const IntVarArgs& successors_var,  const IntVar cost_var) {
        ViewArray<Int::IntView> successors(home, successors_var);
        Int::IntView cost(cost_var);


        if (ChristofidesBounds::post(home, successors, cost, std::move(instance)) != ES_OK) {
            home.fail();
        }
    }
}
