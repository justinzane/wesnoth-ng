/**
 * @file 		point_2d.hpp is part of wesnoth-ng
 * @author		Justin Zane Chudgar, <justin@justinzane.com>
 * @date		Dec 13, 2013
 * @copyright   Copyright justin, 2013
 * @section		License GPLv3
 * > wesnoth-ng is free software: you can redistribute it and/or modify
 * > it under the terms of the GNU General Public License as published by
 * > the Free Software Foundation, either version 3 of the License, or
 * > (at your option) any later version.
 * >
 * > wesnoth-ng is distributed in the hope that it will be useful,
 * > but WITHOUT ANY WARRANTY; without even the implied warranty of
 * > MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * > GNU General Public License for more details.
 * >
 * > You should have received a copy of the GNU General Public License
 * > along with wesnoth-ng.  If not, see http://www.gnu.org/licenses/ .
 * @brief		TODO WRITEME
 * @details		TODO WRITEME
 */

#ifndef SDL2_POINT_HPP_
#define SDL2_POINT_HPP_

#include <math.h>
#include <utility>
#include <stdlib.h>

class point_2Df;

/** @class point_2Dd @brief geometric two dimensional point, integer valued */
class point_2Dd {
    public:
        point_2Dd() {}
        point_2Dd(point_2Df fp) {x_ = (int)fp.get_x(); y_ = (int)fp.get_y(); }
        point_2Dd(int x, int y) { x_ = x; y_ = y; }
        point_2Dd(std::pair<int, int> p) { x_ = p.first; y_ = p.second; }

        ~point_2Dd() {}

        int get_x() { return x_; }
        int get_y() { return y_; }
        std::pair<int, int> get() { return new std::pair<int, int>(x_, y_); }

        void set_x(int x) {x_ = x;}
        void set_y(int y) {y_ = y;}
        void set(std::pair<int, int> p) { x_ = p.first; y_ = p.second; }

        bool operator==(point_2Dd& comp) { return (this->x_ == comp.x_ && this->y_ == comp.y_); }

    private:
        int x_ = {0};
        int y_ = {0};
};

/** @class point_2Df @brief geometric two dimensional point, float valued */
class point_2Df {
    public:
        point_2Df() {}
        point_2Df(point_2Dd ip) { x_ = (float)ip.get_x(); y_ = (float)ip.get_y(); }
        point_2Df(float x, float y) { x_ = x; y_ = y; }

        point_2Df(float angle, float mag, bool is_degrees) {
            float radians;
            if (is_degrees) {
                radians = angle * M_PI / 180.0f;
            } else {
                radians = angle;
            }
            x_ = cos(radians) * mag; y_ = sin(radians) * mag;
        }
        point_2Df(std::pair<float, float> p) { x_ = p.first; y_ = p.second; }

        ~point_2Df() {}

        float get_x() { return x_; }
        float get_y() { return y_; }
        std::pair<float, float> get() {
            std::pair<float, float> ret {x_, y_};
            return ret;
        }

        void set_x(float x) {x_ = x;}
        void set_y(float y) {y_ = y;}
        void set(std::pair<float, float> p) { x_ = p.first; y_ = p.second; }

        bool operator==(point_2Df& comp) { return (this->x_ == comp.x_ && this->y_ == comp.y_); }

        float angle_rad() { return atan2(y_, x_); }
        float angle_deg() { return atan2(y_, x_) * 180.0 / M_PI; }
        float magnitude() { return sqrt(pow(x_, 2) + pow(y_, 2)); }

    private:
        float x_ = {0};
        float y_ = {0};
};

/** @class point_2Dd @brief geometric two dimensional point, integer valued */
class point_3Dd {
    public:
        point_3Dd() {}
        point_3Dd(point_3Df fp) {x_ = (int)fp.get_x(); y_ = (int)fp.get_y(); }
        point_3Dd(int x, int y) { x_ = x; y_ = y; }
        point_3Dd(std::pair<int, int> p) { x_ = p.first; y_ = p.second; }

        ~point_3Dd() {}

        int get_x() { return x_; }
        int get_y() { return y_; }
        std::pair<int, int> get() { return new std::pair<int, int>(x_, y_); }

        void set_x(int x) {x_ = x;}
        void set_y(int y) {y_ = y;}
        void set(std::pair<int, int> p) { x_ = p.first; y_ = p.second; }

        bool operator==(point_3Dd& comp) { return (this->x_ == comp.x_ && this->y_ == comp.y_); }

    private:
        int x_ = {0};
        int y_ = {0};
};

/** @class point_3Df @brief geometric two dimensional point, float valued */
class point_3Df {
    public:
        point_3Df() {}
        point_3Df(point_3Dd ip) { x_ = (float)ip.get_x(); y_ = (float)ip.get_y(); }
        point_3Df(float x, float y) { x_ = x; y_ = y; }
        point_3Df(float angle, float mag) { x_ = cos(angle) * mag; y_ = sin(angle) * mag; }
        point_3Df(std::pair<float, float> p) { x_ = p.first; y_ = p.second; }

        ~point_3Df() {}

        float get_x() { return x_; }
        float get_y() { return y_; }
        std::pair<float, float> get() { return new std::pair<float, float>(x_, y_); }

        void set_x(float x) {x_ = x;}
        void set_y(float y) {y_ = y;}
        void set(std::pair<float, float> p) { x_ = p.first; y_ = p.second; }

        bool operator==(point_3Df& comp) { return (this->x_ == comp.x_ && this->y_ == comp.y_); }

        float angle_rad() { return atan2(y_, x_); }
        float angle_deg() { return atan2(y_, x_) * 180.0 / M_PI; }
        float magnitude() { return sqrt(x_^2 + y_^2); }

    private:
        float x_ = {0};
        float y_ = {0};
};


#endif /* SDL2_POINT_HPP_ */
