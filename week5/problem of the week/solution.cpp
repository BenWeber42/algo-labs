#include <iostream>
#include <boost/graph/adjacency_list.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>

using namespace std;
using namespace boost;

using Graph = adjacency_list
   <setS, vecS, undirectedS,
   no_property, property<edge_weight_t, int>>;

using Edge = graph_traits<Graph>::edge_descriptor;
using EdgeIt = graph_traits<Graph>::edge_iterator;
using EdgeWeightMap = property_map<Graph, edge_weight_t>::type;

int main() {
   ios_base::sync_with_stdio(false);

   int tests;
   cin >> tests;
   while (tests--) {
      int n, m, k, source, target;
      cin >> n >> m >> k >> source >> target;

      ++k;
      Graph graph(n*k);
      EdgeWeightMap weights = get(edge_weight, graph);

      for (int i = 0; i < m; ++i) {
         int a, b, c, d;
         cin >> a >> b >> c >> d;

         Edge e;
         bool unconnected;
         for (int j = 0; j < k; ++j) {
            tie(e, unconnected) = add_edge(a + j*n, b + j*n, graph);

            if (unconnected) {
               weights[e] = c;
            } else {
               weights[e] = min(weights[e], c);
            }
         }

         for (int j = 1; d == 1 && j < k; ++j) {
            // river edge
            tie(e, unconnected) = add_edge(a + (j - 1)*n, b + j*n, graph);

            if (unconnected) {
               weights[e] = c;
            } else {
               weights[e] = min(weights[e], c);
            }

            tie(e, unconnected) = add_edge(a + j*n, b + (j - 1)*n, graph);

            if (unconnected) {
               weights[e] = c;
            } else {
               weights[e] = min(weights[e], c);
            }
         }
      }

      vector<int> distances(n*k);
      dijkstra_shortest_paths(graph, source,
         distance_map(make_iterator_property_map(distances.begin(),
            get(vertex_index, graph))));

      cout << distances[(k - 1)*n + target] << endl;
   }
}
