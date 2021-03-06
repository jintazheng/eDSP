/*
 * eDSP, A cross-platform Digital Signal Processing library written in modern C++.
 * Copyright (C) 2018 Mohammed Boujemaoui Boulaghmoudi, All rights reserved.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of  MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along withº
 * this program.  If not, see <http://www.gnu.org/licenses/>
 *
 * File: fftw_impl.hpp
 * Date: 09/06/18
 * Author: Mohammed Boujemaoui
 */

#ifndef EDSP_FFTW_IMPL_HPP
#define EDSP_FFTW_IMPL_HPP

#include <edsp/meta/is_null.hpp>
#include <edsp/meta/advance.hpp>
#include <edsp/meta/iterator.hpp>
#include <edsp/meta/expects.hpp>
#include <edsp/meta/data.hpp>

#include <complex>
#include <fftw3.h>
#include <algorithm>

namespace edsp { inline namespace spectral {
    namespace internal {
        template <typename T>
        inline T* fftw_cast(const T* p) noexcept {
            return const_cast<T*>(p);
        }

        inline fftwf_complex* fftw_cast(const std::complex<float>* p) {
            return const_cast<fftwf_complex*>(reinterpret_cast<const fftwf_complex*>(p));
        }

        inline fftw_complex* fftw_cast(const std::complex<double>* p) {
            return const_cast<fftw_complex*>(reinterpret_cast<const fftw_complex*>(p));
        }
    } // namespace internal

    template <typename T>
    struct fftw_impl {
        using value_type   = T;
        using complex_type = std::complex<T>;
        using size_type    = int;

        explicit fftw_impl(size_type nfft) : nfft_(nfft) {}

        ~fftw_impl() {
            if (!meta::is_null(plan_)) {
                fftwf_destroy_plan(plan_);
            }
        }

        inline void dft(const complex_type* src, complex_type* dst) {
            if (meta::is_null(plan_)) {
                input_complex.resize(static_cast<unsigned long>(nfft_));
                output_complex.resize(static_cast<unsigned long>(nfft_));
                plan_ = fftwf_plan_dft_1d(nfft_, internal::fftw_cast(meta::data(input_complex)),
                                          internal::fftw_cast(meta::data(output_complex)), FFTW_FORWARD,
                                          FFTW_ESTIMATE | FFTW_PRESERVE_INPUT);
            }

            std::copy(src, src + nfft_, std::begin(input_complex));
            fftwf_execute_dft(plan_, internal::fftw_cast(meta::data(input_complex)),
                              internal::fftw_cast(meta::data(output_complex)));
            std::copy(std::cbegin(output_complex), std::cend(output_complex), dst);
        }

        inline void idft(const complex_type* src, complex_type* dst) {
            if (meta::is_null(plan_)) {
                input_complex.resize(static_cast<unsigned long>(nfft_));
                output_complex.resize(static_cast<unsigned long>(nfft_));
                plan_ = fftwf_plan_dft_1d(nfft_, internal::fftw_cast(meta::data(input_complex)),
                                          internal::fftw_cast(meta::data(output_complex)), FFTW_BACKWARD,
                                          FFTW_ESTIMATE | FFTW_PRESERVE_INPUT);
            }

            std::copy(src, src + nfft_, std::begin(input_complex));
            fftwf_execute_dft(plan_, internal::fftw_cast(meta::data(input_complex)),
                              internal::fftw_cast(meta::data(output_complex)));
            std::copy(std::cbegin(output_complex), std::cend(output_complex), dst);
        }

        inline void dft(const value_type* src, complex_type* dst) {
            if (meta::is_null(plan_)) {
                input_real.resize(static_cast<unsigned long>(nfft_));
                output_complex.resize(static_cast<unsigned long>(std::floor(nfft_ / 2) + 1));
                plan_ = fftwf_plan_dft_r2c_1d(nfft_, internal::fftw_cast(meta::data(input_real)),
                                              internal::fftw_cast(meta::data(output_complex)),
                                              FFTW_ESTIMATE | FFTW_PRESERVE_INPUT);
            }

            std::copy(src, src + nfft_, std::begin(input_real));
            fftwf_execute_dft_r2c(plan_, internal::fftw_cast(meta::data(input_real)),
                                  internal::fftw_cast(meta::data(output_complex)));
            std::copy(std::cbegin(output_complex), std::cend(output_complex), dst);
        }

