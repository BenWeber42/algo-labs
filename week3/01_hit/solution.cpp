#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <iostream>

using namespace std;
using K = CGAL::Exact_predicates_exact_constructions_kernel;

int main() {

   while (true) {
      int n;
      cin >> n;

      if (n == 0) {
         break;
      }

      long x, y, a, b;
      cin >> x >> y >> a >> b;

      K::Ray_2 ray(K::Point_2(x, y), K::Point_2(a, b));

      bool intersect = false;

      while (n--) {
         long r, s, t, u;
         cin >> r >> s >> t >> u;
         
         if (!intersect) {
            K::Segment_2 segment(K::Point_2(r, s), K::Point_2(t, u));

            if (CGAL::do_intersect(ray, segment)) {
               intersect = true;
            }
         }
      }
      if (intersect) {
         cout << "yes" << endl;
      } else {
         cout << "no" << endl;
      }
   }

}
