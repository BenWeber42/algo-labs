#include <iostream>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/successive_shortest_path_nonnegative_weights.hpp>
#include <boost/graph/find_flow_cost.hpp>

using namespace boost;
using namespace std;

using cint = int; // capacity type

using Traits = adjacency_list_traits<vecS, vecS, directedS>;
using Graph = adjacency_list<vecS, vecS, directedS, no_property,
   property<edge_capacity_t, cint,
      property<edge_residual_capacity_t, cint,
         property<edge_weight_t, cint,
            property<edge_reverse_t, Traits::edge_descriptor>>>>>;

int main() {
   ios_base::sync_with_stdio(false);

   int tests;
   cin >> tests;
   while (tests--) {
      int n;
      cin >> n;

      Graph graph(2*n*n); // for each field an in vertex and an out vertex
      auto capacities = get(edge_capacity, graph);
      auto weights = get(edge_weight, graph);
      auto reverse = get(edge_reverse, graph);

      auto adder = [&] (int from, int to, cint capacity, cint weight) {
         auto e = add_edge(from, to, graph).first;
         auto rev = add_edge(to, from, graph).first;
         
         capacities[e] = capacity;
         capacities[rev] = 0;

         weights[e] = weight;
         weights[rev] = -weight;

         reverse[e] = rev;
         reverse[rev] = e;
      };

      const int out = n*n;
      for (int x = 0; x < n; ++x) {
         for (int y = 0; y < n; ++y) {
            int a;
            cin >> a;

            const int id = x*n + y;
            
            // one way he can collect a coins
            adder(id, out + id, 1, 100 - a);
            // the other way there are no coins left
            adder(id, out + id, 1, 100);

            if (0 < x) {
               // connect to left shelf
               const int left_id = (x - 1)*n + y;
               adder(out + left_id, id, 2, 100);
            }
            if (0 < y) {
               // connect to upper shelf
               const int upper_id = x*n + y - 1;
               adder(out + upper_id, id, 2, 100);
            }
         }
      }

      successive_shortest_path_nonnegative_weights(graph, 0, 2*n*n - 1);

      cint coins = 0;
      auto residuals = get(edge_residual_capacity, graph);
      auto pair_edges = edges(graph);
      auto edge_it = pair_edges.first;
      auto end = pair_edges.second;

      for (; edge_it != end; ++edge_it) {
         const auto edge = *edge_it;

         if (capacities[edge] == 1 && residuals[edge] == 0) {
            coins += 100 - weights[edge];
         }
      }

      cout << coins << endl;
   }
}
