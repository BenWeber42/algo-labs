#include <utility>
#include <algorithm>
#include <iostream>
#include <deque>
#include <vector>
#include <unordered_set>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>

#undef NDEBUG
#include <cassert>

using namespace std;
using IK = CGAL::Exact_predicates_inexact_constructions_kernel;

bool right_of(const IK::Point_2& a, const IK::Point_2& b, const IK::Point_2& c) {
   return !CGAL::left_turn(a, b, c);
}

class Segment {
private:
   IK::Point_2 a, b;
public:
   Segment(const IK::Point_2& a, const IK::Point_2& b) : a(a), b(b) {}
   Segment() : a(0, 0), b(0, 0) {}

   const IK::Point_2& get_a() const {
      return a;
   }

   const IK::Point_2& get_b() const {
      return b;
   }

   void swap() {
      std::swap(a, b);
   }
};

class Triangle {
private:
   Segment a, b, c;
public:
   Triangle() : a(), b(), c() {}
   Triangle(const Segment& _a, const Segment& _b, const Segment& _c) : a(_a), b(_b), c(_c) {

      if (!right_of(a.get_a(), a.get_b(), b.get_a())) {
         a.swap();
      }

      if (!right_of(b.get_a(), b.get_b(), c.get_a())) {
         b.swap();
      }

      if (!right_of(c.get_a(), c.get_b(), a.get_a())) {
         c.swap();
      }

      assert(right_of(a.get_a(), a.get_b(), b.get_a()));
      assert(right_of(b.get_a(), b.get_b(), c.get_a()));
      assert(right_of(c.get_a(), c.get_b(), a.get_a()));
   }

   bool contains(const IK::Point_2& p) const {
      return
         right_of(a.get_a(), a.get_b(), p) &&
         right_of(b.get_a(), b.get_b(), p) &&
         right_of(c.get_a(), c.get_b(), p);
   }

   bool contains(const Segment& s) const {
      return contains(s.get_a()) && contains(s.get_b());
   }
};

class Match {
private:
   int word, pos;
public:
   Match() : word(-1), pos(-1) {}
   Match(int word, int pos) : word(word), pos(pos) {}

   int get_word() const {
      return word;
   }

   int get_pos() const {
      return pos;
   }

   bool operator<(const Match& other) const {
      return pos < other.pos;
   }
};

class Snippet {
private:
   vector<int> histogram;
   deque<Match> snippet;
   int distinct_words;

   bool vi(int i) const {
      return 0 <= i && i < histogram.size();
   }

   int count(const Match& match) {
      return count(match.get_word());
   }

   int count(int word) const {
      assert(vi(word));
      return histogram[word];
   }

   void add(const Match& match) {
      assert(vi(match.get_word()));
      snippet.push_back(match);
      if (0 == histogram[match.get_word()]++) {
         ++distinct_words;
      }
   }

   void remove() {
      if (0 == --histogram[snippet.front().get_word()]) {
         --distinct_words;
      }
      snippet.pop_front();
   }

   const Match& get() const {
      return snippet.front();
   }

   void normalize() {
      while (count(get()) > 1) {
         remove();
      }
   }

public:
   Snippet(int n) : histogram(n, 0), snippet(), distinct_words(0) {}

   void add_match(const Match& match) {
      add(match);
      normalize();
   }

   bool complete() const {
      return distinct_words == histogram.size();
   }

   int length() const {
      return snippet.back().get_pos() - snippet.front().get_pos() + 1;
   }
};

int main() {
   ios_base::sync_with_stdio(false);

   int tests;
   cin >> tests;
   while (0 < tests--) {
      int m, n;
      cin >> m >> n;

      vector<IK::Point_2> path_points(m);
      for (int i = 0; i < m; ++i) {
         int x, y;
         cin >> x >> y;
         path_points[i] = IK::Point_2(x, y);
      }

      vector<Segment> path(m - 1);
      for (int i = 1; i < m; ++i) {
         path[i - 1] = Segment(path_points[i], path_points[i - 1]);
      }

      vector<Triangle> parts(n);
      for (int i = 0; i < n; ++i) {
         int x1, x2, y1, y2;

         cin >> x1 >> y1 >> x2 >> y2;
         Segment a(IK::Point_2(x1, y1), IK::Point_2(x2, y2));
         cin >> x1 >> y1 >> x2 >> y2;
         Segment b(IK::Point_2(x1, y1), IK::Point_2(x2, y2));
         cin >> x1 >> y1 >> x2 >> y2;
         Segment c(IK::Point_2(x1, y1), IK::Point_2(x2, y2));

         parts[i] = Triangle(a, b, c);
      }

      vector<Match> matches;
      for (int j = 0; j < n; ++j) {
         for (int i = 0; i < m - 1; ++i) {
            if (parts[j].contains(path[i])) {
               matches.push_back(Match(i, j));
            }
         }
      }

      sort(matches.begin(), matches.end());
      
      int length = numeric_limits<int>::max();
      Snippet snippet(m - 1);
      for (const auto& match: matches) {
         snippet.add_match(match);
         if (snippet.complete()) {
            length = std::min(length, snippet.length());
         }
      }

      cout << length << endl;
   }
}
