#undef NDEBUG

#include <iostream>
#include <vector>
#include <unordered_set>
#include <algorithm>
#include <cassert>
#include <deque>

using namespace std;

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
      int n, num_matches = 0;
      cin >> n;

      vector<int> m(n);
      for (int i = 0; i < n; ++i) {
         cin >> m[i];
         num_matches += m[i];
      }

      vector<Match> matches;
      for (int i = 0; i < n; ++i) {
         for (int j = 0; j < m[i]; ++j) {
            int pos;
            cin >> pos;
            matches.push_back(Match(i, pos));
         }
      }

      // sort by position
      sort(matches.begin(), matches.end());

      int length = numeric_limits<int>::max();
      Snippet snippet(n);
      for (const auto& match: matches) {
         snippet.add_match(match);
         if (snippet.complete()) {
            length = min(length, snippet.length());
         }
      }

      cout << length << endl;

   }

}