        inline void idft(const complex_type* src, value_type* dst) {
            const auto c_size = std::floor(nfft_ / 2) + 1;
            if (meta::is_null(plan_)) {
                input_complex.resize((unsigned long) c_size);
                output_real.resize(static_cast<unsigned long>(nfft_));
                plan_ = fftwf_plan_dft_c2r_1d(nfft_, internal::fftw_cast(meta::data(input_complex)),
                                              internal::fftw_cast(meta::data(output_real)),
                                              FFTW_ESTIMATE | FFTW_PRESERVE_INPUT);
            }

            std::copy(src, src + (int) c_size, std::begin(input_complex));
            fftwf_execute_dft_c2r(plan_, internal::fftw_cast(meta::data(input_complex)),
                                  internal::fftw_cast(meta::data(output_real)));
            std::copy(std::cbegin(output_real), std::cend(output_real), dst);
        }

        inline void dht(const value_type* src, value_type* dst) {
            if (meta::is_null(plan_)) {
                input_real.resize(static_cast<unsigned long>(nfft_));
                output_real.resize(static_cast<unsigned long>(nfft_));
                plan_ = fftwf_plan_r2r_1d(nfft_, internal::fftw_cast(meta::data(input_real)),
                                          internal::fftw_cast(meta::data(output_real)), FFTW_DHT,
                                          FFTW_ESTIMATE | FFTW_PRESERVE_INPUT);
            }
            std::copy(src, src + nfft_, std::begin(input_real));
            fftwf_execute_r2r(plan_, internal::fftw_cast(meta::data(input_real)),
                              internal::fftw_cast(meta::data(output_real)));
            std::copy(std::cbegin(output_real), std::cend(output_real), dst);
        }

        inline void dct(const value_type* src, value_type* dst) {
            if (meta::is_null(plan_)) {
                input_real.resize(static_cast<unsigned long>(nfft_));
                output_real.resize(static_cast<unsigned long>(nfft_));
                plan_ = fftwf_plan_r2r_1d(nfft_, internal::fftw_cast(meta::data(input_real)),
                                          internal::fftw_cast(meta::data(output_real)), FFTW_REDFT10,
                                          FFTW_ESTIMATE | FFTW_PRESERVE_INPUT);
            }
            std::copy(src, src + nfft_, std::begin(input_real));
            fftwf_execute_r2r(plan_, internal::fftw_cast(meta::data(input_real)),
                              internal::fftw_cast(meta::data(output_real)));
            std::copy(std::cbegin(output_real), std::cend(output_real), dst);
        }

        inline void idct(const value_type* src, value_type* dst) {
            if (meta::is_null(plan_)) {
                input_real.resize(static_cast<unsigned long>(nfft_));
                output_real.resize(static_cast<unsigned long>(nfft_));
                plan_ = fftwf_plan_r2r_1d(nfft_, internal::fftw_cast(meta::data(input_real)),
                                          internal::fftw_cast(meta::data(output_real)), FFTW_REDFT01,
                                          FFTW_ESTIMATE | FFTW_PRESERVE_INPUT);
            }

            std::copy(src, src + nfft_, std::begin(input_real));
            fftwf_execute_r2r(plan_, internal::fftw_cast(meta::data(input_real)),
                              internal::fftw_cast(meta::data(output_real)));
            std::copy(std::cbegin(output_real), std::cend(output_real), dst);
        }

        inline void idft_scale(value_type* dst) {
            const auto scaling = static_cast<value_type>(nfft_);
            for (size_type i = 0; i < nfft_; ++i) {
                dst[i] /= scaling;
            }
        }

        inline void idft_scale(complex_type* dst) {
            const auto scaling = static_cast<value_type>(nfft_);
            for (size_type i = 0; i < nfft_; ++i) {
                dst[i] /= scaling;
            }
        }

        inline void idct_scale(value_type* dst) {
            const auto scaling = 2 * nfft_;
            for (size_type i = 0; i < nfft_; ++i) {
                dst[i] /= scaling;
            }
        }

    private:
        ::fftwf_plan plan_{nullptr};
        size_type nfft_;
        std::vector<std::complex<float>> input_complex;
        std::vector<std::complex<float>> output_complex;
        std::vector<float> input_real;
        std::vector<float> output_real;
    };

