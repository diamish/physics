#pragma once
#include <complex>
#include <vector>

int next_power_of_two(int n);

// Converts a real array (length n) to complex spectrum (length n/2+1) using FFTW3.
// in: pointer to real array (size n). out: pointer to complex array (size n/2+1)
void fft_real_to_complex(const double* in, int n, std::complex<double>* out);

// Inverse: complex spectrum (n/2+1) to real time series (n)
void ifft_complex_to_real(const std::complex<double>* in, int n, double* out);

