#include <iostream>
#include <vector>
#undef NDEBUG
#include <cassert>

using namespace std;

int distance(int lights, int pattern, int k) {
   // could use some bit fiddeling here, but this should suffice
   int diff = 0;
   int xored = pattern ^ lights;
   for (int i = 0; i < k; ++i) {
      if (xored & 0x1)
         ++diff;
      xored = xored >> 1;
   }

   assert(0 <= diff && diff <= k);
   return diff;
}

vector<int> toggled_mem, untoggled_mem, toggled_distance, untoggled_distance, lights;

int sequence_distance(int t, bool toggled) {

   if (t < 0)
      return 0;

   if (toggled) {
      if (toggled_mem[t] != -1) {
         return toggled_mem[t];
      }
   } else {
      if (untoggled_mem[t] != -1) {
         return untoggled_mem[t];
      }
   }

   int distance;
   if (toggled) {
      distance = min(
         toggled_distance[t] + sequence_distance(t - 1, toggled),

         1 + untoggled_distance[t] + sequence_distance(t - 1, !toggled)
      );
   } else {
      distance = min(
         untoggled_distance[t] + sequence_distance(t - 1,toggled),

         1 + toggled_distance[t] + sequence_distance(t - 1, !toggled)
      );
   }

   if (toggled) {
      return toggled_mem[t] = distance;
   } else {
      return untoggled_mem[t] = distance;
   }
}

int main() {
   ios_base::sync_with_stdio(false);

   int tests;
   cin >> tests;

   while (0 < tests--) {

      int n, k, pattern, l;
      cin >> n >> k >> pattern;
      l = n/k;

      lights = vector<int>(l);
      for (int i = 0; i < l; ++i) {
         int x = 0;
         for (int j = 0; j < k; ++j) {
            int b;
            cin >> b;
            x = (x << 1) + b;
         }
         lights[i] = x;
      }

      toggled_distance = vector<int>(l);
      untoggled_distance = vector<int>(l);
      for (int i = 0; i < l; ++i) {
         untoggled_distance[i] = distance(lights[i], pattern, k);
         toggled_distance[i] = distance(~lights[i], pattern, k);
      }

      toggled_mem = vector<int>(l, -1);
      untoggled_mem = vector<int>(l, -1);

      cout << sequence_distance(l - 1, false) << endl;
   }
}
