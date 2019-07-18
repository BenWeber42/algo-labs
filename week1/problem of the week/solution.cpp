#include <iostream>
#include <vector>
#include <unordered_set>
#include <cassert>

using namespace std;

template<typename T>
using set = unordered_set<T>;

set<int> set_intersection(set<int>& a, set<int>& b) {

   if (a.size() > b.size())
      swap(a, b);

   set<int> result;

   for (set<int>::const_iterator it = a.begin(); it != a.end(); ++it)
      if (b.find(*it) != b.end())
         result.insert(*it);

   return result;
}

int main() {

   ios_base::sync_with_stdio(false);

   int t;
   cin >> t;

   for (int test = 0; test < t; ++test) {
      int n, k;

      cin >> n;
      cin >> k;

      set<int> heavier;
      set<int> lighter;

      for (int i = 1; i <= n; ++i) {
         heavier.insert(i);
         lighter.insert(i);
      }

      for (int weighting = 0; weighting < k; ++weighting) {
         int P;
         cin >> P;

         set<int> left;
         set<int> right;

         for (int i = 0; i < P; ++i) {
            int x;
            cin >> x;
            left.insert(x);
         }

         for (int i = 0; i < P; ++i) {
            int x;
            cin >> x;
            right.insert(x);
         }

         char result;

         cin >> result;

         if (result == '=') {
            for (set<int>::iterator it = left.begin(); it != left.end(); ++it) {
               heavier.erase(*it);
               lighter.erase(*it);
            }
            for (set<int>::iterator it = right.begin(); it != right.end(); ++it) {
               heavier.erase(*it);
               lighter.erase(*it);
            }
         } else if (result == '<') {
            for (set<int>::iterator it = left.begin(); it != left.end(); ++it) {
               heavier.erase(*it);
            }
            for (set<int>::iterator it = right.begin(); it != right.end(); ++it) {
               lighter.erase(*it);
            }
            heavier = set_intersection(heavier, right);
            lighter = set_intersection(lighter, left);
         } else if (result == '>') {
            for (set<int>::iterator it = left.begin(); it != left.end(); ++it) {
               lighter.erase(*it);
            }
            for (set<int>::iterator it = right.begin(); it != right.end(); ++it) {
               heavier.erase(*it);
            }
            heavier = set_intersection(heavier, left);
            lighter = set_intersection(lighter, right);
         } else {
            assert(false);
         }

      }  

      if (heavier.size() == 1 && lighter.size() == 0) {
         cout << (*heavier.begin()) << endl;
      } else if (lighter.size() == 1 && heavier.size() == 0) {
         cout << (*lighter.begin()) << endl;
      } else if (lighter.size() == 1 && heavier.size() == 1 && *(lighter.begin()) == *(heavier.begin())) {
         cout << (*lighter.begin()) << endl;
      } else {
         cout << 0 << endl;
      }
   }
}
