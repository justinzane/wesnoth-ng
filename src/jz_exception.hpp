/**
 * @file 		jz_exception.hpp is part of Project wesnoth-old
 * @author		justin
 * @date		Nov 2, 2013
 * @copyright   Copyright justin, Nov 2, 2013
 * @section		<license> License
 * > wesnoth-old is free software: you can redistribute it and/or modify
 * > it under the terms of the GNU General Public License as published by
 * > the Free Software Foundation, either version 3 of the License, or
 * > (at your option) any later version.
 * >
 * > wesnoth-old is distributed in the hope that it will be useful,
 * > but WITHOUT ANY WARRANTY; without even the implied warranty of
 * > MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * > GNU General Public License for more details.
 * >
 * > You should have received a copy of the GNU General Public License
 * > along with wesnoth-old.  If not, see <a href="http://www.gnu.org/licenses/">
 * > http://www.gnu.org/licenses/</a>.
 * @brief		TODO WRITEME
 * @details		TODO WRITEME
 */
#ifndef JZ_EXCEPTION_HPP_
#define JZ_EXCEPTION_HPP_

#include <exception>

namespace imgops {

/**  @type imgops::jz_exception TODO: WRITEME */
class jz_exception: public std::exception {
    public:
        jz_exception();
        virtual ~jz_exception();
};

} /* namespace imgops */

#endif /* JZ_EXCEPTION_HPP_ */
