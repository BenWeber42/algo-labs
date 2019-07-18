#include <iostream>
#include <vector>
#include <limits>
#undef NDEBUG
#include <cassert>

using namespace std;

int main() {
   ios_base::sync_with_stdio(false);

   int tests;
   cin >> tests;
   while (tests--) {
      int n, m, r, b;
      cin >> n >> m >> r >> b;
      --r; --b;

      vector<vector<int>> transitions(n);
      for (int i = 0; i < m; ++i) {
         int u, v;
         cin >> u >> v;
         --u; --v;
         assert(v > u);
         transitions[v].push_back(u);
      }

      vector<int> max_moves(n, numeric_limits<int>::min());
      vector<int> min_moves(n, numeric_limits<int>::max());
      max_moves[n - 1] = min_moves[n - 1] = 0;

      for (int i = n - 1; 0 <= i; --i) {

         assert(max_moves[i] != numeric_limits<int>::min());
         assert(min_moves[i] != numeric_limits<int>::max());
         for (int j: transitions[i]) {
            min_moves[j] = min(min_moves[j], max_moves[i] + 1);
            max_moves[j] = max(max_moves[j], min_moves[i] + 1);
         }
      }

      if (min_moves[r] - (min_moves[r]%2) < min_moves[b]) {
         cout << '0' << endl;
      } else {
         cout << '1' << endl;
      }

   }
}
