#include "FFTUtils.h"
#include <fftw3.h>
#include <cmath>
#include <stdexcept>

int next_power_of_two(int n) {
    int p = 1;
    while (p < n) p <<= 1;
    return p;
}

void fft_real_to_complex(const double* in, int n, std::complex<double>* out) {
    int N = next_power_of_two(n);
    std::vector<double> buf(N, 0.0);
    for (int i=0;i<n;i++) buf[i] = in[i];
    fftw_complex* O = (fftw_complex*) fftw_malloc(sizeof(fftw_complex)*(N/2+1));
    fftw_plan p = fftw_plan_dft_r2c_1d(N, buf.data(), O, FFTW_ESTIMATE);
    fftw_execute(p);
    for (int k=0;k<=N/2;k++) out[k] = std::complex<double>(O[k][0], O[k][1]);
    fftw_destroy_plan(p);
    fftw_free(O);
}

void ifft_complex_to_real(const std::complex<double>* in, int n, double* out) {
    int N = next_power_of_two(n);
    fftw_complex* I = (fftw_complex*) fftw_malloc(sizeof(fftw_complex)*(N/2+1));
    for (int k=0;k<=N/2;k++) { I[k][0] = in[k].real(); I[k][1] = in[k].imag(); }
    std::vector<double> buf(N, 0.0);
    fftw_plan p = fftw_plan_dft_c2r_1d(N, I, buf.data(), FFTW_ESTIMATE);
    fftw_execute(p);
    // normalize (FFTW does not normalize inverse)
    for (int i=0;i<n;i++) out[i] = buf[i] / (double)N;
    fftw_destroy_plan(p);
    fftw_free(I);
}
