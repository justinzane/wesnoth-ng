/**
 * @file src/hash.hpp
 * @project The Battle for Wesnoth NG - https://github.com/justinzane/wesnoth-ng
 * @brief 
 * @authors 
 * @copyright Copyright (C) 2008 - 2013 by Thomas Baumhauer <thomas.baumhauer@NOSPAMgmail.com>
 Part of the Battle for Wesnoth Project http://www.wesnoth.org/

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY.

 See the COPYING file for more details.
 */

#ifndef HASH_HPP_INCLUDED
#define HASH_HPP_INCLUDED

#include <gcrypt.h>
#include <stdlib.h>
#include <time.h>
#include <string>

/** @def GCRYPT_NO_DEPRECATED
 *  @link http://www.gnupg.org/documentation/manuals/gcrypt/Header.html#Header */
#ifndef GCRYPT_NO_DEPRECATED
#define GCRYPT_NO_DEPRECATED
#endif

namespace util {

/**
 * Creates a random 16 byte salt.
 * @return salt as a string
 * @todo Use a better rng. 2013-11-08 justinzane
 */
inline std::string get_salt() {
    srand(time(NULL));
    char rnd_c[16];
    for (int i = 0; i < 16; i++) {
        rnd_c[i] = (rand() % 10);
    }
    return (std::string)rnd_c;
}

/**
 * @brief Returns the hash digest for a string as a string.
 * @note Currently using SHA1 as hash function.
 */
inline std::string get_hash(const std::string& text) {
    std::string digest;
    gcry_md_hash_buffer(GCRY_MD_SHA1,
                        static_cast<void*>(&digest),
                        static_cast<const void*>(&text),
                        text.length());
    return digest;
};

}  // namespace util

#endif // HASH_HPP_INCLUDED
