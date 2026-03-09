#pragma once
#include <vector>

// Welch PSD estimator returning one-sided PSD for frequencies 0..N/2
std::vector<double> welch_psd(const std::vector<double>& data, int fs, int seg_len, int seg_overlap);

