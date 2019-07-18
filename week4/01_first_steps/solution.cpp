#include <algorithm>
#include <numeric>
#include <iostream>
#include <boost/tuple/tuple.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/kruskal_min_spanning_tree.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>

using namespace std;
using namespace boost;

using Graph = adjacency_list<vecS, vecS, undirectedS, no_property,
   property<edge_weight_t, int>>;

using Edge = graph_traits<Graph>::edge_descriptor;
using EdgeIt = graph_traits<Graph>::edge_iterator;

using EdgeWeightMap = property_map<Graph, edge_weight_t>::type;

int main() {
   ios_base::sync_with_stdio(false);

   int tests;
   cin >> tests;
   while (0 < tests--) {
      int V, E;
      cin >> V >> E;

      Graph graph(V);
      EdgeWeightMap weights = get(edge_weight, graph);

      for (int i = 0; i < E; ++i) {
         int a, b, w;
         cin >> a >> b >> w;
         Edge e;
         tie(e, tuples::ignore) = add_edge(a, b, graph);
         weights[e] = w;
      }

      vector<Edge> mst;
      kruskal_minimum_spanning_tree(graph, back_inserter(mst));

      vector<int> dist(V);
      dijkstra_shortest_paths(graph, 0,
         weight_map(weights).
            distance_map(make_iterator_property_map(dist.begin(),
               get(vertex_index, graph))));

      int mst_weight = accumulate(mst.begin(), mst.end(), 0,
         [&weights](int sum, const Edge& edge) {
            return sum + weights[edge];
         }
      );

      int max_distance = *max_element(dist.begin(), dist.end());

      cout << mst_weight << ' ' << max_distance << endl;
   }
}
