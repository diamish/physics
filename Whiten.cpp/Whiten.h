#pragma once
#include <vector>

// Whiten vector in-place using provided PSD (one-sided). The PSD vector length should match FFT bins.
void whiten_blocks(std::vector<double>& data, const std::vector<double>& psd, int fs);

