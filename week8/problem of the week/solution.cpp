#include <iostream>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/basic.h>
#include <CGAL/QP_models.h>
#include <CGAL/QP_functions.h>
#undef NDEBUG
#include <cassert>

#ifdef CGAL_USE_GMP
   #include <CGAL/Gmpq.h>
   using ET = CGAL::Gmpq;
#else
   #include <CGAL/MP_Float.h>
   using ET = CGAL::MP_Float;
#endif

using ptype = ET; // program number type

using K = CGAL::Exact_predicates_inexact_constructions_kernel;
using Program = CGAL::Quadratic_program<ptype>;
using Solution = CGAL::Quadratic_program_solution<ET>;

using namespace std;

int main() {
   ios_base::sync_with_stdio(false);

   int tests;
   cin >> tests;
   while (tests--) {
      int l, s, w;
      cin >> l >> s >> w;

      vector<K::Point_2> lights(l);
      for (int i = 0; i < l; ++i) {
         cin >> lights[i];
      }

      vector<K::Point_2> stamps(s);
      vector<int> stamps_M(s);
      for (int i = 0; i < s; ++i) {
         cin >> stamps[i];
         cin >> stamps_M[i];
      }

      vector<K::Segment_2> walls(w);
      for (int i = 0; i < w; ++i) {
         cin >> walls[i];
      }

      // set 1 <= p_i <= 2^12
      Program lp(CGAL::SMALLER, true, 1, true, (1<<12) /* 2^12 */ );

      // all stamps
      for (int j = 0; j < s; ++j) {
         // all lights
         for (int i = 0; i < l; ++i) {
            bool affects = true;
            for (const auto& wall: walls) {
               if (CGAL::do_intersect(K::Segment_2(lights[i], stamps[j]), wall)) {
                  affects = false;
                  break;
               }
            }
            if (affects) {
               const unsigned long x = lights[i].x() - stamps[j].x();
               const unsigned long y = lights[i].y() - stamps[j].y();
               const unsigned long distance = x*x + y*y;
               const ptype a((unsigned long) 1, distance);
               lp.set_a(i, j, a);
               lp.set_a(i, j + s, a);
            } else {
               lp.set_a(i, j, 0);
               lp.set_a(i, j + s, 0);
            }
         }
         // illumination: I_j <= M_j
         lp.set_r(j, CGAL::SMALLER);
         lp.set_b(j, stamps_M[j]);
         // illumination: I_j >= 1
         lp.set_r(j + s, CGAL::LARGER);
         lp.set_b(j + s, 1);
      }

      Solution sol = CGAL::solve_linear_program(lp, ET());
      assert(sol.solves_linear_program(lp));

      if (sol.is_infeasible()) {
         cout << "no" << endl;
      } else {
         cout << "yes" << endl;
      }
   }
}
