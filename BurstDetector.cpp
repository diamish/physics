#include "BurstDetector.h"
#include <cmath>
#include <vector>

std::vector<Candidate> detect_bursts(const std::vector<double>& whitened, double start_time, int fs) {
    std::vector<Candidate> results;

    int n = whitened.size();
    int win_samples = std::max(1, fs / 10);
    int hop = win_samples / 2;

    double threshold = 5.0; // assume whitened ~ N(0,1)

    for (int s = 0; s + win_samples <= n; s += hop) {
        double sum2 = 0.0;

        for (int i = 0; i < win_samples; i++)
            sum2 += whitened[s + i] * whitened[s + i];

        double rms = std::sqrt(sum2 / win_samples);

        if (rms > threshold) {
            Candidate c;
            c.start_time = start_time + (double)s / fs;
            c.peak_time = start_time + (double)(s + win_samples / 2) / fs;
            c.end_time = start_time + (double)(s + win_samples) / fs;
            c.snr = rms;
            c.central_freq = 0.0;
            c.bandwidth = 0.0;
            c.notes = "excess_power_rms";

            results.push_back(c);

            s += win_samples;
        }
    }

    return results;
}