#pragma clang diagnostic push
#pragma ide diagnostic ignored "performance-unnecessary-value-param"
#pragma ide diagnostic ignored "cppcoreguidelines-pro-type-static-cast-downcast"
#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCDFAInspection"
#pragma clang diagnostic ignored "-Wunknown-pragmas"
#pragma ide diagnostic ignored "OCSimplifyInspection"

#include "tsp.h"
#include "propagators/propagators.h"
#include "utilities/value_selection.h"

#include <iostream>
#include <gecode/driver.hh>
#include <gecode/kernel.hh>
#include <gecode/int.hh>
#include <utility>

using namespace Gecode;
using namespace std;

namespace hc {
    TSPModel::TSPModel(const InspectorTSPModelOptions &options)
            : IntMinimizeScript(options),
              instance_(options.instance()),
              succ_(*this, instance_->locations(), 0, instance_->locations() - 1),
              prev_(*this, instance_->locations(), 0, instance_->locations() - 1),
              edge_costs_(*this, instance_->locations(), 0,
                          *std::max_element(instance_->max_costs().begin(), instance_->max_costs().end())),
              tour_cost_(*this, 0, instance_->max_total_cost()),
              var_branching_(options.branching_var()),
              val_branching_(options.branching_val()),
              use_warnsdorff_dominated_edges_propagation_(options.use_warnsdorff_dominated_edges_propagation()),
              use_warnsdorff_dominated_edges2_propagation_(options.use_warnsdorff_dominated_edges2_propagation()),
              use_dominated_edges_propagation_(options.use_dominated_edges_propagation()),
              use_christofides_propagation_(options.use_christofides_propagation()),
              use_one_tree_propagation_(options.use_one_tree_propagation()),
              uses_half_checking_propagators_(false),
              use_all_nogoods_(options.use_all_nogoods()),
              warnsdorff_start_(0),
              next_warnsdorff_(warnsdorff_start_),
              rnd_(42) {
        // The TSP constraint cost-circuit
        // First, set up the cost matrix
        IntArgs costs(instance_->locations() * instance_->locations());
        for (int s = 0; s < instance_->locations(); ++s) {
            for (int e = 0; e < instance_->locations(); ++e) {
                const int pos = s * instance_->locations() + e;
                costs[pos] = instance_->line(s, e).length();
            }
        }
        // Then post the actual constraint
        circuit(*this, costs, succ_, edge_costs_, tour_cost_, options.ipl());

        // Connect the successors with the previous pointers
        channel(*this, succ_, prev_, options.ipl());

        // Symmetry breaking, tour goes forward
        // From the Gecode TSP example
        {
            IntVar points_to_0(*this, 0, instance_->locations() - 1);
            element(*this, succ_, points_to_0, 0);
            rel(*this, points_to_0, IRT_LE, succ_[0]);
        }
    }



    void TSPModel::configure_asset(const Gecode::MetaInfo &mi) {
        // Use new node for the Warnsdorff start
        warnsdorff_start_ = static_cast<int>(rnd()(succ_.size()));
        next_warnsdorff_ = warnsdorff_start_;

        // Set up half-checking propagators for additional assets
        if (mi.asset() != 0)
        {
            if (use_dominated_edges_propagation_) {
                set_uses_half_checking_propagators();
                hc::no_dominated_edge_pairs(*this, instance_, succ_);
            }

            if (use_warnsdorff_dominated_edges_propagation_) {
                set_uses_half_checking_propagators();
                hc::no_warnsdorff_dominated_edges(*this, instance_, warnsdorff_start_, succ_);
            }

            if (use_warnsdorff_dominated_edges2_propagation_) {
                set_uses_half_checking_propagators();
                hc::no_warnsdorff_dominated_edges2(*this, instance_, warnsdorff_start_, succ_);
            }

            if (use_christofides_propagation_) {
                set_uses_half_checking_propagators();
                hc::christofides(*this, instance_, succ_, tour_cost_);
            }

            if (use_one_tree_propagation_) {
                set_uses_half_checking_propagators();
                hc::hk_1tree(*this, instance_, succ_, prev_, tour_cost_);
            }
        }

        // Set up branching
        configure_branching();
    }

