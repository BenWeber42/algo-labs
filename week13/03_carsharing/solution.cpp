#include <vector>
#include <map>
#include <unordered_map>
#include <iostream>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/successive_shortest_path_nonnegative_weights.hpp>
#undef NDEBUG
#include <cassert>

using namespace boost;
using namespace std;

using cint = int; // capacity type

using Traits = adjacency_list_traits<vecS, vecS, directedS>;
using Graph = adjacency_list<vecS, vecS, directedS, no_property,
               property<edge_capacity_t, cint,
                  property<edge_residual_capacity_t, cint,
                     property<edge_reverse_t, Traits::edge_descriptor,
                        property<edge_weight_t, cint>>>>>;

using Edge = graph_traits<Graph>::edge_descriptor;

struct Booking {
   // start-/end station
   int start_s, end_s;
   // start-/end time
   int start_t, end_t;
   int profit;
   int duration() const {
      return end_t - start_t;
   }
};

int main() {
   ios_base::sync_with_stdio(false);

   int tests;
   cin >> tests;
   while (tests--) {
      int N, S;
      cin >> N >> S;

      int cars = 0;
      vector<int> station(S);
      for (int i = 0; i < S; ++i) {
         cin >> station[i];
         cars += station[i];
      }

      int id = 0;
      vector<std::map<int, int>> time_points(S);
      vector<int> time_points_min(S, numeric_limits<int>::max());
      vector<int> time_points_max(S, numeric_limits<int>::min());
      vector<Booking> bookings(N);
      int min_time = numeric_limits<int>::max();
      int max_time = numeric_limits<int>::min();
      for (int i = 0; i < N; ++i) {
         int s, t, d, a, p;
         cin >> s >> t >> d >> a >> p;
         --s; --t; // use 0-indexing
         bookings[i] = {
            .start_s = s,
            .end_s = t,
            .start_t = d,
            .end_t = a,
            .profit = p
            };
         
         auto& start_s = time_points[s];
         auto& end_s = time_points[t];

         min_time = min(min_time, d);
         max_time = max(max_time, a);

         if (start_s.find(d) == start_s.end()) {
            start_s[d] = id++;
            time_points_min[s] = min(time_points_min[s], d);
            time_points_max[s] = max(time_points_max[s], d);
         }

         if (end_s.find(a) == end_s.end()) {
            end_s[a] = id++;
            time_points_min[t] = min(time_points_min[t], a);
            time_points_max[t] = max(time_points_max[t], a);
         }
      }

      const int source = id++;
      const int sink = id++;

      Graph graph(id);

      auto capacities = get(edge_capacity, graph);
      auto costs = get(edge_weight, graph);
      auto reverse = get(edge_reverse, graph);

      auto adder = [&](int from, int to, int capacity, int cost) {
         auto edge = add_edge(from, to, graph).first;
         auto rev = add_edge(to, from, graph).first;

         assert(cost >= 0);

         capacities[edge] = capacity;
         capacities[rev] = 0;

         costs[edge] = cost;
         costs[rev] = -cost;

         reverse[edge] = rev;
         reverse[rev] = edge;

         return edge;
      };

      for (int i = 0; i < S; ++i) {

         if (time_points[i].size() == 0) {
            continue; // no booking involve this station
         }

         // source to stations
         if (station[i] != 0) {
            adder(
               source,
               time_points[i][time_points_min[i]],
               station[i],
               100*(time_points_min[i] - min_time)
               );
         }

         // for each station connect each time point
         for (auto prev = time_points[i].cbegin(), cur = ++time_points[i].cbegin();
               cur != time_points[i].cend();
               ++prev, ++cur) {

            // every car costs us 100 per minute in a station
            adder(prev->second, cur->second, cars, 100*(cur->first - prev->first));
         }

         // stations to sink
         adder(
            time_points[i][time_points_max[i]],
            sink,
            cars,
            100*(max_time - time_points_max[i])
            );
      }

      
      boost::unordered_map<Edge, int> profits;
      // connect bookings
      for (const auto& booking: bookings) {
         auto edge = adder(
            time_points[booking.start_s][booking.start_t],
            time_points[booking.end_s][booking.end_t],
            1,
            100*booking.duration() - booking.profit
            );

         profits[edge] = booking.profit;
      }

      successive_shortest_path_nonnegative_weights(graph, source, sink);

      auto residuals = get(edge_residual_capacity, graph);
      auto es = edges(graph);
      auto eit = es.first;
      auto eend = es.second;
      int profit = 0;
      for (; eit != eend; ++eit) {
         if (profits.find(*eit) != profits.end() && residuals[*eit] == 0) {
            profit += profits[*eit];
         }
      }

      cout << profit << endl;
   }
}
