#pragma clang diagnostic push
#pragma ide diagnostic ignored "cppcoreguidelines-pro-type-static-cast-downcast"
#ifndef HC_TSP_COMMON_MODEL_H
#define HC_TSP_COMMON_MODEL_H

#include <cstring>
#include <cstdlib>
#include <memory>
#include <optional>

#include <gecode/driver.hh>
#include <gecode/int.hh>

#include "utilities/tsp.h"

namespace hc {
    enum class VarBranching {
        InputOrder,
        AfcSizeMin,
        Warnsdorff,
        CostRegret,
    };

    enum class ValBranching {
        First,
        Random,
        MinLength,
        MaxLength,
        MinDegreeMinLength,
        MinDegreeMaxLength,
    };

    class TSPModelOptions : public Gecode::Options {
        Gecode::Driver::StringOption branching_val_;
        Gecode::Driver::IntOption tsp_grid_size_;
        Gecode::Driver::StringValueOption tsp_data_file_;
        Gecode::Driver::BoolOption use_dominated_edges_propagation_;
        Gecode::Driver::BoolOption use_warnsdorff_dominated_edges_propagation_;
        Gecode::Driver::BoolOption use_warnsdorff_dominated_edges2_propagation_;
        Gecode::Driver::BoolOption use_one_tree_propagation_;
        Gecode::Driver::BoolOption use_christofides_propagation_;
        Gecode::Driver::BoolOption use_all_nogoods_;
        std::optional<const std::shared_ptr<const TSPInstance>> tsp_instance_;
    public:
        TSPModelOptions();

        void parse(int& argc, char* argv[]);

        [[nodiscard]] VarBranching branching_var() const {
            return static_cast<VarBranching>(branching());
        }

        [[nodiscard]] ValBranching branching_val() const {
            return static_cast<ValBranching>(branching_val_.value());
        }

        [[nodiscard]] bool use_dominated_edges_propagation() const {
            return use_dominated_edges_propagation_.value();
        }

        [[nodiscard]] bool use_warnsdorff_dominated_edges_propagation() const {
            return use_warnsdorff_dominated_edges_propagation_.value();
        }

        [[nodiscard]] bool use_warnsdorff_dominated_edges2_propagation() const {
            return use_warnsdorff_dominated_edges2_propagation_.value();
        }

        [[nodiscard]] bool use_one_tree_propagation() const {
            return use_one_tree_propagation_.value();
        }

        [[nodiscard]] bool use_christofides_propagation() const {
            return use_christofides_propagation_.value();
        }

        [[nodiscard]] bool use_all_nogoods() const {
            return use_all_nogoods_.value();
        }

        [[nodiscard]] std::shared_ptr<const TSPInstance> instance() const {
            return tsp_instance_.value();
        }

        void instance(const std::shared_ptr<const TSPInstance> tsp_instance) { tsp_instance_.emplace(tsp_instance); }

        static std::shared_ptr<const TSPInstance> make_grid(const int size) ;
    };
}

#endif //HC_TSP_COMMON_MODEL_H

#pragma clang diagnostic pop