    void TSPModel::configure_branching() {
        // First, create the value selection that is going to be used
        auto biased_value_order = [](const Space &home, IntVar variable, int position) -> int {
            Rnd& rnd = static_cast<const TSPModel&>(home).rnd();
            return choose_value_biased<double>(
                    rnd,
                    variable,
                    [&](int value) -> double {
                        return position;
                    });
        };

        IntValBranch min_val_branch = INT_VAL(biased_value_order);
        IntValBranch val_branch;
        switch (val_branching_) {
            case ValBranching::First: {
                val_branch = INT_VAL(biased_value_order);
                break;
            }
            case ValBranching::Random: {
                auto random_value = [](const Space &home, IntVar variable, int position) -> int {
                    unsigned int size = variable.size();
                    unsigned int value_pos = static_cast<const TSPModel&>(home).rnd()(size);
                    Int::ViewValues<Int::IntView> iv(variable);
                    while (iv()) {
                        if (--size == 0) {
                            return iv.val();
                        }
                        ++iv;
                    }
                    GECODE_NEVER;
                    return variable.min();
                };
                val_branch = INT_VAL(random_value);
                break;
            }
            case ValBranching::MinLength: {
                auto biased_value_min_length = [](const Space &home, IntVar variable, int position) -> int {
                    Rnd& rnd = static_cast<const TSPModel&>(home).rnd();
                    auto instance = static_cast<const TSPModel&>(home).instance();
                    return choose_value_biased<double>(
                            rnd,
                            variable,
                            [&](int value) -> double {
                                return instance->line(position, value).length();
                            });
                };
                val_branch = INT_VAL(biased_value_min_length);
                break;
            }
            case ValBranching::MaxLength: {
                auto biased_value_max_length = [](const Space &home, IntVar variable, int position) -> int {
                    Rnd& rnd = static_cast<const TSPModel&>(home).rnd();
                    auto instance = static_cast<const TSPModel&>(home).instance();
                    return choose_value_biased<double>(
                            rnd,
                            variable,
                            [&](int value) -> double {
                                return -instance->line(position, value).length();
                            });
                };
                val_branch = INT_VAL(biased_value_max_length);
                break;
            }
            case ValBranching::MinDegreeMinLength: {
                auto biased_warnsdorff_value_min_length = [](const Space &home, IntVar variable, int position) -> int {
                    Rnd& rnd = static_cast<const TSPModel&>(home).rnd();
                    auto succ = static_cast<const TSPModel&>(home).succ();
                    auto instance = static_cast<const TSPModel&>(home).instance();
                    return choose_value_biased<pair<double, double>>(
                            rnd,
                            variable,
                            [&](int value) -> pair<double, double> {
                                auto out_degree = succ[value].size();
                                auto length = instance->line(position, value).length();
                                return make_pair(out_degree, length);
                            });
                };
                val_branch = INT_VAL(biased_warnsdorff_value_min_length);
                break;
            }
            case ValBranching::MinDegreeMaxLength: {
                auto biased_warnsdorff_value_max_length = [](const Space &home, IntVar variable, int position) -> int {
                    Rnd& rnd = static_cast<const TSPModel&>(home).rnd();
                    auto succ = static_cast<const TSPModel&>(home).succ();
                    auto instance = static_cast<const TSPModel&>(home).instance();
                    return choose_value_biased<pair<double, double>>(
                            rnd,
                            variable,
                            [&](int value) -> pair<double, double> {
                                auto out_degree = succ[value].size();
                                auto length = instance->line(position, value).length();
                                return make_pair(out_degree, -length);
                            });
                };
                val_branch = INT_VAL(biased_warnsdorff_value_max_length);
                break;
            }
        }

        // If this asset uses half-checking propagators, we need to ensure that no nogood recording is done.
        // Since we have no information in the master-function on which asset we are in when doing a restart,
        // we hack this in here by using a custom commit function mimicking the standard commit function, which
        // will as a side-effect turn of nogood recording.
        //
        // When the option use_all_nogoods is true, then we do keep the nogoods, even from half-checking propagators
        //
        if (uses_half_checking_propagators() && !use_all_nogoods_) {
            auto commit_without_nogood_recording = [](Space &home, unsigned int a, IntVar x, int i, int n) {
                if (a == 0U) {
                    rel(home, x, IRT_EQ, n);
                } else {
                    rel(home, x, IRT_NQ, n);
                }
            };
            val_branch = INT_VAL(
                    val_branch.val(),
                    commit_without_nogood_recording
            );
            min_val_branch = INT_VAL(
                    min_val_branch.val(),
                    commit_without_nogood_recording
            );
        }

        // Set up branching using the created value selection and the configured variable selection
        //
        switch (var_branching_) {
            case VarBranching::InputOrder: {
                branch(*this, succ_, INT_VAR_NONE(), val_branch);
            }
                break;
            case VarBranching::AfcSizeMin:
                branch(*this, succ_, INT_VAR_AFC_SIZE_MAX(), val_branch);
                break;
            case VarBranching::Warnsdorff:
                branch(*this, succ_, INT_VAR_MERIT_MAX(&warnsdorff_var_merit), val_branch);
                break;
            case VarBranching::CostRegret:
                // This branching is adapted from the Gecode TSP example

                // First enumerate cost values, prefer those where the worst choice gives the worst regret,
                // using biased minimum value ordering
                branch(*this, edge_costs_, INT_VAR_REGRET_MAX_MAX(), min_val_branch);

                // Then fix the successors, trying to set values along warnsdorff path
                branch(*this, succ_, INT_VAR_MERIT_MAX(&warnsdorff_var_merit), val_branch);
                break;
        }
    }

