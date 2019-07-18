#include <utility>
#include <limits>
#include <tuple>
#include <vector>
#include <algorithm>
#include <iostream>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Delaunay_triangulation_2.h>
#include <CGAL/QP_models.h>
#include <CGAL/QP_functions.h>
#undef NDEBUG
#include <cassert>

using dint = long; // distance type
using pint = int; // position type

#ifdef CGAL_USE_GMP
   #include <CGAL/Gmpq.h>
   using ET = CGAL::Gmpq;
#else
   #include <CGAL/MP_Float.h>
   using ET = CGAL::MP_Float;
#endif

using Program = CGAL::Quadratic_program<ET>;
using Solution = CGAL::Quadratic_program_solution<ET>;

using K = CGAL::Exact_predicates_inexact_constructions_kernel;
using Triangulation = CGAL::Delaunay_triangulation_2<K>;


using namespace std;


// squared distance
dint sdist(dint x1, dint x2, dint y1, dint y2) {
   dint x = x1 - x2;
   dint y = y1 - y2;
   return x*x + y*y;
}

dint sdist(const K::Point_2& p1, const K::Point_2& p2) {
   return sdist((dint) p1.x(), (dint) p2.x(), (dint) p1.y(), (dint) p2.y());
}


int main() {
   ios_base::sync_with_stdio(false);

   int tests;
   cin >> tests;
   while (tests--) {
      int a, s, b, e;
      cin >> a >> s >> b >> e;

      // [(x, y, density)]
      vector<tuple<pint, pint, int>> asteroids(a);
      for (int i = 0; i < a; ++i) {
         pint x, y;
         int d;
         cin >> x >> y >> d;
         asteroids[i] = make_tuple(x, y, d);
      }

      // [(x, y, max radius)]
      vector<pair<K::Point_2, dint>> targets(s);
      for (int i = 0; i < s; ++i) {
         pint x, y;
         cin >> x >> y;
         targets[i] = make_pair(K::Point_2(x, y), numeric_limits<dint>::max());
      }

      if (b > 0) {
         // update max radius for each target

         // [(x, y]
         vector<K::Point_2> hunters(b);
         for (int i = 0; i < b; ++i) {
            pint x, y;
            cin >> x >> y;
            hunters[i] = K::Point_2(x, y);
         }

         Triangulation t;
         t.insert(hunters.begin(), hunters.end());

         for (auto& target: targets) {
            auto handle = t.nearest_vertex(target.first);
            assert(!t.is_infinite(handle));

            // closest hunter <=> max radius for that target
            K::Point_2 hunter = handle->point();

            target.second = sdist(target.first, hunter);
         }
      }

      Program lp(CGAL::SMALLER, true, 0, false, 0);

      for (int i = 0; i < a; ++i) {
         for (int j = 0; j < s; ++j) {
            dint dist = sdist(
               get<0>(asteroids[i]),
               targets[j].first.x(),
               get<1>(asteroids[i]),
               targets[j].first.y()
               );

            if (dist < targets[j].second) {
               lp.set_a(j, i, ET(1, max((dint) 1, dist)));
            } else {
               lp.set_a(j, i, 0);
            }
         }
         // total energy >= density
         lp.set_r(i, CGAL::LARGER);
         lp.set_b(i, get<2>(asteroids[i]));
      }

      // use only as much energy as is available
      for (int j = 0; j < s; ++j) {
         lp.set_a(j, a, 1);
      }
      lp.set_r(a, CGAL::SMALLER);
      lp.set_b(a, e);

      Solution sol = CGAL::solve_linear_program(lp, ET());
      assert(sol.solves_linear_program(lp));

      if (sol.is_infeasible()) {
         cout << "n" << endl;
      } else {
         cout << "y" << endl;
      }
   }
}
