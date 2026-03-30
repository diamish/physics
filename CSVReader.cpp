#include "CSVReader.h"
#include <fstream>
#include <sstream>
#include <iostream>

std::vector<double> CSVReader::read_strain(const std::string& filename) {
    std::vector<double> strain;
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        return strain;
    }

    std::string line;

    // 🔹 Skip header line: "time,strain"
    std::getline(file, line);

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string time_str, strain_str;

        // Split by comma
        if (std::getline(ss, time_str, ',') &&
            std::getline(ss, strain_str, ',')) {

            double value;
            std::stringstream valStream(strain_str);

            if (valStream >> value) {
                strain.push_back(value);
            }
        }
    }

    file.close();

    std::cout << "Loaded " << strain.size() << " strain values.\n";
    return strain;
}