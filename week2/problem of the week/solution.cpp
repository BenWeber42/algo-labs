#include <vector>
#include <iostream>
#include <cstdlib>

void assert(bool cond) {
   if (!cond)
      exit(EXIT_FAILURE);
}

using namespace std;

class Node {
public:
   Node() : distance(-1) {}
   int distance;
   // edges
   vector<int> backward, forward;
};

int shortest(const vector<Node>& graph, int current, int target, int diff) {

   if (current == target) {
      // at last check if there's an outgoing edge from the target
      // that could be part of the second shortest path
      for (int next: graph[target].forward) {

         int distance = graph[next].distance;
         if (distance == -1) {
            // doesn't reach target!
            continue;
         }

         if (diff == -1) {
            diff = distance + 1;
         } else {
            diff = min(diff, distance + 1);
         }
      }
      return diff;
   }

   assert(graph[current].forward.size() >= 1);

   // find the edge with the minimal distance
   int min = -1, min_id = -1;
   for (int next: graph[current].forward) {

      int distance = graph[next].distance;
      if (distance == -1) {
         // doesn't reach target!
         continue;
      }

      if (min == -1) {
         min = distance;
         min_id = next;
      } else if (distance < min) {
         min = distance;
         min_id = next;
      }
   }

   // find the edge with the second smallest distance
   int min2 = -1;
   for (int next: graph[current].forward) {

      if (next == min_id) {
         continue;
      }

      int distance = graph[next].distance;
      if (distance == -1) {
         // doesn't reach target!
         continue;
      }

      if (min2 == -1) {
         min2 = distance;
      } else if (distance < min2) {
         min2 = distance;
      }
   }

   assert(min != -1);
   assert(min + 1 == graph[current].distance);

   if (min2 != -1) {
      assert(min2 >= min);
      if (diff == -1) {
         diff = min2 - min;
      } else if (min2 - min < diff) {
         diff = min2 - min;
      }
   }

   return shortest(graph, min_id, target, diff);
}

int main() {

   ios_base::sync_with_stdio(false);

   int tests;
   cin >> tests;
   for (int test = 0; test < tests; ++test) {
      int n, m, s, t;
      cin >> n >> m >> s >> t;

      --s; --t; // s == t is also possible!

      vector<Node> graph(n);

      for (int i = 0; i < m; ++i) {
         int a, b;
         cin >> a >> b;

         --a; --b; // a == b is also possible!
         graph[a].forward.push_back(b);
         graph[b].backward.push_back(a);
      }

      // first do backwards bsf to get distances & reachability
      vector<int> frontier, next_frontier;
      frontier.push_back(t);
      int distance = graph[t].distance = 0;

      while (!frontier.empty()) {
         ++distance;
         next_frontier.clear();
         for (int current: frontier) {
            for (int next: graph[current].backward) {
               if (graph[next].distance == -1) {
                  graph[next].distance = distance;
                  next_frontier.push_back(next);
               }
            }
         }
         swap(frontier, next_frontier);
      }

      frontier.clear();
      next_frontier.clear();

      if (graph[s].distance == -1) {
         // source and target aren't connected!
         cout << "no" << endl;
         continue;
      }

      // follow the shortest path and
      // along the way check if there's another path
      // and determine the minimal distance of that path
      int diff = shortest(graph, s, t, -1);

      if (diff == -1) {
         // there is no second shortest path!
         cout << "no" << endl;
         continue;
      }

      cout << (graph[s].distance + diff) << endl;
   }
}
