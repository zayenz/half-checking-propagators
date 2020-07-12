#include <memory>

#include <gecode/int.hh>
#include <utility>
#include <utilities/tsp.h>

using namespace hc;
using namespace Gecode;
using namespace std;

typedef NaryPropagator<Int::IntView, Int::PC_INT_VAL> NaryBase;

class NoWarnsdorffDominatedEdges2 : public NaryBase {
    protected:
    using NaryBase::x;
    shared_ptr<const TSPInstance> instance_;
    int start_node_;
    int current_node_index_;
    vector<bool> assigned_line_collected_;
    vector<bool> node_propagated_;
    vector<LineSegment> assigned_lines_;
    public:
    // posting
    NoWarnsdorffDominatedEdges2(Space &home, ViewArray<Int::IntView> &successors, int start_node,
                                shared_ptr<const TSPInstance> instance)
            : NaryPropagator(home, successors),
              instance_(std::move(instance)),
              start_node_(start_node),
              current_node_index_(start_node),
              assigned_line_collected_(x.size(), false),
              node_propagated_(x.size(), false),
              assigned_lines_() {
      home.notice(*this, AP_WEAKLY);
      home.notice(*this, AP_DISPOSE);
    }

    static ExecStatus post(Space &home,
                           ViewArray<Int::IntView> &successors,
                           int start_node,
                           shared_ptr<const TSPInstance> instance) {
      auto *propagator = new(home) NoWarnsdorffDominatedEdges2(home, successors, start_node, std::move(instance));
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
      assigned_line_collected_.~vector();
      assigned_lines_.~vector();
      (void) NaryPropagator::dispose(home);
      return sizeof(*this);
    }

    // copying
    NoWarnsdorffDominatedEdges2(Space &home, NoWarnsdorffDominatedEdges2 &p)
            : NaryBase(home, p),
              instance_(p.instance_),
              start_node_(p.start_node_),
              current_node_index_(p.current_node_index_),
              assigned_line_collected_(p.assigned_line_collected_),
              assigned_lines_(p.assigned_lines_) {
    }

    Propagator *copy(Space &home) override {
      return new(home) NoWarnsdorffDominatedEdges2(home, *this);
    }

    // cost computation
    [[nodiscard]] PropCost cost(const Space &, const ModEventDelta &) const override {
      return PropCost::crazy(PropCost::Mod::HI, x.size());
    }

    void collect_assigned_lines() {
      for (int i = 0; i < x.size(); ++i) {
        if (!assigned_line_collected_[i] && x[i].assigned()) {
          assigned_line_collected_[i] = true;
          assigned_lines_.emplace_back(instance_->line(i, x[i].val()));
        }
      }
    }

    void advance_current_node() {
      while (x[current_node_index_].assigned()) {
        current_node_index_ = x[current_node_index_].val();
        if (current_node_index_ == start_node_) {
          return;
        }
      }
    }

    // propagation
    ExecStatus propagate(Space &home, const ModEventDelta &) override {
      if (x.assigned()) {
        // No need to remove anything for assignments,
        // checking is handled by the circuit propagator
        return home.ES_SUBSUMED(*this);
      }
      collect_assigned_lines();
      advance_current_node();

      // Ensure that each node is propagated only once.
      if (node_propagated_[current_node_index_]) {
        return ES_FIX;
      }
      node_propagated_[current_node_index_] = true;

      Int::IntView &current_node = x[current_node_index_];
      if (current_node.assigned()) {
        // No need to remove anything for assignments,
        // checking is handled by the circuit propagator
        return ES_FIX;
      }

      Region region;
      int *to_remove = region.alloc<int>(current_node.size());
      int to_remove_count = 0;
      Int::ViewValues<Gecode::Int::IntView> iv(current_node);
      while (iv()) {
        const int next_node = iv.val();
        LineSegment line = instance_->line(current_node_index_, next_node);
        for (const auto &other_line : assigned_lines_) {
          const bool lines_not_connected = line.start_id() != other_line.end_id() &&
                                           line.end_id() != other_line.start_id();
          if (lines_not_connected &&
              intersects(line, other_line)) {
            to_remove[to_remove_count++] = next_node;
            break;
          }
        }
        ++iv;
      }

      if (to_remove_count == 0) {
        return ES_FIX;
      } else {
        // Make sure we do not cause failures, so that we can reach a solution
        if (to_remove_count >= current_node.size()) return ES_NOFIX;
        Support::quicksort(to_remove, to_remove_count);
        auto to_remove_iterator = Iter::Values::Array(to_remove, to_remove_count);
        GECODE_ME_CHECK(current_node.minus_v(home, to_remove_iterator, false));
        return ES_NOFIX;
      }
    }
};

namespace hc {
    void no_warnsdorff_dominated_edges2(Home home, std::shared_ptr<const hc::TSPInstance> instance, int start_node,
                                        const Gecode::IntVarArgs &args) {
      ViewArray<Int::IntView> successors(home, args);

      if (NoWarnsdorffDominatedEdges2::post(home, successors, start_node, std::move(instance)) != ES_OK) {
        home.fail();
      }
    }
}
