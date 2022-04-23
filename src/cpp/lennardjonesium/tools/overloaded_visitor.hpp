/**
 * overloaded_visitor.hpp
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

#ifndef LJ_OVERLOADED_VISITOR_HPP
#define LJ_OVERLOADED_VISITOR_HPP

namespace tools
{
    /**
     * This is just a template for creating overloaded function objects to use with std::variant
     * and std::visit.  Rather than copy-paste it into files, it made sense to define it in a
     * central place.  When instantiated from a list of lambdas, it creates a function object
     * whose operator() is overloaded to dispatch to each of the given lambdas:
     * 
     *  auto visitor = OverloadedVisitor
     *  {
     *      [](int arg) {something_with_int(arg);},
     *      [](float arg) {something_with_float(arg)},
     *      etc.
     *  };
     * 
     * The lambdas should have different argument types so that this overloading is possible.
     */
    template<class... Lambdas>
    struct OverloadedVisitor : Lambdas... { using Lambdas::operator()...; };
} // namespace tools


#endif
