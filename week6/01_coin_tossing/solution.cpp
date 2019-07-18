#include <iostream>
#include <boost/tuple/tuple.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/edmonds_karp_max_flow.hpp>
#undef NDEBUG
#include <cassert>

using namespace std;
using namespace boost;

using cint = long;
using Traits = adjacency_list_traits<vecS, vecS, directedS>;
using Graph = adjacency_list<
   vecS, vecS, directedS,
   no_property, property<edge_capacity_t, cint,
      property<edge_residual_capacity_t, cint,
         property<edge_reverse_t, Traits::edge_descriptor>>>>;

using Edge = graph_traits<Graph>::edge_descriptor;
using EdgeIt = graph_traits<Graph>::edge_iterator;
using CapacityMap = property_map<Graph, edge_capacity_t>::type;
using ResidualCapacityMap = property_map<Graph, edge_residual_capacity_t>::type;
using ReverseEdgeMap = property_map<Graph, edge_reverse_t>::type;

int main() {
   ios_base::sync_with_stdio(false);

   int tests;
   cin >> tests;
   while (tests--) {
      int n, m;
      cin >> n >> m;

      // offsets/ids
      const int source = 0;
      const int matches = 1;
      const int players = 1 + m;
      const int sink = 1 + m + n;

      Graph graph(
         1 + // source
         m + // matches
         n + // players
         1   // sink
      );

      CapacityMap capacities = get(edge_capacity, graph);
      //ResidualCapacityMap residual_capacities =
         //get(edge_residual_capacity, graph);
      ReverseEdgeMap reverse_map = get(edge_reverse, graph);

      auto edgeAdder =
      [&graph, &capacities, &reverse_map]
      (int from, int to, cint capacity)
      {
         Edge e, rev;
         tie(e, tuples::ignore) = add_edge(from, to, graph);
         tie(rev, tuples::ignore) = add_edge(to, from, graph);

         capacities[e] = capacity;
         capacities[rev] = 0;
         reverse_map[e] = rev;
         reverse_map[rev] = e;
      };

      vector<cint> totals(n);
      for (int i = 0; i < m; ++i) {
         int a, b;
         cint c;
         cin >> a >> b >> c;

         if (c == 0) {
            // unknown
            const int match_id = matches + i;
            const int a_id = players + a;
            const int b_id = players + b;

            // source to match
            edgeAdder(source, match_id, 1);

            // match to playes (a_id, b_id)
            edgeAdder(match_id, a_id, 1);
            edgeAdder(match_id, b_id, 1);
         } else if (c == 1) {
            // player a won
            totals[a] += 1;
         } else if (c == 2) {
            // player b won
            totals[b] += 1;
         } else {
            assert(false);
         }
      }

      cint total = 0;
      cint flow = 0;
      bool negative_flow = false;
      for (int i = 0; i < n; ++i) {
         cint score;
         cin >> score;

         if (negative_flow) {
            continue;
         }

         total += score;
         score -= totals[i];
         flow += score;

         if (score < 0) {
            negative_flow = true;
         }

         const int p_id = players + i;

         // each player got at most this score
         edgeAdder(p_id, sink, score);
      }

      if (negative_flow || total != m) {
         cout << "no" << endl;
         continue;
      }

      // max flow must equal total, otherwise it's not possible
      cint max_flow = edmonds_karp_max_flow(graph, source, sink);

      if (max_flow == flow) {
         cout << "yes" << endl;
      } else {
         cout << "no" << endl;
      }
   }
}
