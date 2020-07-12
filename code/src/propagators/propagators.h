#ifndef HC_PROPAGATORS_LIB_H
#define HC_PROPAGATORS_LIB_H

#include <gecode/driver.hh>
#include <gecode/int.hh>

#include <memory>
#include <vector>
#include <cassert>
#include <optional>
#include <utilities/tsp.h>

namespace hc {

    /**
     * Remove all edges that are dominated by another assigned edge.
     *
     * @param home Space to post in
     * @param instance The TSP instance to remove dominated edges
     * @param successors The variables representing the circuit
     */
    void no_dominated_edge_pairs(Gecode::Home home, std::shared_ptr<const hc::TSPInstance> instance, const Gecode::IntVarArgs& successors);

    /**
     * Following the "Warnsdorff path" (i.e., the assgined path form a specified start-node), remove all edges from
     * the final node that crosses another assigned edge.
     *
     * @param home Space to post in
     * @param instance The TSP instance describing the problem
     * @param start_node The node to start the path from
     * @param successors The variables representing the circuit
     */
    void no_warnsdorff_dominated_edges(Gecode::Home home, std::shared_ptr<const hc::TSPInstance> instance, int start_node, const Gecode::IntVarArgs& successors);

    /**
     * Following the "Warnsdorff path" (i.e., the assgined path form a specified start-node), remove all edges from
     * the final node that crosses another assigned edge.
     *
     * @param home Space to post in
     * @param instance The TSP instance describing the problem
     * @param start_node The node to start the path from
     * @param successors The variables representing the circuit
     */
    void no_warnsdorff_dominated_edges2(Gecode::Home home, std::shared_ptr<const hc::TSPInstance> instance, int start_node, const Gecode::IntVarArgs& successors);

    void hk_1tree(Gecode::Home home, std::shared_ptr<const TSPInstance> instance, const Gecode::IntVarArgs& successors, const Gecode::IntVarArgs& predeccesors, const Gecode::IntVar cost);

    void christofides(Gecode::Home home, std::shared_ptr<const TSPInstance> instance, const Gecode::IntVarArgs& successors, const Gecode::IntVar cost);
}

#endif //HC_PROPAGATORS_LIB_H
