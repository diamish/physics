#include "PSD.h"
#include "FFTUtils.h"
#include <cmath>
#include <stdexcept>

// Simple implementation: break data into overlapping segments, window (Hann), FFT, accumulate |X|^2
std::vector<double> welch_psd(const std::vector<double>& data, int fs, int seg_len, int seg_overlap) {
    if (seg_len <= 0) throw std::invalid_argument("seg_len");
    if (seg_overlap >= seg_len) throw std::invalid_argument("seg_overlap");

    int hop = seg_len - seg_overlap;
    size_t nd = data.size();
    if (nd < (size_t)seg_len) {
        // pad with zeros
        std::vector<double> padded = data;
        padded.resize(seg_len, 0.0);
        return welch_psd(padded, fs, seg_len, seg_overlap);
    }

    int num_segs = 1 + (nd - seg_len) / hop;
    int fftlen = next_power_of_two(seg_len);
    int half = fftlen/2 + 1;

    std::vector<double> psd(half, 0.0);
    std::vector<double> window(seg_len);
    // Hann window
    for (int i=0;i<seg_len;i++) window[i] = 0.5 * (1.0 - cos(2.0*M_PI*i/(seg_len-1)));

    std::vector<double> seg(fftlen, 0.0);
    std::vector<std::complex<double>> X(half);

    double scale = 0.0;
    for (int i=0;i<seg_len;i++) scale += window[i]*window[i];
    scale = scale * fs; // scaling to power spectral density units

    for (int s=0; s<num_segs; ++s) {
        size_t start = s * hop;
        std::fill(seg.begin(), seg.end(), 0.0);
        for (int i=0;i<seg_len;i++) seg[i] = data[start + i] * window[i];
        fft_real_to_complex(seg.data(), fftlen, X.data());
        for (int k=0;k<half;k++) {
            double mag2 = std::norm(X[k]);
            psd[k] += mag2;
        }
    }

    for (int k=0;k<half;k++) {
        psd[k] /= (double)num_segs;
        psd[k] /= scale; // convert to one-sided PSD
    }
    return psd;
}
