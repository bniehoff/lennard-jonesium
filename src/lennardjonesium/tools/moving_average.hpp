/**
 * moving_average.hpp
 * 
 * Copyright (c) 2021-2022 Benjamin E. Niehoff
 * 
 * This file is part of Lennard-Jonesium.
 * 
 * Lennard-Jonesium is free software: you can redistribute
 * it and/or modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation, either
 * version 3 of the License, or (at your option) any later version.
 * 
 * Lennard-Jonesium is distributed in the hope that it will
 * be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public
 * License along with Lennard-Jonesium.  If not, see
 * <https://www.gnu.org/licenses/>.
 */

#ifndef LJ_MOVING_AVERAGE_HPP
#define LJ_MOVING_AVERAGE_HPP

#include <cassert>
#include <memory>
#include <numeric>

#include <boost/circular_buffer.hpp>

namespace tools
{
    template<class T, class Alloc = std::allocator<T>>
    class MovingAverage {
        /**
         * MovingAverage keeps track of the moving average of some quantity over time.  It puts
         * values into a FIFO queue of some fixed depth, and computes their average when requested.
         * It also informs the caller whether the queue has been filled or not, and provides a
         * method to empty it and restart averaging.
         * 
         * MovingAverage acts a bit like an STL container, but only exposes a minimal set of
         * methods, and adds the methods sum() and average().
         * 
         * In order to compute the sum and average, we assume the "zero" of type T is the value
         * given by default construction.  If it is not, then an appropriate zero value can be
         * passed to the constructor.  (Unfortunately we can't do this with a non-type template
         * parameter, because Eigen types are not structural types.)
         */

        public:
            MovingAverage(int size, T zero = T{}) : buffer_(size), zero_{zero} {}

            void push_back(T entry) {buffer_.push_back(entry);}

            size_t size() {return buffer_.size();}

            size_t capacity() {return buffer_.capacity();}

            void clear() {buffer_.clear();}

            bool empty() {return buffer_.empty();}

            bool full() {return buffer_.full();}

            T sum() {return std::accumulate(buffer_.begin(), buffer_.end(), zero_);}

            T average()
            {
                assert(size() > 0 && "Cannot compute average of empty MovingAverage");

                return sum() / static_cast<double>(size());
            }
        
        private:
            boost::circular_buffer<T, Alloc> buffer_;
            T zero_;
    };
} // namespace tools


#endif
