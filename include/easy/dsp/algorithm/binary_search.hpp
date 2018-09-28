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
 * Filename: binary_search.hpp
 * Author: Mohammed Boujemaoui
 * Date: 1/9/2018
 */
#ifndef EASYDSP_BINARY_SEARCH_HPP
#define EASYDSP_BINARY_SEARCH_HPP

#include <algorithm>
#include <easy/meta/iterator.hpp>

namespace easy { namespace dsp { inline namespace algorithm {

    /**
     * @brief Checks if an element equivalent to value appears within the range [first, last).
     *
     * For binary_search to succeed, the range [first, last) must be ordered.
     *
     * @param first Forward iterator defining the begin of the range to examine.
     * @param last Forward iterator defining the end of the range to examine.
     * @param value Value to compare the elements to.
     * @returns Iterator pointing to the first element that is equal than value, or last if no such element is found.
     */
    template <typename ForwardIt>
    constexpr ForwardIt binary_search(ForwardIt first, ForwardIt last, const value_type_t<ForwardIt>& value) {
        const auto it = std::lower_bound(first, last, value);
        return (it != last && (value == *it)) ? it : last;
    }

    /**
     * @brief Checks if an element equivalent to value appears within the range [first, last).
     *
     * For binary_search to succeed, the range [first, last) must be ordered.
     *
     * @param first Forward iterator defining the begin of the range to examine.
     * @param last Forward iterator defining the end of the range to examine.
     * @param value Value to compare the elements to.
     * @param comp Binary predicate which returns ​true if the first argument is less than the second.
     * @returns Iterator pointing to the first element that is equal than value, or last if no such element is found.
     */
    template <typename ForwardIt, class Compare>
    constexpr ForwardIt binary_search(ForwardIt first, ForwardIt last, const value_type_t<ForwardIt>& value, Compare comp) {
        const auto it = std::lower_bound(first, last, value, comp);
        return (it != last && (value == *it)) ? it : last;
    }


}}} // namespace easy::dsp

#endif // EASYDSP_BINARY_SEARCH_HPP
