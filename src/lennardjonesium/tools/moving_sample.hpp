/**
 * moving_sample.hpp
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

#ifndef LJ_MOVING_SAMPLE_HPP
#define LJ_MOVING_SAMPLE_HPP

#include <cassert>
#include <memory>
#include <numeric>

#include <boost/circular_buffer.hpp>

#include <Eigen/Dense>

namespace detail
{
    template<class T, class Alloc = std::allocator<T>>
    class MovingSampleBase
    {
        public:
            void push_back(T value) {buffer_.push_back(value);}

            size_t size() {return buffer_.size();}

            size_t capacity() {return buffer_.capacity();}

            void clear() {buffer_.clear();}

            bool empty() {return buffer_.empty();}

            bool full() {return buffer_.full();}
        
        protected:
            explicit MovingSampleBase(int size) : buffer_(size) {}

            boost::circular_buffer<T, Alloc> buffer_;
    };
} // namespace detail


namespace tools
{
    /**
     * MovingSample keeps a fixed-sized sample of a quantity over time, used for computing
     * estimates of its statistics.  It does this by pushing values into a FIFO queue of fixed
     * length.  When requested, it computes statistics on these values (mean, variance).  These
     * statistics are sample estimates.
     * 
     * We also provide a means for clearing and restarting the queue, as well as for testing
     * whether it is completely full.
     * 
     * MovingSample acts a bit like an STL container, but only exposes a minimal set of
     * methods, and adds the method statistics().  For simple template parameters like <double>,
     * the mean and variance are the same type.  However, for Eigen::Vector types, we will want
     * to comptute a covariance matrix, which is a different type.
     * 
     * Because of this specialization for Eigen::Vector types, we split some of the shared
     * functionality into a MovingSampleBase template class, which allows us to specialize only
     * the parts of the interface that change.  We do not actually need to use CRTP, however.
     */

    template<class T, class Alloc = std::allocator<T>>
    class MovingSample : public detail::MovingSampleBase<T, Alloc>
    {
        /**
         * This unspecialized version of MovingSample will work as long as T is an ordinary
         * numeric type.  Note that it may produce odd results if T is an integral type as a result
         * of integer division!
         */

        using mean_type = T;
        using variance_type = T;

        // The sample size is cast into this type in the denominator of mean/variance expressions.
        using denominator_type = T;
        
        public:
            explicit MovingSample(int size)
                : detail::MovingSampleBase<T, Alloc>::MovingSampleBase(size)
            {}

            struct Statistics
            {
                const mean_type mean;
                const variance_type variance;
            };

            Statistics statistics()
            {
                assert(this->size() > 1 && "Cannot compute statistics without at least 2 samples");

                constexpr mean_type initial_mean{};
                constexpr variance_type initial_variance{};

                // Compute the mean
                mean_type mean = (
                    std::reduce(this->buffer_.begin(), this->buffer_.end(), initial_mean)
                    / static_cast<denominator_type>(this->size())
                );

                // Compute the variance (using Bessel's correction for sample variance)
                variance_type variance = (
                    std::transform_reduce(
                        this->buffer_.begin(), this->buffer_.end(), initial_variance,
                        std::plus<variance_type>(),
                        [mean](mean_type value) -> variance_type {
                            return (value - mean) * (value - mean);
                        })
                    / static_cast<denominator_type>(this->size() - 1)
                );

                return Statistics{mean, variance};
            }
    };

    template<class Scalar, int Size, class Alloc>
    class MovingSample<Eigen::Vector<Scalar, Size>, Alloc>
        : public detail::MovingSampleBase<Eigen::Vector<Scalar, Size>, Alloc>
    {
        /**
         * This specialization should work for Eigen::Vector types.  In place of a variance value
         * for a single sampled quantity, the Statistics structure has a covariance matrix for the
         * sampled Eigen::Vector quantity.
         */

        using mean_type = Eigen::Vector<Scalar, Size>;
        using covariance_type = Eigen::Matrix<Scalar, Size, Size>;

        // The sample size is cast into this type in the denominator of mean/variance expressions.
        using denominator_type = Scalar;

        public:
            explicit MovingSample(int size)
                : detail::MovingSampleBase<mean_type, Alloc>::MovingSampleBase(size)
            {}

            struct Statistics
            {
                const mean_type mean;
                const covariance_type covariance;
            };

            Statistics statistics()
            {
                assert(this->size() > 1 && "Cannot compute statistics without at least 2 samples");

                const mean_type initial_mean = mean_type::Zero();
                const covariance_type initial_covariance = covariance_type::Zero();

                // Compute the mean
                mean_type mean = (
                    std::reduce(this->buffer_.begin(), this->buffer_.end(), initial_mean)
                    / static_cast<denominator_type>(this->size())
                );

                // Compute the covariance matrix (using Bessel's correction for sample variance)
                using function_argument_type = const Eigen::Ref<const mean_type>&;
                covariance_type covariance = (
                    std::transform_reduce(
                        this->buffer_.begin(), this->buffer_.end(), initial_covariance,
                        std::plus<covariance_type>(),
                        [&mean](function_argument_type value) -> covariance_type {
                            return (value - mean) * (value - mean).transpose();
                        })
                    / static_cast<denominator_type>(this->size() - 1)
                );

                return Statistics{mean, covariance};
            }
    };
} // namespace tools


#endif
