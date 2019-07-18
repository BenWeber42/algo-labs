#include <limits>
#include <iostream>
#include <vector>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Delaunay_triangulation_2.h>
#include <CGAL/Triangulation_face_base_with_info_2.h>
#undef NDEBUG
#include <cassert>


using pint = int; // point integer
using dint = long; // distance integer

struct FaceInfo {
   dint d0, d1, d2;
   dint distance;
};

using K = CGAL::Exact_predicates_inexact_constructions_kernel;
using Vb = CGAL::Triangulation_vertex_base_2<K>;
using Fb = CGAL::Triangulation_face_base_with_info_2<FaceInfo, K>;
using Tds = CGAL::Triangulation_data_structure_2<Vb, Fb>;
using Triangulation = CGAL::Delaunay_triangulation_2<K, Tds>;
using Face_handle = Triangulation::Face_handle;

using namespace std;

dint distance(const K::Point_2& a, const K::Point_2& b) {
   dint x = ((dint) a.x()) - ((dint) b.x());
   dint y = ((dint) a.y()) - ((dint) b.y());

   return x*x + y*y;
}

dint max_distance(const Face_handle& f, const Triangulation& t) {
   assert(!t.is_infinite(f));

   dint distance = numeric_limits<dint>::min();

   distance = max(distance, min(f->info().d0, f->neighbor(0)->info().distance));
   distance = max(distance, min(f->info().d1, f->neighbor(1)->info().distance));
   distance = max(distance, min(f->info().d2, f->neighbor(2)->info().distance));

   return distance;
}

int main() {
   ios_base::sync_with_stdio(false);

   while (true) {
      int n;
      cin >> n;

      if (n == 0) {
         break;
      }

      vector<K::Point_2> pts(n);
      for (int i = 0; i < n; ++i) {
         cin >> pts[i];
      }

      Triangulation t;
      t.insert(pts.begin(), pts.end());

      // infinite faces = max
      auto f = t.incident_faces(t.infinite_vertex());
      do {
         assert(t.is_infinite(f));
         f->info().distance = numeric_limits<dint>::max();
      } while (++f != t.incident_faces(t.infinite_vertex()));

      // initialize finite faces
      for (auto f = t.finite_faces_begin(); f != t.finite_faces_end(); ++f) {
         f->info().d0 = distance(f->vertex(1)->point(), f->vertex(2)->point());
         f->info().d1 = distance(f->vertex(0)->point(), f->vertex(2)->point());
         f->info().d2 = distance(f->vertex(0)->point(), f->vertex(1)->point());
         f->info().distance = numeric_limits<dint>::min();
      }

      set<Face_handle> current, next;
      for (auto f = t.finite_faces_begin(); f != t.finite_faces_end(); ++f) {
         current.insert(f);
      }

      // compute fixed point with chaotic iteration
      while (!current.empty()) {
         next.clear();
         for (auto& f: current) {
            assert(!t.is_infinite(f));

            dint distance = max_distance(f, t);

            if (distance > f->info().distance) {
               f->info().distance = distance;

               // could carry out a minor optimization here (each gate limits the maximum)
               if (
                  !t.is_infinite(f->neighbor(0)) &&
                  distance > f->neighbor(0)->info().distance
                  ) {

                  next.insert(f->neighbor(0));
               }
               if (
                  !t.is_infinite(f->neighbor(1)) &&
                  distance > f->neighbor(1)->info().distance
                  ) {

                  next.insert(f->neighbor(1));
               }
               if (
                  !t.is_infinite(f->neighbor(2)) &&
                  distance > f->neighbor(2)->info().distance
                  ) {

                  next.insert(f->neighbor(2));
               }
            }
         }

         swap(current, next);
      }

      int m;
      cin >> m;
      for (int i = 0; i < m; ++i) {
         K::Point_2 u;
         cin >> u;
         dint d;
         cin >> d;

         auto p = t.nearest_vertex(u)->point();
         if (d > distance(u, p)) {
            cout << "n";
            continue;
         }

         auto f = t.locate(u);
         if (t.is_infinite(f)) {
            cout << "y";
            continue;
         }

         // point could be located on a face boundary...
         assert(f->info().distance > numeric_limits<dint>::min());

         if (4*d <= f->info().distance) {
            cout << "y";
         } else {
            cout << "n";
         }
      }
      cout << endl;
   }
}
