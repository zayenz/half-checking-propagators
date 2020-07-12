#pragma clang diagnostic push
#pragma ide diagnostic ignored "cppcoreguidelines-pro-type-static-cast-downcast"

#ifndef HC_TSP_MODEL_H
#define HC_TSP_MODEL_H

#include <cstring>
#include <cstdlib>
#include <memory>
#include <optional>

#include <gecode/driver.hh>
#include <gecode/int.hh>

#include "utilities/tsp.h"
#include "tsp_common.h"


namespace hc {

    class InspectorTSPModelOptions : public TSPModelOptions {
    public:
        InspectorTSPModelOptions();

        void parse(int& argc, char* argv[]);
    };

    class TSPModel : public Gecode::IntMinimizeScript {
    private:
        /// The TSP instance
        const std::shared_ptr<const TSPInstance> instance_;
        /// The successor variables
        Gecode::IntVarArray succ_;
        /// The previous-variables, the inverse of the successors
        Gecode::IntVarArray prev_;
        /// The cost of each edge
        Gecode::IntVarArray edge_costs_;
        /// The total tour cost
        Gecode::IntVar tour_cost_;
        /// The variable branching that should be used
        const VarBranching var_branching_;
        /// The value branching that should be used
        const ValBranching val_branching_;
        /// When true, use warnsdorff dominated edges propagation in one asset
        const bool use_warnsdorff_dominated_edges_propagation_;
        /// When true, use warnsdorff dominated edges propagation in one asset
        const bool use_warnsdorff_dominated_edges2_propagation_;
        /// When true, use dominated edges propagation in one asset
        const bool use_dominated_edges_propagation_;
        /// When true, use christofides bounds
        const bool use_christofides_propagation_;
        /// When true, use one tree propagation in one asset
        const bool use_one_tree_propagation_;
        /// When true, this instance is known to use half-checking propagators.
        /// Starts out as false, but when set it remains true
        bool uses_half_checking_propagators_;
        /// Use all no-goods (even from half-checking propagators)
        bool use_all_nogoods_;
        /// Starting position for
        int warnsdorff_start_;
        /// The next variable when starting from warnsdorff_start_
        mutable int next_warnsdorff_;
        /// A source of randomness
        mutable Gecode::Rnd rnd_;
    public:
        /// Construction of the model.
        explicit TSPModel(const InspectorTSPModelOptions& opts);

        /// Constructor for cloning \a s
        TSPModel(TSPModel &s);

        /// Copy space during cloning
        TSPModel *copy() override;

        /// Print solution
        void print(std::ostream &os) const override;

        /// The cost of a solution for optimization purposes
        [[nodiscard]] Gecode::IntVar cost() const override;

        [[nodiscard]] const std::shared_ptr<const TSPInstance> &instance() const;

        [[nodiscard]] int warnsdorff_start() const { return warnsdorff_start_; };

        [[nodiscard]] int warnsdorff_next() const { return next_warnsdorff_; };

        [[nodiscard]] const Gecode::IntVarArray &succ() const;

        [[nodiscard]] const Gecode::IntVarArray &prev() const;

        [[nodiscard]] const Gecode::IntVarArray &edge_costs() const;

        [[nodiscard]] const Gecode::IntVar &tour_cost() const;

        /// Check if this Space uses half-checking propagators
        [[nodiscard]] bool uses_half_checking_propagators() const;

        /// Set that this space (and thus all descendant spaces) uses half-checking propagators
        void set_uses_half_checking_propagators();

        Gecode::Rnd &rnd() const;

        bool master(const Gecode::MetaInfo &mi) override;

        bool slave(const Gecode::MetaInfo &mi) override;

        void configure_asset(const Gecode::MetaInfo &mi);

        void configure_branching();

    private:
        /// The next variable to assign according to Warnsdorff
        int next_warnsdorff() const;

        /// Merit function that gives merit 1.0 based on if the variable is the end of the warnsdorff-chain, else 0.0
        static double warnsdorff_var_merit(const Gecode::Space& home, Gecode::IntVar x, int position);

    };

#if defined(HC_USE_QT)
    /// Inspector showing a TSP tour
    ///
    /// Inspired by the Knights tour inspector from Gecode
    class TourInspector : public Gecode::Gist::Inspector {
    protected:
        /// The graphics scene displaying the board
        QGraphicsScene* scene_;
        /// The window containing the graphics scene
        QMainWindow* mw_;
    public:
        /// Constructor
        TourInspector() : scene_(nullptr), mw_(nullptr) {}
        /// Inspect space \a s
        void inspect(const Gecode::Space& s) override;
        /// Set up main window
        void initialize();
        /// Name of the inspector
        std::string name() override { return "TSP Visualiser"; }
        /// Finalize inspector
        void finalize() override {
            delete mw_;
            mw_ = nullptr;
        }
    };
#endif

}

#endif //HC_TSP_MODEL_H

#pragma clang diagnostic pop
