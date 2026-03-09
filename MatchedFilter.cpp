#include "lacc/physics/MatchedFilter.hpp"
#include "lacc/adt/PriorityQueue.hpp"

#include <algorithm>

namespace lacc::physics {

static std::vector<double> make_template(int window) {
  std::vector<double> temp(window);
  for (int i = 0; i < window; ++i) {
    double x = static_cast<double>(i) / (window - 1);
    temp[i] = (x < 0.5) ? (2.0 * x) : (2.0 * (1.0 - x));
  }
  return temp;
}

std::vector<Peak> simple_matched_filter_peaks(
  const std::vector<double>& t,
  const std::vector<double>& strain,
  int window,
  double threshold,
  int topk
) {
  std::vector<Peak> peaks;
  if (window < 4 || static_cast<std::size_t>(window) >= strain.size()) return peaks;

  const auto temp = make_template(window);

  std::vector<double> score(strain.size(), 0.0);
  for (std::size_t i = 0; i + window < strain.size(); ++i) {
    double s = 0.0;
    for (int j = 0; j < window; ++j) s += strain[i + j] * temp[j];
    score[i + window/2] = s;
  }

  for (std::size_t i = 1; i + 1 < score.size(); ++i) {
    if (score[i] > threshold && score[i] > score[i-1] && score[i] > score[i+1]) {
      peaks.push_back(Peak{t[i], score[i]});
    }
  }

  // Keep only the Top-K peaks without sorting the entire list.
  // We use a min-heap of size K where the *smallest* of the kept peaks is on top.
  struct ByScoreMinHeap {
    bool operator()(const Peak& a, const Peak& b) const {
      // Return true if a has *lower* priority than b.
      // For a min-heap by score, larger scores have lower priority.
      return a.score > b.score;
    }
  };

  if (topk <= 0 || peaks.empty()) return {};

  lacc::adt::PriorityQueue<Peak, ByScoreMinHeap> pq;
  for (const auto& p : peaks) {
    if (static_cast<int>(pq.size()) < topk) {
      pq.push(p);
    } else if (p.score > pq.top().score) {
      pq.pop();
      pq.push(p);
    }
  }

  std::vector<Peak> out;
  out.reserve(pq.size());
  while (!pq.empty()) {
    out.push_back(pq.top());
    pq.pop();
  }

  // Deterministic ordering: primary descending score, secondary ascending time.
  std::sort(out.begin(), out.end(),
            [](const Peak& a, const Peak& b){
              if (a.score != b.score) return a.score > b.score;
              return a.time < b.time;
            });
  return out;
}

} // namespace lacc::physics
