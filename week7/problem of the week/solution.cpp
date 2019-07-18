#include <iostream>
#include <boost/graph/adjacency_list.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/graph/push_relabel_max_flow.hpp>
#undef NDEBUG
#include <cassert>

using namespace std;
using namespace boost;

using cint = int;

// might have double edges, but that's ok
using Traits = adjacency_list_traits<vecS, vecS, directedS>;
using Graph = adjacency_list<
         vecS, vecS, directedS,
         no_property, property<edge_capacity_t, cint,
                        property<edge_residual_capacity_t, cint,
                           property<edge_reverse_t, Traits::edge_descriptor>>>>;

using Edge = graph_traits<Graph>::edge_descriptor;
using EdgeIt = graph_traits<Graph>::edge_iterator;

using EdgeCapacityMap = property_map<Graph, edge_capacity_t>::type;
using ResidualCapacityMap = property_map<Graph, edge_residual_capacity_t>::type;
using ReverseEdgeMap = property_map<Graph, edge_reverse_t>::type;
               
int main() {
   ios_base::sync_with_stdio(false);

   int tests;
   cin >> tests;
   while (tests--) {
      int n, m, s, d;
      cin >> n >> m >> s >> d;

      // each vertex twice + source + sink
      Graph graph(2*n + 2);
      EdgeCapacityMap capacities = get(edge_capacity, graph);
      ReverseEdgeMap reverse = get(edge_reverse, graph);

      const auto edgeAdder =
      [&graph, &capacities, &reverse]
      (int from, int to, cint capacity)
      {
         Edge e, rev;
         tie(e, tuples::ignore) = add_edge(from, to, graph);
         tie(rev, tuples::ignore) = add_edge(to, from, graph);

         assert(capacities[e] == 0);
         assert(capacities[rev] == 0);

         capacities[e] = capacity;
         capacities[rev] = 0;
         reverse[e] = rev;
         reverse[rev] = e;
      };

      for (int i = 0; i < n; ++i) {
         // in vertex = i, out vertex = n + i
         edgeAdder(i, n + i, 1);
      }

      for (int i = 0; i < m; ++i) {
         int from, to;
         cin >> from >> to;
         from = n + from; // out vertex
         edgeAdder(from, to, 1);
      }

      // sources
      for (int i = 0; i < s; ++i) {
         int source;
         cin >> source; // in vertex unchanged
         edgeAdder(2*n, source, m);
      }

      // sinks
      for (int i = 0; i < d; ++i) {
         int sink;
         cin >> sink;
         sink = n + sink; // out vertex
         edgeAdder(sink, 2*n + 1, m);
      }

      cint max_flow = push_relabel_max_flow(graph, 2*n, 2*n + 1);

      cout << max_flow << endl;
   }
}