    template <>
    struct fftw_impl<float> {
        using value_type   = float;
        using complex_type = std::complex<float>;
        using size_type    = int;

        explicit fftw_impl(size_type nfft) : nfft_(nfft) {}

        ~fftw_impl() {
            if (!meta::is_null(plan_)) {
                fftwf_destroy_plan(plan_);
            }
        }

        inline void dft(const complex_type* src, complex_type* dst) {
            if (meta::is_null(plan_))
                plan_ = fftwf_plan_dft_1d(nfft_, internal::fftw_cast(src), internal::fftw_cast(dst), FFTW_FORWARD,
                                          FFTW_ESTIMATE | FFTW_PRESERVE_INPUT);
            fftwf_execute_dft(plan_, internal::fftw_cast(src), internal::fftw_cast(dst));
        }

        inline void idft(const complex_type* src, complex_type* dst) {
            if (meta::is_null(plan_)) {
                plan_ = fftwf_plan_dft_1d(nfft_, internal::fftw_cast(src), internal::fftw_cast(dst), FFTW_BACKWARD,
                                          FFTW_ESTIMATE | FFTW_PRESERVE_INPUT);
            }
            fftwf_execute_dft(plan_, internal::fftw_cast(src), internal::fftw_cast(dst));
        }

        inline void dft(const value_type* src, complex_type* dst) {
            if (meta::is_null(plan_)) {
                plan_ = fftwf_plan_dft_r2c_1d(nfft_, internal::fftw_cast(src), internal::fftw_cast(dst),
                                              FFTW_ESTIMATE | FFTW_PRESERVE_INPUT);
            }
            fftwf_execute_dft_r2c(plan_, internal::fftw_cast(src), internal::fftw_cast(dst));
        }

        inline void idft(const complex_type* src, value_type* dst) {
            if (meta::is_null(plan_)) {
                plan_ = fftwf_plan_dft_c2r_1d(nfft_, internal::fftw_cast(src), internal::fftw_cast(dst),
                                              FFTW_ESTIMATE | FFTW_PRESERVE_INPUT);
            }
            fftwf_execute_dft_c2r(plan_, internal::fftw_cast(src), internal::fftw_cast(dst));
        }

        inline void dht(const value_type* src, value_type* dst) {
            if (meta::is_null(plan_)) {
                plan_ = fftwf_plan_r2r_1d(nfft_, internal::fftw_cast(src), internal::fftw_cast(dst), FFTW_DHT,
                                          FFTW_ESTIMATE | FFTW_PRESERVE_INPUT);
            }
            fftwf_execute_r2r(plan_, internal::fftw_cast(src), internal::fftw_cast(dst));
        }

        inline void dct(const value_type* src, value_type* dst) {
            if (meta::is_null(plan_)) {
                plan_ = fftwf_plan_r2r_1d(nfft_, internal::fftw_cast(src), internal::fftw_cast(dst), FFTW_REDFT10,
                                          FFTW_ESTIMATE | FFTW_PRESERVE_INPUT);
            }
            fftwf_execute_r2r(plan_, internal::fftw_cast(src), internal::fftw_cast(dst));
        }

        inline void idct(const value_type* src, value_type* dst) {
            if (meta::is_null(plan_)) {
                plan_ = fftwf_plan_r2r_1d(nfft_, internal::fftw_cast(src), internal::fftw_cast(dst), FFTW_REDFT01,
                                          FFTW_ESTIMATE | FFTW_PRESERVE_INPUT);
            }
            fftwf_execute_r2r(plan_, internal::fftw_cast(src), internal::fftw_cast(dst));
        }

        inline void idft_scale(value_type* dst) {
            const auto scaling = static_cast<value_type>(nfft_);
            for (size_type i = 0; i < nfft_; ++i) {
                dst[i] /= scaling;
            }
        }

        inline void idft_scale(complex_type* dst) {
            const auto scaling = static_cast<value_type>(nfft_);
            for (size_type i = 0; i < nfft_; ++i) {
                dst[i] /= scaling;
            }
        }

        inline void idct_scale(value_type* dst) {
            const auto scaling = 2 * nfft_;
            for (size_type i = 0; i < nfft_; ++i) {
                dst[i] /= scaling;
            }
        }

    private:
        ::fftwf_plan plan_{nullptr};
        size_type nfft_;
    };

