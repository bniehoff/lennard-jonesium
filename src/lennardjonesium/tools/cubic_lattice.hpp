/**
 * cubic_lattice.hpp
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

#ifndef LJ_CUBIC_LATTICE_HPP
#define LJ_CUBIC_LATTICE_HPP

#include <type_traits>

#include <Eigen/Dense>

#include <lennardjonesium/draft_cpp23/generator.hpp>
#include <lennardjonesium/tools/bounding_box.hpp>

namespace tools
{
    class CubicLattice
    {
        /**
         * CubicLattice provides the coordinates of points in a cubic lattice, at the requested
         * density.  It also computes the BoundingBox of the occupied volume.  There will be
         * derived classes which implement simple, body-centered, and face-centered cubic lattices.
         */

        public:
            struct UnitCell
            {
                /**
                 * The UnitCell will determine what sort of cubic lattice we are talking about.
                 * It is determined by an Eigen matrix whose columns are the lattice sites, scaled
                 * to fit within a standard 1x1x1 cube.
                 */

                const Eigen::Matrix4Xd lattice_sites;

                /**
                 * The "density" is just the number of lattice sites in the cell.  We use it as
                 * either int or double, depending on context.
                 */
                template <typename T> requires std::is_arithmetic_v<T>
                auto density() {return static_cast<T>(lattice_sites.cols());}
            };

            // The following static methods create the three basic cubic lattices
            inline static UnitCell Simple() {return UnitCell{
                Eigen::MatrixX4d{{0.0, 0.0, 0.0, 0.0}}.transpose()
            };}

            inline static UnitCell BodyCentered() {return UnitCell{
                Eigen::MatrixX4d{{0.0, 0.0, 0.0, 0.0}, {0.5, 0.5, 0.5, 0.0}}.transpose()
            };}

            inline static UnitCell FaceCentered() {return UnitCell{
                Eigen::MatrixX4d{
                    {0.0, 0.0, 0.0, 0.0},
                    {0.5, 0.5, 0.0, 0.0},
                    {0.5, 0.0, 0.5, 0.0},
                    {0.0, 0.5, 0.5, 0.0}
                }.transpose()
            };}

            // The constructor takes a unit cell type to fully specify the cubic lattice
            CubicLattice(int particle_count, double density, UnitCell unit_cell = FaceCentered());

            /**
             * The unit cells will be enumerated at coordinates that fit inside the smallest
             * possible cube.
             */
            std::generator<Eigen::Vector4d> operator() ();

            BoundingBox bounding_box()
                {return BoundingBox(static_cast<double>(cells_per_side_) * scale_);}
        
        private:
            UnitCell unit_cell_;
            double scale_;
            int particle_count_;
            int cells_per_side_;
            
    };
} // namespace tools


#endif
