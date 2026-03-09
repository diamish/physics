// Simple test program to inspect an HDF5 file using HDF5Reader
#include "HDF5Reader.h"
#include <iostream>

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: test_hdf5reader file.hdf5 [start end]\n";
        return 1;
    }
    std::string file = argv[1];
    try {
        if (argc == 2) {
            auto s = HDF5Reader::read_strain(file);
            std::cout << "Read " << s.data.size() << " samples, start_time=" << s.start_time
                      << ", sample_rate=" << s.sample_rate << "\n";
            if (!s.dq_flags.empty()) std::cout << "DQ flags present (" << s.dq_flags.size() << ")\n";
            // print first few samples
            for (size_t i=0;i<std::min<size_t>(10, s.data.size()); ++i) {
                std::cout << i << ": " << s.data[i] << "\n";
            }
        } else if (argc >= 4) {
            double t0 = std::stod(argv[2]);
            double t1 = std::stod(argv[3]);
            auto s = HDF5Reader::read_strain_range(file, t0, t1);
            std::cout << "Read range " << t0 << " .. " << t1 << " -> samples=" << s.data.size()
                      << ", actual start_time=" << s.start_time << ", fs=" << s.sample_rate << "\n";
            for (size_t i=0;i<std::min<size_t>(10, s.data.size()); ++i) {
                std::cout << i << ": " << s.data[i] << "\n";
            }
        }
    } catch (const std::exception &e) {
        std::cerr << "Error reading file: " << e.what() << "\n";
        return 2;
    }
    return 0;
}
