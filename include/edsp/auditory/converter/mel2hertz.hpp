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
* Filename: mel2hertz.hpp
* Author: Mohammed Boujemaoui
* Date: 12/10/18
*/

#ifndef EDSP_MEL2HERTZ_HPP
#define EDSP_MEL2HERTZ_HPP

#include <edsp/auditory/converter/hertz2mel.hpp>
namespace edsp { namespace auditory { inline namespace converter {

    namespace internal {

        template <mel_base scale>
        struct inverter {};

        template <>
        struct inverter<mel_base::base_e> {
            template <typename T>
            constexpr T operator()(T mel) noexcept {
                return 700.0 * (std::exp(mel / 1127.01048) - 1.0);
            }
        };

        template <>
        struct inverter<mel_base::base_10> {
            template <typename T>
            constexpr T operator()(T mel) noexcept {
                return 700.0 * (std::pow(10.0, mel / 2595.0) - 1.0);
            }
        };
    } // namespace internal

    /**
     * @brief Converts a frequency in mels to Hertz.
     *
     * Depending of the base, the corresponding inverse expressions are:
     * \f[
     * {\displaystyle f=700\left(10^{\frac {m}{2595}}-1\right)=700\left(e^{\frac {m}{1127}}-1\right)}
     * \f]
     *
     * @param mel Frequency in mels.
     * @returns Frequency in Hz.
     * @see hertz2mel
     */
    template <mel_base scale, typename T>
    constexpr T mel2hertz(T mel) noexcept {
        return internal::inverter<scale>{}(mel);
    }

}}} // namespace edsp::auditory::converter

#endif //EDSP_MEL2HERTZ_HPP
