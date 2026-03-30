#include "lacc/physics/GwCsv.hpp"
#include <fstream>
#include <sstream>
#include <stdexcept>

namespace lacc::physics {

GwSeries read_gw_csv(const std::string& path) {
    std::ifstream in(path);
    if (!in) throw std::runtime_error("Failed to open file");

    GwSeries s;
    std::string line;

    std::getline(in, line); // header

    while (std::getline(in, line)) {
        if (line.empty()) continue;

        std::stringstream ss(line);
        std::string a, b;

        std::getline(ss, a, ',');
        std::getline(ss, b, ',');

        s.t.push_back(std::stod(a));
        s.strain.push_back(std::stod(b));
    }

    return s;
}

} // namespace lacc::physics