#include "graph.h"

#include <unordered_set>
#include <map>


using namespace std;

namespace hc {

    bool connected(const LineSegment &edge, int node) {
        return edge.start_id() == node || edge.end_id() == node;
    }


    MST kruskal(int nodes,
                const vector<LineSegment> &mandatory_edges,
                const vector<LineSegment> &edges,
                const function<bool(const LineSegment &)> &filter)
    {
        UnionFind sets(nodes);
        vector<LineSegment> edges_used;
        edges_used.reserve(nodes);
        for (const auto &mandatory_edge : mandatory_edges) {
            sets.join(mandatory_edge.start_id(), mandatory_edge.end_id());
            edges_used.emplace_back(mandatory_edge);
        }

        for (const auto &edge : edges) {
            if (filter(edge)) {
                int node1 = edge.start_id();
                int node2 = edge.end_id();
                if (!sets.same_set(node1, node2)) {
                    sets.join(node1, node2);
                    edges_used.emplace_back(edge);
                    if (sets.set_count() == 1) {
                        break;
                    }
                }
            }
        }

        const MST &result = MST(edges_used);

        return result;
    }


    OneTree kruskal_1_tree(int nodes,
                           int excluded_node,
                           const vector<LineSegment> &mandatory_edges,
                           const vector<LineSegment> &edges,
                           const function<bool(const LineSegment &)> &filter)
    {
        assert(0 <= excluded_node && excluded_node < nodes && "The excluded node must be one of the nodes");

        UnionFind sets(nodes);
        vector<LineSegment> extra_edges;
        extra_edges.reserve(2);
        vector<LineSegment> edges_used;
        edges_used.reserve(nodes);
        for (const auto &mandatory_edge : mandatory_edges) {
            if (connected(mandatory_edge, excluded_node)) {
                extra_edges.emplace_back(mandatory_edge);
            } else {
                sets.join(mandatory_edge.start_id(), mandatory_edge.end_id());
                edges_used.emplace_back(mandatory_edge);
            }
        }

        //for (const auto &edge : edges) {
        int edge_index = 0;
        for (; edge_index < edges.size(); ++edge_index) {
            const auto &edge = edges[edge_index];
            if (filter(edge)) {
                if (connected(edge, excluded_node)) {
                    if (extra_edges.size() < 2) {
                        // Since the edges are iterated over in order, this will
                        extra_edges.emplace_back(edge);
                    }
                } else {
                    int node1 = edge.start_id();
                    int node2 = edge.end_id();
                    if (!sets.same_set(node1, node2)) {
                        sets.join(node1, node2);
                        edges_used.emplace_back(edge);
                        if (sets.set_count() == 2) {
                            break;
                        }
                    }
                }
            }
        }

        for (; extra_edges.size() < 2 && edge_index < edges.size(); ++edge_index) {
            const auto &edge = edges[edge_index];
            if (connected(edge, excluded_node)) {
                if (filter(edge)) {
                    // Since the edges are iterated over in order, this will
                    extra_edges.emplace_back(edge);
                }
            }
        }

        assert(sets.set_size(excluded_node) == 1 && "Excluded node should be excluded in result");

        assert(extra_edges.size() == 2 &&
               "No node should have more than two mandatory edges, no node should have less than 2 possible edges, "
               "so extra node must get exactly two edges.");

        const auto &result = OneTree(nodes, excluded_node, make_pair(extra_edges[0], extra_edges[1]), edges_used);

        return result;
    }


    /**
     *
     * @param nodes Number of total nodes
     * @param odd the nodes to match
     * @param odd_set Set of the nodes to match
     * @param edges Edges to use
     * @return A pair of matched segments, and the unmatched nodes
     */
    pair<vector<LineSegment>, unordered_set<int>> match(int nodes,
                                                        const vector<int> &odd,
                                                        const unordered_set<int> &odd_set,
                                                        const vector<LineSegment> &edges) {
        unordered_set<int> remaining(odd_set);
        vector<LineSegment> matches;

        vector<pair<int, vector<LineSegment>>> grouped;
        grouped.reserve(nodes);
        for (int i = 0; i < nodes; ++i) {
            grouped.emplace_back(make_pair(i, vector<LineSegment>()));
        }
        for (const auto &edge : edges) {
            grouped[edge.start_id()].second.emplace_back(edge);
            grouped[edge.end_id()].second.emplace_back(edge);
        }
        sort(grouped.begin(), grouped.end(), [](const auto &a, const auto &b) {
            return a.second.size() > b.second.size();
        });
        for (int i = 0; i < nodes && !remaining.empty(); ++i) {
            int node_a = grouped[i].first;
            if (remaining.count(node_a) == 1) {
                auto &candidates = grouped[i].second;
                sort(candidates.begin(), candidates.end(), [](const auto &a, const auto &b) {
                    return a.length() < b.length();
                });
                for (const auto &candidate : candidates) {
                    int node_b = candidate.id_not(node_a);
                    if (remaining.count(node_b) == 1) {
                        remaining.erase(node_a);
                        remaining.erase(node_b);
                        matches.emplace_back(candidate);
                        break;
                    }
                }
            }
        }

        return make_pair(move(matches), move(remaining));
    }