    template <>
    struct fftw_impl<double> {
        using value_type   = double;
        using complex_type = std::complex<double>;
        using size_type    = int;

        explicit fftw_impl(size_type nfft) : nfft_(nfft) {}

        ~fftw_impl() {
            if (!meta::is_null(plan_)) {
                fftw_destroy_plan(plan_);
            }
        }

        inline void dft(const complex_type* src, complex_type* dst) {
            if (meta::is_null(plan_))
                plan_ = fftw_plan_dft_1d(nfft_, internal::fftw_cast(src), internal::fftw_cast(dst), FFTW_FORWARD,
                                         FFTW_ESTIMATE | FFTW_PRESERVE_INPUT);
            fftw_execute_dft(plan_, internal::fftw_cast(src), internal::fftw_cast(dst));
        }

        inline void idft(const complex_type* src, complex_type* dst) {
            if (meta::is_null(plan_))
                plan_ = fftw_plan_dft_1d(nfft_, internal::fftw_cast(src), internal::fftw_cast(dst), FFTW_BACKWARD,
                                         FFTW_ESTIMATE | FFTW_PRESERVE_INPUT);
            fftw_execute_dft(plan_, internal::fftw_cast(src), internal::fftw_cast(dst));
        }

        inline void dft(const value_type* src, complex_type* dst) {
            if (meta::is_null(plan_))
                plan_ = fftw_plan_dft_r2c_1d(nfft_, internal::fftw_cast(src), internal::fftw_cast(dst),
                                             FFTW_ESTIMATE | FFTW_PRESERVE_INPUT);
            fftw_execute_dft_r2c(plan_, internal::fftw_cast(src), internal::fftw_cast(dst));
        }

        inline void idft(const complex_type* src, value_type* dst) {
            if (meta::is_null(plan_))
                plan_ = fftw_plan_dft_c2r_1d(nfft_, internal::fftw_cast(src), internal::fftw_cast(dst),
                                             FFTW_ESTIMATE | FFTW_PRESERVE_INPUT);
            fftw_execute_dft_c2r(plan_, internal::fftw_cast(src), internal::fftw_cast(dst));
        }

        inline void dht(const value_type* src, value_type* dst) {
            if (meta::is_null(plan_)) {
                plan_ = fftw_plan_r2r_1d(nfft_, internal::fftw_cast(src), internal::fftw_cast(dst), FFTW_DHT,
                                         FFTW_ESTIMATE | FFTW_PRESERVE_INPUT);
            }
            fftw_execute_r2r(plan_, internal::fftw_cast(src), internal::fftw_cast(dst));
        }

        inline void dct(const value_type* src, value_type* dst) {
            if (meta::is_null(plan_)) {
                plan_ = fftw_plan_r2r_1d(nfft_, internal::fftw_cast(src), internal::fftw_cast(dst), FFTW_REDFT10,
                                         FFTW_ESTIMATE | FFTW_PRESERVE_INPUT);
            }
            fftw_execute_r2r(plan_, internal::fftw_cast(src), internal::fftw_cast(dst));
        }

        inline void idct(const value_type* src, value_type* dst) {
            if (meta::is_null(plan_)) {
                plan_ = fftw_plan_r2r_1d(nfft_, internal::fftw_cast(src), internal::fftw_cast(dst), FFTW_REDFT01,
                                         FFTW_ESTIMATE | FFTW_PRESERVE_INPUT);
            }
            fftw_execute_r2r(plan_, internal::fftw_cast(src), internal::fftw_cast(dst));
        }

        inline void idft_scale(value_type* dst) {
            const auto scaling = static_cast<value_type>(nfft_);
            for (size_type i = 0; i < nfft_; ++i) {
                dst[i] /= scaling;
            }
        }

        inline void idft_scale(complex_type* dst) {
            const auto scaling = static_cast<value_type>(nfft_);
            for (size_type i = 0; i < nfft_; ++i) {
                dst[i] /= scaling;
            }
        }

        inline void idct_scale(value_type* dst) {
            const auto scaling = 2 * nfft_;
            for (size_type i = 0; i < nfft_; ++i) {
                dst[i] /= scaling;
            }
        }

    private:
        ::fftw_plan plan_{nullptr};
        size_type nfft_;
    };
}} // namespace edsp::spectral

#endif // EDSP_FFTW_IMPL_HPP
