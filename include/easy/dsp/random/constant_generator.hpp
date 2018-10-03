/*
 * EasyDSP, A cross-platform Digital Signal Processing library written in modern C++.
 * Copyright (C) 2018 Mohammed Boujemaoui Boulaghmoudi
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
 * Filename: pink_noise.hpp
 * Author: Mohammed Boujemaoui
 * Date: 01/08/2018
 */
#ifndef EASYDSP_CONSTANT_GENERATOR_HPP
#define EASYDSP_CONSTANT_GENERATOR_HPP

namespace easy { namespace dsp { namespace random {

    /**
     * @class constant_generator
     * @brief This class implements a constant generator.
     */
    template <typename T>
    struct constant_generator {
        using value_type = T;

        /**
         * @brief Creates a random generator.
         * @param value Constant number to be generated.
         */
        explicit constant_generator(const value_type& value) : generator_(value) {}

        /**
         * @brief Generates a constant number.
         * @return The generated constant number.
         */
        value_type operator()() {
            return generator_;
        }

    private:
        T generator_;
    };

}}} // namespace easy::dsp::random

#endif // EASYDSP_CONSTANT_GENERATOR_HPP
