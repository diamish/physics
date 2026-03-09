#pragma once
#include <string>
#include <vector>

struct StrainSeries {
    std::vector<double> data;
    double start_time = 0.0;
    double sample_rate = 4096.0;
};

class HDF5Reader {
public:
    // Reads the primary strain dataset. Throws std::runtime_error on error.
    static StrainSeries read_strain(const std::string& file_path, const std::string& dataset = "/strain/Strain");
};
