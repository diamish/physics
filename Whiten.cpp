#include "Whiten.h"
#include "FFTUtils.h"
#include <cmath>
#include <complex>
#include <vector>
#include <algorithm>

// Very simple block-wise whitening: process the whole time series in chunks of fftlen
void whiten_blocks(std::vector<double>& data, const std::vector<double>& psd, int fs) {
    int n = (int)data.size();
    int fftlen = 1;
    while (fftlen < n) fftlen <<= 1;
    int half = fftlen/2 + 1;

    std::vector<std::complex<double>> X(half);
    fft_real_to_complex(data.data(), n, X.data());

    // Interpolate PSD to FFT bins if needed. Here we assume psd.size() == half.
    if ((int)psd.size() != half) {
        // naive resize/interp: zero-pad or truncate
        // For skeleton, guard and return
        return;
    }
    for (int k=0;k<half;k++) {
        double denom = std::sqrt(psd[k] / 2.0);
        if (denom <= 0) denom = 1e-12;
        X[k] /= denom;
    }
    std::vector<double> out(n);
    ifft_complex_to_real(X.data(), n, out.data());
    data = out;
}
