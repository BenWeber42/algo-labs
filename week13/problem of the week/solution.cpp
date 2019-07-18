#include <algorithm>
#include <vector>
#include <iostream>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/push_relabel_max_flow.hpp>
#undef NDEBUG
#include <cassert>

using namespace std;
using namespace boost;

using cint = int; // capacity type

using Traits = adjacency_list_traits<vecS, vecS, directedS>;
using Graph = adjacency_list<vecS, vecS, directedS, no_property,
               property<edge_capacity_t, cint,
                  property<edge_residual_capacity_t, cint,
                     property<edge_reverse_t, Traits::edge_descriptor>>>>;


struct Brick {
   int start, end;
   Brick() = default;
   Brick(int start, int end) : start(start), end(end) {}
   bool operator<(const Brick& other) const {
      return start < other.start ||
            (start == other.start && end < other.end);
   }
};

int main() {
   ios_base::sync_with_stdio(false);

   int tests;
   cin >> tests;
   while (tests--) {
      int w, n;
      cin >> w >> n;

      Graph graph(
         2*(w + 1) + // all possible boundaries (with in and out edges)
         n           // each brick
      );

      const int in_coordinates = 0;          // offset for in coordinates
      const int out_coordinates = w + 1;     // offset for out coordinates
      const int bricks = 2*(w + 1);          // bricks offset

      auto capacities = get(edge_capacity, graph);
      auto reverse = get(edge_reverse, graph);

      auto adder = [&] (int from, int to, cint capacity) {
         auto edge = add_edge(from, to, graph).first;
         auto rev = add_edge(to, from, graph).first;

         capacities[edge] = capacity;
         capacities[rev] = 0;
         reverse[edge] = rev;
         reverse[rev] = edge;
      };

      for (int i = 0; i < n; ++i) {
         int start, end;
         cin >> start >> end;

         if (start == end) {
            continue; // ignore
         }

         if (end < start) {
            swap(end, start);
         }
         assert (0 <= start && start < end && end <= w);

         const int start_out = out_coordinates + start;
         const int end_in = in_coordinates + end;
         const int brick_id = bricks + i;

         // left coordinate node to brick
         adder(start_out, brick_id, 1);
         adder(brick_id, end_in, 1);
      }

      // 0 coordinate can occur several times
      adder(in_coordinates + 0, out_coordinates + 0, n);
      // w coordinate can occur several times
      adder(in_coordinates + w, out_coordinates + w, n);

      for (int i = 1; i < w; ++i) {
         // all other coordinates can occure exactly once
         adder(in_coordinates + i, out_coordinates + i, 1);
      }

      cint flow = push_relabel_max_flow(graph, in_coordinates + 0, out_coordinates + w);

      cout << flow << endl;
   }
}
