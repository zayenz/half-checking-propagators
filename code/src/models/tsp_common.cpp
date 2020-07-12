#pragma clang diagnostic push
#pragma ide diagnostic ignored "performance-unnecessary-value-param"
#pragma ide diagnostic ignored "cppcoreguidelines-pro-type-static-cast-downcast"
#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCDFAInspection"
#pragma clang diagnostic ignored "-Wunknown-pragmas"
#pragma ide diagnostic ignored "OCSimplifyInspection"

#include "tsp_common.h"

#include <iostream>
#include <gecode/driver.hh>
#include <gecode/kernel.hh>
#include <gecode/int.hh>
#include <utility>

using namespace Gecode;
using namespace std;

namespace hc {
    TSPModelOptions::TSPModelOptions()
            : Options("TSP"),
              branching_val_("branching-val", "The value ordering to use for branching"),
              tsp_grid_size_("tsp-grid", "When given a positive integer, create a grid of this size as instance", 0),
              tsp_data_file_("file", "The TSPLib data file to read", ""),
              use_dominated_edges_propagation_("domination-propagation", "When true, propagate dominated edges",
                                               false),
              use_warnsdorff_dominated_edges_propagation_("warnsdorff-domination-propagation", "When true, propagate warnsdorff dominated edges",
                                                          false),
              use_warnsdorff_dominated_edges2_propagation_("warnsdorff-domination-2-propagation", "When true, propagate warnsdorff dominated edges v2",
                                                          false),
              use_one_tree_propagation_("one-tree-propagation", "When true, propagate using one tree analysis",
                                               false),
              use_christofides_propagation_("christofides-propagation", "When true, propagate using christofides analysis",
                                            false),
              use_all_nogoods_("use-all-nogoods", "When true, use all nogoods, even from half-checking propagators",
                               false)
    {
        add(branching_val_);
        add(tsp_data_file_);
        add(tsp_grid_size_);
        add(use_dominated_edges_propagation_);
        add(use_warnsdorff_dominated_edges_propagation_);
        add(use_warnsdorff_dominated_edges2_propagation_);
        add(use_one_tree_propagation_);
        add(use_christofides_propagation_);
        add(use_all_nogoods_);

        branching(static_cast<int>(VarBranching::InputOrder),
                  "input-order",
                  "use the input order for variable with selected value ordering.");
        branching(static_cast<int>(VarBranching::AfcSizeMin),
                  "afc",
                  "use the AFC over size order min value first for variable with selected value ordering.");
        branching(static_cast<int>(VarBranching::Warnsdorff),
                  "warnsdorff",
                  "use the Warnsdorff rule for variable with selected value ordering.");
        branching(static_cast<int>(VarBranching::CostRegret),
                  "cost-regret",
                  "branch on cost variables using regret for variable with biased min value, "
                  "then min degree variable with selected value ordering.");

        branching_val_.add(static_cast<int>(ValBranching::First),
                           "first",
                           "use first (minimum) value, biased.");
        branching_val_.add(static_cast<int>(ValBranching::Random),
                           "random",
                           "use random value.");
        branching_val_.add(static_cast<int>(ValBranching::MinLength),
                           "min-length",
                           "use minimum length value, biased.");
        branching_val_.add(static_cast<int>(ValBranching::MaxLength),
                           "max-length",
                           "use maximum length value, biased.");
        branching_val_.add(static_cast<int>(ValBranching::MinDegreeMinLength),
                           "min-degree-min-length",
                           "use minimum degree of target (warnsdorff), then minimum length value, biased.");
        branching_val_.add(static_cast<int>(ValBranching::MinDegreeMaxLength),
                           "min-degree-max-length",
                           "use minimum degree of target (warnsdorff), then maximum length value, biased.");

        // Configuration for the standard set-up
        //

        // Set up Warnsdorff heuristic, using biased value selection
        branching(static_cast<int>(VarBranching::Warnsdorff));
        branching_val_.value(static_cast<int>(ValBranching::MinDegreeMinLength));

        // Use strong filtering
        ipl(Gecode::IPL_DOM);
        // Find all solutions
        solutions(0);
        // Time limit can be removed when runnning large tests, this is for experiments
        time(5000);
        // Ue no-good recording
        nogoods(true);
        // Record large no-goods, since the trees may be quite deep and we want many restarts
        nogoods_limit(1024);
        // Use 2 assets. Either two normal ones, or one normal and one ahlf-checking
        assets(2);
        // Use the luby sequence for restarts...
        restart(RM_LUBY);
        // ...with the scale set to 660
        restart_scale(660);
        // Luby and the scale 660 is same as in Sequential and Parallel Solution-Biased Search for Subgraph Algorithms.
    }

    void TSPModelOptions::parse(int &argc, char **argv) {
        Options::parse(argc, argv);

        if (tsp_grid_size_.value() > 0) {
            const int size = tsp_grid_size_.value();
            instance(make_grid(size));
            assert(tsp_instance_.value()->locations() > 0);
            assert(!tsp_instance_.value()->lines().empty());
        } else if (std::strcmp(tsp_data_file_.value(), "") == 0) {
            std::cerr << "No data file specified." << std::endl;
            std::exit(EXIT_FAILURE);
        } else {
            const auto &result = TSPInstance::read_instance(tsp_data_file_.value());
            if (result.isErr()) {
                std::cerr << "Could not read file \"" << tsp_data_file_.value() << "\"" << std::endl
                          << "Error: " << result.unwrapErr().text << std::endl;
                std::exit(EXIT_FAILURE);
            }

            const TSPInstance &instance1 = result.unwrap();
            const shared_ptr<const TSPInstance> tsp_instance = std::make_shared<const TSPInstance>(instance1);
            instance(tsp_instance);
        }

        if (use_dominated_edges_propagation_.value()) {
            tsp_instance_.value()->compute_dominated_edges();
        }
    }

    shared_ptr<const TSPInstance> TSPModelOptions::make_grid(const int size) {
        vector<Point> locations;
        locations.reserve(size * size);
        int id = 1;
        for (int i = 0; i < size; ++i) {
            for (int j = 0; j < size; ++j) {
                locations.emplace_back(Point(id, i, j));
                ++id;
            }
        }
        shared_ptr<const TSPInstance> grid = make_shared<const TSPInstance>("Grid", move(locations));
        return grid;
    }

}

#pragma clang diagnostic pop
#pragma clang diagnostic pop
