#include <memory>

#include <gecode/int.hh>
#include <utility>
#include <utilities/tsp.h>

using namespace hc;
using namespace Gecode;
using namespace std;

typedef NaryPropagator<Int::IntView, Int::PC_INT_VAL> NaryBase;

class NoDominatedEdgePairs : public NaryBase {
protected:
    using NaryBase::x;
    shared_ptr<const TSPInstance> instance_;
    vector<bool> propagated_;
public:
    // posting
    NoDominatedEdgePairs(Space &home, ViewArray<Int::IntView>& successors, shared_ptr<const TSPInstance> instance)
            : NaryPropagator(home, successors), instance_(std::move(instance)), propagated_(x.size(), false) {
        home.notice(*this, AP_WEAKLY);
        home.notice(*this, AP_DISPOSE);
    }

    static ExecStatus post(Space &home,
                           ViewArray<Int::IntView>& successors,
                           shared_ptr<const TSPInstance> instance) {
        auto *propagator = new(home) NoDominatedEdgePairs(home, successors, std::move(instance));
        for (const auto &successor : successors) {
            if (successor.assigned()) {
                Int::IntView::schedule(home, *propagator, Int::ME_INT_VAL);
                break;
            }
        }
        return ES_OK;
    }

    // disposal
    size_t dispose(Space &home) override {
        home.ignore(*this, AP_DISPOSE);
        home.ignore(*this, AP_WEAKLY);
        instance_.~shared_ptr();
        propagated_.~vector();
        (void) NaryPropagator::dispose(home);
        return sizeof(*this);
    }

    // copying
    NoDominatedEdgePairs(Space &home, NoDominatedEdgePairs &p)
            : NaryBase(home, p), instance_(p.instance_), propagated_(p.propagated_) {
    }

    Propagator *copy(Space &home) override {
        return new(home) NoDominatedEdgePairs(home, *this);
    }

    // cost computation
    [[nodiscard]] PropCost cost(const Space &, const ModEventDelta &) const override {
        return PropCost::quadratic(PropCost::Mod::LO, x.size());
    }

    // // re-scheduling
    //void reschedule(Space &home) override {
    //}

    // propagation
    ExecStatus propagate(Space &home, const ModEventDelta &) override {
        DominatedEdges &edges = instance_->dominated_edges();

        for (int i = 0; i < x.size(); ++i) {
            if (!propagated_[i]) {
                if (x[i].assigned()) {
                    const vector<Edge> &dominated_edges = edges.dominated(Edge(i, x[i].val()));
                    for (const auto &dominated_edge : dominated_edges) {
                        GECODE_ME_CHECK(x[dominated_edge.from()].nq(home, dominated_edge.to()));
                    }

                    propagated_[i] = true;
                }
            }
        }
        
        return ES_FIX;
    }
};

namespace hc {
    void no_dominated_edge_pairs(Home home, std::shared_ptr<const hc::TSPInstance> instance, const Gecode::IntVarArgs& args) {
        ViewArray<Int::IntView> successors(home, args);

        if (NoDominatedEdgePairs::post(home, successors, std::move(instance)) != ES_OK) {
            home.fail();
        }
    }
}
