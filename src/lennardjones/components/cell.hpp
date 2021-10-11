/**
 * cell.hpp
 * 
 * Copyright (c) 2021 Benjamin E. Niehoff
 * 
 * This file is part of Lennard-Jones-Particles.
 * 
 * Lennard-Jones-Particles is free software: you can redistribute
 * it and/or modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation, either
 * version 3 of the License, or (at your option) any later version.
 * 
 * Lennard-Jones-Particles is distributed in the hope that it will
 * be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public
 * License along with Lennard-Jones-Particles.  If not, see
 * <https://www.gnu.org/licenses/>.
 */

#ifndef LJ_CELL_HPP
#define LJ_CELL_HPP

#include <list>

#include <lennardjones/components/particle.hpp>

namespace components {
    class Cell {
        private:
            /**
             * The Particles will be kept in a master std::vector elsewhere, we just point to them
             * here.  This allows their individual identities to remain intact, and lets us track
             * their individual trajectories.
             */
            std::list<Particle*> particles_;
        
        public:
            void append(Particle*);

            /**
             * TODO: How do we handle transferring Particles between Cells?
             * 
             * We need to "mark" items for removal while calculating the new positions. This
             * requires a std::list iterator.  The std::list is private, so this shouldn't be
             * visible outside the Cell.
             * 
             * When the item is removed, we need to extract the Particle* pointer itself (or we
             * can just refer to the Particle by its ID?).  It can then be appended to another Cell
             * like normal.
             */

            /**
             * A Cell will just be created empty, which means there is no need to override the 
             * default constructor, assignment operator, etc.
             */
    };
}

#endif
