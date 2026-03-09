#include "lacc/physics/GwCsv.hpp"
#include <fstream>
#include <sstream>
#include <stdexcept>

namespace lacc::physics {

GwSeries read_gw_csv(const std::string& path) {
  std::ifstream in(path);
  if (!in) throw std::runtime_error("Failed to open GW CSV: " + path);

  GwSeries s;
  std::string line;
  if (!std::getline(in, line)) throw std::runtime_error("Empty GW CSV: " + path); // header

  while (std::getline(in, line)) {
    if (line.empty()) continue;
    std::stringstream ss(line);
    std::string a, b;
    if (!std::getline(ss, a, ',')) continue;
    if (!std::getline(ss, b, ',')) continue;
    s.t.push_back(std::stod(a));
    s.strain.push_back(std::stod(b));
  }

  if (s.t.empty() || s.t.size() != s.strain.size())
    throw std::runtime_error("GW CSV parse error: " + path);

  return s;
}

} // namespace lacc::physics
