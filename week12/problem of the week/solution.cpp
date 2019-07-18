#include <utility>
#include <vector>
#include <iostream>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Triangulation_vertex_base_with_info_2.h>
#include <CGAL/Delaunay_triangulation_2.h>

struct Info {
   Info() = default;
   Info(int id) : id(id), visited(false) {}
   int id;
   bool visited;
};

using dint = long; // distance type

using K = CGAL::Exact_predicates_inexact_constructions_kernel;
using Vb = CGAL::Triangulation_vertex_base_with_info_2<Info, K>;
using Tds = CGAL::Triangulation_data_structure_2<Vb>;
using Triangulation = CGAL::Delaunay_triangulation_2<K, Tds>;
using Point = Triangulation::Point;
using VertexHandle = Triangulation::Vertex_handle;

using namespace std;

int main() {
   ios_base::sync_with_stdio(false);

   int tests;
   cin >> tests;
   while (tests--) {
      int n;
      dint r;
      cin >> n >> r;

      r = r*r; // use squared distance for cheap distance construction

      vector<pair<Point, Info>> planets(n);
      for (int i = 0; i < n; ++i) {
         int x, y;
         cin >> x >> y;
         planets[i] = make_pair(Point(x, y), Info(i));
      }

      int lower = 1, upper = n/2;
      while (lower < upper) {

         int k = (lower + upper + 1)/2;
         Triangulation t;
         t.insert(planets.begin() + k, planets.end());

         bool possible = false;
         for (
            auto vertex = t.finite_vertices_begin();
            vertex != t.finite_vertices_end();
            ++vertex) {

            if (vertex->info().visited) {
               continue; // ignore visited vertices
            }

            vertex->info().visited = true;

            // do bsf to find the maximal cluster of `vertex`
            int size = 1;
            vector<VertexHandle> current{vertex}, next;
            while (!current.empty()) {

               for (auto& v: current) {
                  // go through all neighbors
                  auto c = t.incident_vertices(v);
                  do {
                     if (t.is_infinite(c)) {
                        continue; // ignore the infinite vertex
                     }
                     if (c->info().visited) {
                        continue; // ignore visited vertices
                     }
                     if (r < CGAL::squared_distance(v->point(), c->point())) {
                        continue; // ignore out of reach vertices
                     }
                     if (++size == k) {
                        possible = true;
                        goto done;
                     }
                     c->info().visited = true;
                     next.push_back(c);
                  } while (++c != t.incident_vertices(v));

               }

               swap(current, next);
               next.clear();
            }
         }

         done:
         if (possible) {
            lower = k;
         } else {
            upper = k - 1;
         }
      }

      assert(lower == upper);
      cout << upper << endl;
   }
}
