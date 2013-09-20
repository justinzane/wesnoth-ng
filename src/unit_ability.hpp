/*
 * unit_ability.hpp
 *
 *  Created on: Sep 19, 2013
 *      Author: justin
 */

#ifndef UNIT_ABILITY_HPP_
#define UNIT_ABILITY_HPP_

/// The things contained within a unit_ability_list.
typedef std::pair<const config *, map_location> unit_ability;

/**
 * A container for holding unit abilities.
 * TODO: EDITME
 */
class unit_ability_list
{
    public:
        /** Typedefs =========================================================================*/

        /**
         * 2-tuples for storing TODO: WRITEME
         * Implemented in unit_abilities.cpp
         */
        std::pair<int, map_location> highest(const std::string& key, int def = 0) const;
        std::pair<int, map_location> lowest(const std::string& key, int def = 0) const;

        /**
         * The following make this class usable with BOOST_FOREACH:
         */
        typedef std::vector<unit_ability>::iterator iterator;
        typedef std::vector<unit_ability>::const_iterator const_iterator;

        /** Functions =========================================================================*/

        /** TODO: WRITEME */
        unit_ability_list() : cfgs_() {}

        /** TODO: WRITEME */
        iterator begin() {
            return cfgs_.begin();
        }

        /** TODO: WRITEME */
        const_iterator begin() const {
            return cfgs_.begin();
        }

        /** TODO: WRITEME */
        iterator end() {
            return cfgs_.end();
        }

        /** TODO: WRITEME */
        const_iterator end() const {
            return cfgs_.end();
        }

        /** TODO: WRITEME */
        bool empty() const {
            return cfgs_.empty();
        }

        /** TODO: WRITEME */
        unit_ability & front() {
            return cfgs_.front();
        }

        /** TODO: WRITEME */
        const unit_ability & front() const {
            return cfgs_.front();
        }

        /** TODO: WRITEME */
        unit_ability & back() {
            return cfgs_.back();
        }

        /** TODO: WRITEME */
        const unit_ability & back() const {
            return cfgs_.back();
        }

        /** TODO: WRITEME */
        iterator erase(const iterator & erase_it) {
            return cfgs_.erase(erase_it);
        }

        /** TODO: WRITEME */
        void push_back(const unit_ability & ability) {
            cfgs_.push_back(ability);
        }

    private:
        // Data:
        std::vector<unit_ability> cfgs_;
};

#endif /* UNIT_ABILITY_HPP_ */
