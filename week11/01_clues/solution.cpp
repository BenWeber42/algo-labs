#include <utility>
#include <functional>
#include <iostream>
#include <vector>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Triangulation_vertex_base_with_info_2.h>
#include <CGAL/Delaunay_triangulation_2.h>

using namespace std;

struct Info {
   // network cluster id & color
   int id, color;
   int visitation;
   Info() : id(-1), color(-1), visitation(0) {}
};

using dint = long; // distance type

using K = CGAL::Exact_predicates_inexact_constructions_kernel;
using Vb = CGAL::Triangulation_vertex_base_with_info_2<Info, K>;
using Tds = CGAL::Triangulation_data_structure_2<Vb>;
using Triangulation = CGAL::Delaunay_triangulation_2<K, Tds>;
using Point = Triangulation::Point;
using VertexHandle = Triangulation::Vertex_handle;

int main() {
   ios_base::sync_with_stdio(false);

   int tests;
   cin >> tests;
   while (tests--) {
      int n, m;
      dint r;
      cin >> n >> m >> r;

      r = r*r; // use squared distance for cheap construction

      vector<pair<Point, Info>> stations(n);
      for (int i = 0; i < n; ++i) {
         int x, y;
         cin >> x >> y;
         stations[i] = make_pair(Point(x, y), Info());
      }

      Triangulation t;
      t.insert(stations.begin(), stations.end());

      // do coloring:
      int id = 0;
      bool colorable = true;
      int visitation = 1;
      for (
         auto vertex_it = t.finite_vertices_begin();
         vertex_it != t.finite_vertices_end();
         ++vertex_it) {

         if (vertex_it->info().id != -1) {
            continue; // already belongs to a network and is already colored
         }

         vertex_it->info().id = id;
         vertex_it->info().color = 0; // start with black

         vector<VertexHandle> current{vertex_it}, next;
         
         while (!current.empty()) {
            for (auto& v: current) {
               auto n = t.incident_vertices(v);
               do {

                  if (t.is_infinite(n)) {
                     continue; // ignore infinite vertex
                  }

                  if (r < CGAL::squared_distance(v->point(), n->point())) {
                     continue; // ignore out of range stations
                  }

                  if (n->info().color == -1) {
                     // hasn't been color -> color and add to next
                     n->info().color = 1 - v->info().color;
                     n->info().id = id;
                     next.push_back(n);
                     continue;
                  }

                  // station has been colored, check if it's a valid coloring
                  if (v->info().color == n->info().color) {
                     colorable = false;
                     goto post_coloring;
                  }
               } while (++n != t.incident_vertices(v));
            }
            swap(current, next);
            next.clear();
         }

         ++id;
      }

      // do proper coloring check for all finite vertices via dfs
      for (
         auto vertex_it = t.finite_vertices_begin();
         vertex_it != t.finite_vertices_end();
         ++vertex_it) {

         function<bool(VertexHandle, VertexHandle)> ok =
         [&](VertexHandle v1, VertexHandle v2) {

            if (v2->info().visitation == visitation) {
               return true;
            }

            v2->info().visitation = visitation;

            if (r < CGAL::squared_distance(v1->point(), v2->point())) {
               return true;
            }

            if (v1 != v2 && v1->info().color == v2->info().color) {
               return false;
            }

            auto n = t.incident_vertices(v2);
            do {
               if (t.is_infinite(n)) {
                  continue;
               }

               if (!ok(v1, n)) {
                  return false;
               }
            } while (++n != t.incident_vertices(v2));

            return true;
         };

         if (!ok(vertex_it, vertex_it)) {
            colorable = false;
            goto post_coloring;
         }

         ++visitation;
      }

      post_coloring:
      for (int i = 0; i < m; ++i) {
         int xa, xb, ya, yb;
         cin >> xa >> ya >> xb >> yb;
         Point a(xa, ya), b(xb, yb);

         if (!colorable) {
            cout << 'n';
            continue;
         }

         if (CGAL::squared_distance(a, b) <= r) {
            // holmes and watson don't need the network
            cout << 'y';
            continue;
         }

         // closest station to holmes
         auto s1 = t.nearest_vertex(a);
         // closest station to watson
         auto s2 = t.nearest_vertex(b);

         assert(s1->info().id != -1);
         assert(s2->info().id != -1);

         if (r < CGAL::squared_distance(a, s1->point())) {
            // holmes out of reach to a station
            cout << 'n';
            continue;
         }
         if (r < CGAL::squared_distance(b, s2->point())) {
            // watson out of reach to a station
            cout << 'n';
            continue;
         }

         if (s1->info().id == s2->info().id) {
            // same network id -> there's a reachable path
            cout << 'y';
         } else {
            cout << 'n';
         }
      }

      cout << endl;
   }
}
