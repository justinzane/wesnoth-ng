/*
 Copyright (C) 2008 - 2013 by David White <dave@whitevine.net>
 Part of the Battle for Wesnoth Project http://www.wesnoth.org/

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY.

 See the COPYING file for more details.
 */

#ifndef UNIT_ID_HPP_INCLUDED
#define UNIT_ID_HPP_INCLUDED

#include <ctime>
#include <cstdint>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/string_generator.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/noncopyable.hpp>
#include <boost/unordered_set.hpp>

class unit_id_manager: private boost::noncopyable {
    private:
        typedef boost::unordered_set<boost::uuids::uuid> uuid_set_t;
        /** An unordered set of uuids to hold those that are known to the manager. */
        uuid_set_t known_ids_;

        /** An intermediate in the conversion of uuids to strings. */
        std::stringstream ss_;

        /** Regenerates a uuid from a string representation thereof. */
        boost::uuids::string_generator strgen_;

        /** Generates an new random uuid. */
        boost::uuids::basic_random_generator<boost::mt19937> rndgen_;

        static unit_id_manager manager_;
        unit_id_manager();

        /** @brief converts a uuid to a string */
        std::string uuid2str(const boost::uuids::uuid& u) {
            known_ids_.insert(u);
            ss_ << u;
            return ss_.str();
        }

        /** @brief converts a string to a uuid */
        boost::uuids::uuid str2uuid(const std::string& s) {
            boost::uuids::uuid u;
            try {
                std::cout << "JZ Converted " << s << " to a uuid.\n";
                u = strgen_.operator ()(s);
            } catch (boost::exception&) {
                std::cout << "JZ Could not convert " << s << " to a uuid.\n";
                u = rndgen_.operator ()();
            }
            std::cout << "JZ Inserting " << u << "\n";
            known_ids_.insert(u);
            return u;
        }

    public:
        /** @brief creates and returns a new uuid. */
        std::string get_id(void) {
            boost::uuids::uuid u = rndgen_.operator ()();
            known_ids_.insert(u);

            return uuid2str(u);
        }

        /** @brief check if the ID manager knows of an id. */
        bool has_id(const std::string id) {
            boost::uuids::uuid u_ = str2uuid(id);
            if (known_ids_.find(u_) == known_ids_.end()) {
                std::cout << "JZ Checking ID: " << id << " --  " << "False\n";
                return false;
            } else {
                std::cout << "JZ Checking ID: " << id << " --  " << "True\n";
                return true;
            }
        }

        /** @brief check if the ID manager knows of an id. */
        boost::unordered_set<boost::uuids::uuid>::iterator find_id(const std::string id) {
            boost::uuids::uuid u_ = str2uuid(id);
            return known_ids_.find(u_);
        }

        /** @brief notifies the id manager of an ID. */
        void store_id(std::string id) {
            std::cout << "JZ Storing ID: " << id << "\n";
            known_ids_.insert(str2uuid(id));
        }

        /** @brief checks if a string is a uuid */
        bool is_valid_id(const std::string& s) {
            boost::uuids::uuid u;
            try {
                u = strgen_.operator ()(s);
                return true;
            } catch (boost::exception&) {
                return false;
            }
        }

        /** @brief Accessor for the singleton instance. */
        static unit_id_manager& instance();

};

#endif
