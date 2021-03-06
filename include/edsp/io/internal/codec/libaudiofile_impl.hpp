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
* Filename: audiofile_impl.hpp
* Author: Mohammed Boujemaoui
* Date: 07/10/18
*/

#ifndef EDSP_AUDIOFILE_IMPL_HPP
#define EDSP_AUDIOFILE_IMPL_HPP

#include <edsp/core/logger.hpp>
#include <edsp/meta/is_signed.hpp>
#include <edsp/meta/advance.hpp>
#include <edsp/meta/iterator.hpp>
#include <edsp/meta/data.hpp>
#include <audiofile.h>
#include <cmath>

namespace edsp { namespace io {

    template <typename T, std::size_t N = 2048>
    struct libaudiofile_decoder {
        static_assert(std::is_arithmetic<T>::value, "Expected arithmetic types");

        using index_type = std::ptrdiff_t;
        using value_type = T;

        libaudiofile_decoder() = default;
        ~libaudiofile_decoder();

        void close();

        bool open(const edsp::string_view& filepath);

        bool is_open() const noexcept;

        index_type samples() const noexcept;

        index_type frames() const noexcept;

        index_type channels() const noexcept;

        double duration() const noexcept;

        double samplerate() const noexcept;

        index_type seek(index_type position) noexcept;

        bool seekable() const noexcept;

        index_type current() const noexcept;

        template <typename OutputIt>
        index_type read(OutputIt first, OutputIt last);

    private:
        void update_format();

    private:
        /* Internal buffer used to read data */
        std::array<T, N> buffer_{};

        /* Number of channels */
        index_type channels_{0};

        /* File descriptor */
        AFfilehandle file_{AF_NULL_FILEHANDLE};

        /* Number of available frames */
        index_type frames_{0};

        /* Size of a frame in bytes */
        float frame_size_{0};

        /* Sample rate of the streaming */
        double samplerate_{0};
    };

    template <typename T, size_t N>
    typename libaudiofile_decoder<T, N>::index_type libaudiofile_decoder<T, N>::current() const noexcept {
        return static_cast<index_type>(afTellFrame(file_, AF_DEFAULT_TRACK));
    }

    template <typename T, size_t N>
    typename libaudiofile_decoder<T, N>::index_type
        libaudiofile_decoder<T, N>::seek(libaudiofile_decoder::index_type position) noexcept {
        return afSeekFrame(file_, AF_DEFAULT_TRACK, position);
    }

    template <typename T, size_t N>
    double libaudiofile_decoder<T, N>::samplerate() const noexcept {
        return samplerate_;
    }

    template <typename T, size_t N>
    double libaudiofile_decoder<T, N>::duration() const noexcept {
        return static_cast<double>(frames_) / samplerate_;
    }

    template <typename T, size_t N>
    typename libaudiofile_decoder<T, N>::index_type libaudiofile_decoder<T, N>::channels() const noexcept {
        return channels_;
    }

    template <typename T, size_t N>
    typename libaudiofile_decoder<T, N>::index_type libaudiofile_decoder<T, N>::frames() const noexcept {
        return frames_;
    }

    template <typename T, size_t N>
    typename libaudiofile_decoder<T, N>::index_type libaudiofile_decoder<T, N>::samples() const noexcept {
        return channels_ * frames_;
    }

    template <typename T, size_t N>
    bool libaudiofile_decoder<T, N>::is_open() const noexcept {
        return file_ != AF_NULL_FILEHANDLE;
    }

    template <typename T, size_t N>
    bool libaudiofile_decoder<T, N>::open(const edsp::string_view& filepath) {
        close();
        file_ = afOpenFile(filepath.data(), "r", NULL);
        if (file_ == AF_NULL_FILEHANDLE) {
            eWarning() << "Could not open file " << filepath;
            return false;
        }

        frames_     = static_cast<index_type>(afGetFrameCount(file_, AF_DEFAULT_TRACK));
        channels_   = static_cast<index_type>(afGetVirtualChannels(file_, AF_DEFAULT_TRACK));
        samplerate_ = afGetRate(file_, AF_DEFAULT_TRACK);
        update_format();
        return true;
    }

    template <typename T, size_t N>
    void libaudiofile_decoder<T, N>::close() {
        if (!is_open()) {
            return;
        }

        afCloseFile(file_);
        file_       = AF_NULL_FILEHANDLE;
        channels_   = 0;
        frames_     = 0;
        samplerate_ = 0;
        frame_size_ = 0;
    }

    template <typename T, size_t N>
    void libaudiofile_decoder<T, N>::update_format() {
        if (std::is_floating_point<T>::value) {
            if (std::is_same<T, float>::value) {
                afSetVirtualSampleFormat(file_, AF_DEFAULT_TRACK, AF_SAMPFMT_FLOAT, sizeof(T) * 8);
            } else {
                afSetVirtualSampleFormat(file_, AF_DEFAULT_TRACK, AF_SAMPFMT_DOUBLE, sizeof(T) * 8);
            }
        } else if (std::is_integral<T>::value) {
            if (meta::is_signed<T>::value) {
                afSetVirtualSampleFormat(file_, AF_DEFAULT_TRACK, AF_SAMPFMT_TWOSCOMP, sizeof(T) * 8);
            } else {
                afSetVirtualSampleFormat(file_, AF_DEFAULT_TRACK, AF_SAMPFMT_UNSIGNED, sizeof(T) * 8);
            }
        }

        frame_size_ = afGetVirtualFrameSize(file_, AF_DEFAULT_TRACK, 1);
    }

    template <typename T, size_t N>
    template <typename OutputIt>
    typename libaudiofile_decoder<T, N>::index_type libaudiofile_decoder<T, N>::read(OutputIt first, OutputIt last) {
        index_type total        = std::distance(first, last);
        index_type remaining    = total;
        index_type samples_read = 0;
        do {
            const auto expected_samples = (remaining >= N) ? N : remaining;
            const auto expected_frames  = std::trunc(expected_samples / channels_);
            const auto frames_read = afReadFrames(file_, AF_DEFAULT_TRACK, meta::data(buffer_), (int) expected_frames);
            samples_read           = frames_read * channels_;
            remaining -= samples_read;
            for (auto i = 0; i < samples_read; ++i, ++first) {
                *first = buffer_[i];
            }
        } while (remaining > 0 && samples_read > 0);
        return total - remaining;
    }

    template <typename T, size_t N>
    libaudiofile_decoder<T, N>::~libaudiofile_decoder() {
        close();
    }

    template <typename T, size_t N>
    bool libaudiofile_decoder<T, N>::seekable() const noexcept {
        return true;
    }

}} // namespace edsp::io

#endif //EDSP_AUDIOFILE_IMPL_HPP
