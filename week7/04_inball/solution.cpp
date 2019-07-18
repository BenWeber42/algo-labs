#include <iostream>
#include <CGAL/basic.h>
#include <CGAL/QP_models.h>
#include <CGAL/QP_functions.h>
#undef NDEBUG
#include <cassert>

#ifdef CGAL_USE_GMP
   #include <CGAL/Gmpz.h>
   using ET = CGAL::Gmpz;
#else
   #include <CGAL/MP_Float.h>
   using ET = CGAL::MP_Float;
#endif

// program integer (with this alias we can easily change it later)
using pint = int;

using Program = CGAL::Quadratic_program<pint>;
using Solution = CGAL::Quadratic_program_solution<ET>;
using Status = CGAL::Quadratic_program_status;

using namespace std;

int main() {
   ios_base::sync_with_stdio(false);

   while (true) {
      int n;
      cin >> n;

      if (n == 0) {
         break;
      }

      int d;
      cin >> d;

      Program lp(CGAL::SMALLER, false, 0, false, 0);

      // i: constraint index
      for (int i = 0; i < n; ++i) {

         pint norm = 0;
         // j: variable index
         for (int j = 0; j < d; ++j) {
            pint a;
            cin >> a;

            norm += a*a;

            lp.set_a(j, i, a);
         }

         norm = sqrt(norm);
         lp.set_a(d, i, norm);

         pint b;
         cin >> b;
         lp.set_b(i, b);
      }

      // j: variable index
      for (int j = 0; j < d; ++j) {
         lp.set_c(j, 0);
      }

      lp.set_c(d, -1);
      lp.set_c0(0);

      // r >= 0
      lp.set_l(d, true, 0);

      Solution s = CGAL::solve_linear_program(lp, ET());
      assert(s.solves_linear_program(lp));

      if (s.is_infeasible()) {
         cout << "none" << endl;
      } else if (s.is_unbounded()) {
         cout << "inf" << endl;
      } else if (s.is_optimal()) {
         cout << (long) floor(CGAL::to_double(*(s.variable_values_begin() + d))) << endl;
      } else {
         assert(false);
      }

   }
}
