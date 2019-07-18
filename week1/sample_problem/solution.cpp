#include <iostream>
#include <cassert>

using namespace std;

int main() {

   ios_base::sync_with_stdio(false);

   int n, pairs(0), prev_pairs(0);
   cin >> n;

   for (int i(0); i < n; ++i) {

      int x;
      cin >> x;

      if (x == 0) {
         pairs += ++prev_pairs;
      } else if (x == 1) {
         prev_pairs = i - prev_pairs;
         pairs += prev_pairs;
      } else {
         assert(false);
      }
   }

   assert(cin.good());

   cout << pairs << endl;
}
