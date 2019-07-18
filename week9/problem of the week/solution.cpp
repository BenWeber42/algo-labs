#include <iostream>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/successive_shortest_path_nonnegative_weights.hpp>
#include <boost/graph/push_relabel_max_flow.hpp>
#include <boost/graph/cycle_canceling.hpp>
#include <boost/graph/find_flow_cost.hpp>
#include <boost/tuple/tuple.hpp>


using namespace std;
using namespace boost;

using cint = int; // capacity type
using Traits = adjacency_list_traits<vecS, vecS, directedS>;
using Graph = adjacency_list<vecS, vecS, directedS,
      no_property, property<edge_capacity_t, cint,
                     property<edge_residual_capacity_t, cint,
                        property<edge_reverse_t, Traits::edge_descriptor,
                           property<edge_weight_t, cint>>>>>;



int main() {
   ios_base::sync_with_stdio(false);

   int tests;
   cin >> tests;
   while (tests--) {
      int N, M, S;
      cin >> N >> M >> S;

      Graph graph(
         1 + // source
         S + // states
         M + // sites
         N + // buyers
         1   // sink
      );

      const int source = 0;
      const int states = 1;
      const int sites = 1 + S;
      const int buyers = 1 + S + M;
      const int sink = 1 + S + M + N;

      auto capacities = get(edge_capacity, graph);
      auto costs = get(edge_weight, graph);
      auto reverse = get(edge_reverse, graph);
      auto residuals = get(edge_residual_capacity, graph);

      const auto adder = 
      [&]
      (int from, int to, cint capacity, cint cost) {
         auto e = add_edge(from, to, graph).first;
         auto rev = add_edge(to, from, graph).first;
         capacities[e] = capacity;
         capacities[rev] = 0;
         costs[e] = cost;
         costs[rev] = -cost;
         reverse[e] = rev;
         reverse[rev] = e;
      };

      // states
      for (int i = 0; i < S; ++i) {
         int limit;
         cin >> limit;

         const int state = states + i;
         // source to states
         adder(source, state, limit, 0);
      }

      // sites
      for (int i = 0; i < M; ++i) {
         int state_offset;
         cin >> state_offset;
         --state_offset; // use 0-indexing

         const int site = sites + i;
         const int state = states + state_offset;
         // states to sites
         adder(state, site, 1, 0);
      }

      // bids
      for (int i = 0; i < N; ++i) {
         const int buyer = buyers + i;

         // every buyer is interested in buying at moste one site
         // buyer to sink
         adder(buyer, sink, 1, 0);
         for (int j = 0; j < M; ++j) {
            const int site = sites + j;
            int bid;
            cin >> bid;

            // site to buyer
            adder(site, buyer, 1, 100 - bid);
         }
      }

      successive_shortest_path_nonnegative_weights(graph, source, sink);
      int cost = find_flow_cost(graph);

      int flow = 0;
      for (auto edges = out_edges(vertex(source, graph), graph); edges.first != edges.second; ++edges.first) {
         flow += capacities[*edges.first] - residuals[*edges.first];
      }

      cost = flow*100 - cost;

      cout << flow << " " << cost << endl;

   }
}