    optional<vector<LineSegment>> christofides(
            shared_ptr<const TSPInstance> instance,
            int nodes, 
            const vector<LineSegment> &mandatory_edges, 
            const vector<LineSegment> &edges,
            const function<bool(const LineSegment &)> &filter) 
    {
        MST mst = kruskal(nodes, mandatory_edges, edges, filter);

        vector<int> odd;
        unordered_set<int> odd_set;
        odd.reserve(nodes);
        for (int i = 0; i < nodes; ++i) {
            if ((mst.edges_at(i).size() & 1U) == 1) {
                odd.emplace_back(i);
                odd_set.emplace(i);
            }
        }
        assert((odd.size() & 1U) == 0);

        vector<LineSegment> candidate_edges;
        candidate_edges.reserve(odd.size() * odd.size());
        for (const auto &edge : edges) {
            if (edge.start_id() != edge.end_id())
            {
                if (odd_set.count(edge.start_id()) == 1 && odd_set.count(edge.end_id()) == 1) {
                    candidate_edges.emplace_back(edge);
                }
            }
        }
        pair<vector<LineSegment>, unordered_set<int>> match_and_remaining = match(nodes, odd, odd_set,
                                                                     candidate_edges);
        auto& matches = match_and_remaining.first;
        auto& remaining = match_and_remaining.second;

        if (!remaining.empty()) {
            vector<LineSegment> extra_lines;
            for (int n1 : remaining) {
                for (int n2 : remaining) {
                    if (n1 < n2) {
                        extra_lines.emplace_back(instance->line(n1, n2));
                    }
                }
            }
            sort(extra_lines.begin(), extra_lines.end(), [](const auto& a, const auto& b){
                return a.length() < b.length();
            });
            int pos = 0;
            while (pos < extra_lines.size() && remaining.size() > 1) {
                const auto& line = extra_lines[pos];
                if (remaining.count(line.start_id()) == 1 &&
                    remaining.count(line.end_id()) == 1) {
                    matches.emplace_back(line);
                    remaining.erase(line.start_id());
                    remaining.erase(line.end_id());
                }
                ++pos;
            }
            if (!remaining.empty()) {
                std::cerr << "Some nodes not matched at all..." << std::endl;
                return optional<vector<LineSegment>>();
            }
            //matches.insert(matches.end(), extra_lines.begin(), extra_lines.end());
//            if (matches.size() +1 != nodes) {
//                std::cerr << "Wrong number of edges" << std::endl;
//                return optional<vector<LineSegment>>();
//            }
        }

        vector<LineSegment> christofides_edges;
        christofides_edges.reserve(mst.edges().size() + matches.size());
        christofides_edges.insert(christofides_edges.end(), mst.edges().begin(), mst.edges().end());
        christofides_edges.insert(christofides_edges.end(), matches.begin(), matches.end());

        vector<int> euler_circuit = hierholzer_path(nodes, christofides_edges);
        vector<bool> visited(nodes, false);
        int last = euler_circuit[0];
        visited[last] = true;
        vector<LineSegment> circuit;
        circuit.reserve(nodes+1);
        for (int i = 1; i < euler_circuit.size(); ++i) {
            int next = euler_circuit[i];
            if (!visited[next]) {
                visited[next] = true;
                circuit.emplace_back(instance->line(last, next));
                last = next;
            }
        }
        if (circuit[0].start_id() != circuit[circuit.size() - 1].end_id()) {
            circuit.emplace_back(instance->line(circuit[circuit.size() - 1].end_id(), circuit[0].start_id()));
        }

        return optional(circuit);
    }

    /*
stack St;
put start vertex in St;
until St is empty
  let V be the value at the top of St;
  if degree(V) = 0, then
    add V to the answer;
    remove V from the top of St;
  otherwise
    find any edge coming out of V;
    remove it from the graph;
    put the second end of this edge in St;
     */
    vector<int> hierholzer_path(int nodes, const vector<LineSegment>& edges) {
        vector<vector<LineSegment>> graph(nodes, vector<LineSegment>());
        typedef map<pair<int, int>, LineSegment> emap;
        emap edge_map;
        for (const auto &edge : edges) {
            int n1 = edge.start_id();
            int n2 = edge.end_id();
            graph[n1].emplace_back(edge);
            graph[n2].emplace_back(edge);
            edge_map.insert(emap::value_type(make_pair(min(n1, n2), max(n1, n2)), edge));
        }
        vector<int> result;
        vector<int> stack;
        stack.emplace_back(0);
        while (!stack.empty()) {
            int top = *stack.rbegin();
            if (graph[top].empty()) {
                result.emplace_back(top);
                stack.pop_back();
            } else {
                auto &edge = *graph[top].rbegin();
                graph[top].pop_back();
                int next = edge.id_not(top);
                vector<LineSegment> &next_edges = graph[next];
                next_edges.erase(find(next_edges.begin(), next_edges.end(), edge));
                stack.emplace_back(next);
            }
        }

        return result;

//        vector<LineSegment> result_edges;
//        for (int i = 0; i < result.size(); ++i) {
//            int n1 = result[i];
//            int n2 = result[(i+1) % result.size()];
//            auto key = make_pair(min(n1, n2), max(n1, n2));
//            result_edges.emplace_back(edge_map.find(key)->second);
//        }
//
//        return result_edges;
    }


}