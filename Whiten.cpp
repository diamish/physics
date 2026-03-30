#include "Whiten.h"
#include "FFTUtils.h"
#include <cmath>
#include <complex>
#include <vector>

void whiten_blocks(std::vector<double>& data, const std::vector<double>& psd, int fs) {
    int n = (int)data.size();

    // Derive FFT length from PSD
    int half = psd.size();
    int fftlen = 2 * (half - 1);

    std::vector<std::complex<double>> X(half);

    fft_real_to_complex(data.data(), n, X.data());

    for (int k = 0; k < half; k++) {
        double denom = std::sqrt(psd[k]);
        if (denom <= 1e-12) denom = 1e-12;
        X[k] /= denom;
    }

    std::vector<double> out(n);
    ifft_complex_to_real(X.data(), n, out.data());

    data = out;
}