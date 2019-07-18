#include <cassert>
#include <iostream>
#include <utility>
#include <algorithm>
#include <vector>

using namespace std;

int main() {

   ios_base::sync_with_stdio(false);

   int tests;
   cin >> tests;

   for (int test = 0; test < tests; ++test) {
      int n, m;

      cin >> n >> m;

      vector<pair<int, int>> intervals;

      for (int i = 0; i < n; ++i) {
         int p, q;
         cin >> p >> q;

         if (p != 0) {
            assert(q != 0);
            assert(p <= q);

            intervals.push_back(make_pair(p, q));

         } else {
            assert(q == 0);
         }

      }

      // check if there are unbeaten humans:
      sort(intervals.begin(), intervals.end(),
         [] (const pair<int, int>& a, const pair<int, int>& b) {
         return a.first < b.first || (a.first == b.first && a.second > b.second);
      });

      int beaten = 0;

      for (const auto& interval: intervals) {
         if (beaten + 1 < interval.first) {
            break;
         }
         beaten = max(beaten, interval.second);
      }

      if (beaten < m) {
         // there are some unbeaten humans
         cout << 0 << endl;
         continue;
      }

      // check which aliens beat all other aliens
      int count = 0, max_bound = 0;
      pair<int, int> last = make_pair(0, 0);
      for (const auto& interval: intervals) {

         if (interval.second > max_bound) {
            ++count;
            max_bound = interval.second;
            last = interval;
            continue;
         }

         if (interval.second < max_bound) {
            continue;
         }

         assert(interval.second == max_bound);

         if (last == interval) {
            // decrease count due to duplicate
            --count;
            // reset last, in case of tripple etc
            last = make_pair(0, 0);
         }
      }

      cout << count << endl;
   }
}
