// main.cpp
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <getopt.h>

#include "CSVReader.h"
#include "PSD.h"
#include "Whiten.h"
#include "BurstDetector.h"

void print_usage() {
    std::cout << "Usage: ligo_scan --input file.csv --detector H1 --start 0 --end 60 --fs 4096 --out candidates.csv\n";
}

int main(int argc, char** argv) {
    std::string input_file = "gw_sample.csv";
    std::string detector = "H1";
    double start_time = 0.0;
    double end_time = 60.0;
    int fs = 4096;
    std::string out_file = "candidates.csv";

    static struct option long_options[] = {
        {"input", required_argument, 0, 'i'},
        {"detector", required_argument, 0, 'd'},
        {"start", required_argument, 0, 's'},
        {"end", required_argument, 0, 'e'},
        {"fs", required_argument, 0, 'f'},
        {"out", required_argument, 0, 'o'},
        {"help", no_argument, 0, 'h'},
        {0,0,0,0}
    };

    int opt;
    while ((opt = getopt_long(argc, argv, "i:d:s:e:f:o:h", long_options, nullptr)) != -1) {
        switch (opt) {
            case 'i': input_file = optarg; break;
            case 'd': detector = optarg; break;
            case 's': start_time = atof(optarg); break;
            case 'e': end_time = atof(optarg); break;
            case 'f': fs = atoi(optarg); break;
            case 'o': out_file = optarg; break;
            default: print_usage(); return 1;
        }
    }

    if (input_file.empty()) {
        std::cerr << "Error: input file required\n";
        print_usage();
        return 1;
    }

    std::cout << "Reading " << input_file << " for detector " << detector << "\n";

    // Read CSV strain data
    std::vector<double> strain;
    try {
        strain = CSVReader::read_strain(input_file);
    } catch (const std::exception &ex) {
        std::cerr << "Failed to read CSV: " << ex.what() << "\n";
        return 1;
    }

    if (strain.empty()) {
        std::cerr << "No data loaded from CSV.\n";
        return 1;
    }

    // TODO: trim strain to requested start/end times if necessary
    // For now assume file contains desired range

    // Compute PSD (Welch)
    int seg_len = 4 * fs;     // 4-second base segment
    int seg_overlap = seg_len / 2;
    std::vector<double> psd = welch_psd(strain, fs, seg_len, seg_overlap);

    // Whiten entire series in blocks
    std::vector<double> whitened = strain;
    whiten_blocks(whitened, psd, fs);

    // Run burst detector on whitened series
    std::vector<Candidate> cands = detect_bursts(whitened, start_time, fs);

    // Write output CSV
    std::ofstream ofs(out_file);
    ofs << "detector,start_time,peak_time,end_time,snr,central_freq,bandwidth,notes\n";
    for (const auto &c : cands) {
        ofs << detector << "," << c.start_time << "," << c.peak_time << "," << c.end_time << ","
            << c.snr << "," << c.central_freq << "," << c.bandwidth << "," << c.notes << "\n";
    }
    ofs.close();

    std::cout << "Wrote " << cands.size() << " candidates to " << out_file << "\n";
    return 0;
}