    bool TSPModel::master(const MetaInfo &mi) {
        switch (mi.type()) {
            case MetaInfo::RESTART:
                if (mi.last() != nullptr)
                    constrain(*mi.last());
                mi.nogoods().post(*this);
                // Perform a restart even if a solution has been found
                return true;
            case MetaInfo::PORTFOLIO:
                assert(BrancherGroup::all.size(*this) == 0 && "No branchers in the master space.");
                // Return value ignored
                return true;
            default:
                GECODE_NEVER;
                return true;
        }
    }

    bool TSPModel::slave(const MetaInfo &mi) {
        switch (mi.type()) {
            case MetaInfo::RESTART:
                // No need to do anything in a slave-space on restart.
                // If we want to add LNS, this is where that would go.
                break;
            case MetaInfo::PORTFOLIO:
                configure_asset(mi);
                break;
        }
        bool is_complete_search = !uses_half_checking_propagators();
        return is_complete_search;
    }

    bool TSPModel::uses_half_checking_propagators() const {
        return uses_half_checking_propagators_;
    }

    void TSPModel::set_uses_half_checking_propagators() {
        uses_half_checking_propagators_ = true;
    }

    TSPModel::TSPModel(TSPModel &s) :
            IntMinimizeScript(s),
            instance_(s.instance_),
            var_branching_(s.var_branching_),
            val_branching_(s.val_branching_),
            use_warnsdorff_dominated_edges_propagation_(s.use_warnsdorff_dominated_edges_propagation_),
            use_warnsdorff_dominated_edges2_propagation_(s.use_warnsdorff_dominated_edges2_propagation_),
            use_dominated_edges_propagation_(s.use_dominated_edges_propagation_),
            use_christofides_propagation_(s.use_christofides_propagation_),
            use_one_tree_propagation_(s.use_one_tree_propagation_),
            uses_half_checking_propagators_(s.uses_half_checking_propagators_),
            use_all_nogoods_(s.use_all_nogoods_),
            warnsdorff_start_(s.warnsdorff_start_),
            next_warnsdorff_(s.next_warnsdorff_),
            rnd_(s.rnd_.seed()) {
        succ_.update(*this, s.succ_);
        prev_.update(*this, s.prev_);
        edge_costs_.update(*this, s.edge_costs_);
        tour_cost_.update(*this, s.tour_cost_);
    }

    TSPModel *TSPModel::copy() {
        return new TSPModel(*this);
    }


    void TSPModel::print(std::ostream &os) const {
        if (succ_.assigned()) {
            os << "Tour : ";
            int pos = 0;
            do {
                os << instance_->location(pos) << " ";
                pos = succ_[pos].val();
            } while (pos != 0);
            os << std::endl;
        }
        os << "Successors : " << succ_ << std::endl;
        os << "Edge costs : " << edge_costs_ << std::endl;
        os << "Total cost : " << cost() << std::endl;
    }

    IntVar TSPModel::cost() const {
        return tour_cost_;
    }

    const shared_ptr<const TSPInstance> &TSPModel::instance() const {
        return instance_;
    }

    const IntVarArray &TSPModel::succ() const {
        return succ_;
    }

    const IntVarArray &TSPModel::prev() const {
        return prev_;
    }

    const IntVarArray &TSPModel::edge_costs() const {
        return edge_costs_;
    }

    const IntVar &TSPModel::tour_cost() const {
        return tour_cost_;
    }

    double TSPModel::warnsdorff_var_merit(const Gecode::Space &home, Gecode::IntVar x, int position) {
        const auto& tsp = static_cast<const TSPModel&>(home);
        if (position == tsp.next_warnsdorff()) {
            return 1.0;
        } else {
            return 0.0;
        }
    }


    int TSPModel::next_warnsdorff() const {
        while (succ_[next_warnsdorff_].assigned()) {
            next_warnsdorff_ = succ_[next_warnsdorff_].val();
            if (next_warnsdorff_ == warnsdorff_start_) {
                // Looped
                break;
            }
        }
        return next_warnsdorff_;
    }

    Rnd &TSPModel::rnd() const {
        return rnd_;
    }

    InspectorTSPModelOptions::InspectorTSPModelOptions()
            : TSPModelOptions()
    {
    }

    void InspectorTSPModelOptions::parse(int &argc, char **argv) {
        TSPModelOptions::parse(argc, argv);
    }

}

#pragma clang diagnostic pop
#pragma clang diagnostic pop
