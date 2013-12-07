/**
 * @file src/events.cpp
 * @project The Battle for Wesnoth NG - https://github.com/justinzane/wesnoth-ng
 * @brief 
 * @authors 
 * @copyright Copyright (C) 2003 - 2013 by David White <dave@whitevine.net>
 Part of the Battle for Wesnoth Project http://www.wesnoth.org/

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY.

 See the COPYING file for more details.
 */

#include "global.hpp"

#include "clipboard.hpp"
#include "cursor.hpp"
#include "events.hpp"
#include "log/log.hpp"
#include "log/log.hpp"
#include "preferences_display.hpp"
#include "sound.hpp"
#include "sld2/sdl2_rndr_mgr.hpp"
#if defined _WIN32
#include "windows_tray_notification.hpp"
#endif

#include "SDL2/SDL.h"

#include <algorithm>
#include <cassert>
#include <deque>
#include <utility>
#include <vector>

#define ERR_GEN LOG_STREAM(err, lg::general)

namespace events {

namespace {

struct context {
        context() :
            handlers(), focused_handler( -1) {
        }

        void add_handler(handler* ptr);bool remove_handler(handler* ptr);
        int cycle_focus();
        void set_focus(const handler* ptr);

        std::vector<handler*> handlers;
        int focused_handler;

        void delete_handler_index(size_t handler);
};

void context::add_handler(handler* ptr) {
    handlers.push_back(ptr);
}

void context::delete_handler_index(size_t handler) {
    if (focused_handler == static_cast<int>(handler)) {
        focused_handler = -1;
    } else if (focused_handler > static_cast<int>(handler)) {
        --focused_handler;
    }

    handlers.erase(handlers.begin() + handler);
}

bool context::remove_handler(handler* ptr) {
    if (handlers.empty()) {
        return false;
    }

    static int depth = 0;
    ++depth;

    //the handler is most likely on the back of the events array,
    //so look there first, otherwise do a complete search.
    if (handlers.back() == ptr) {
        delete_handler_index(handlers.size() - 1);
    } else {
        const std::vector<handler*>::iterator i = std::find(handlers.begin(), handlers.end(),
                                                            ptr);
        if (i != handlers.end()) {
            delete_handler_index(i - handlers.begin());
        } else {
            return false;
        }
    }

    --depth;

    if (depth == 0) {
        cycle_focus();
    } else {
        focused_handler = -1;
    }

    return true;
}

int context::cycle_focus() {
    int index = focused_handler + 1;
    for (size_t i = 0; i != handlers.size(); ++i) {
        if (size_t(index) == handlers.size()) {
            index = 0;
        }

        if (handlers[size_t(index)]->requires_event_focus()) {
            focused_handler = index;
            break;
        }
    }

    return focused_handler;
}

void context::set_focus(const handler* ptr) {
    const std::vector<handler*>::const_iterator i = std::find(handlers.begin(), handlers.end(),
                                                              ptr);
    if (i != handlers.end() && ( * *i).requires_event_focus()) {
        focused_handler = int(i - handlers.begin());
    }
}

//this object stores all the event handlers. It is a stack of event 'contexts'.
//a new event context is created when e.g. a modal dialog is opened, and then
//closed when that dialog is closed. Each context contains a list of the handlers
//in that context. The current context is the one on the top of the stack
std::deque<context> event_contexts;

std::vector<pump_monitor*> pump_monitors;

}  //end anon namespace

pump_monitor::pump_monitor() {
    pump_monitors.push_back(this);
}

pump_monitor::~pump_monitor() {
    pump_monitors.erase(std::remove(pump_monitors.begin(), pump_monitors.end(), this),
                        pump_monitors.end());
}

event_context::event_context() {
    event_contexts.push_back(context());
}

event_context::~event_context() {
    assert(event_contexts.empty() == false);
    event_contexts.pop_back();
}

void focus_handler(const handler* ptr) {
    if (event_contexts.empty() == false) {
        event_contexts.back().set_focus(ptr);
    }
}

bool has_focus(const handler* hand, const SDL_Event* event) {
    if (event_contexts.empty()) {
        return true;
    }

    if (hand->requires_event_focus(event) == false) {
        return true;
    }

    const int foc_i = event_contexts.back().focused_handler;

    //if no-one has focus at the moment, this handler obviously wants
    //focus, so give it to it.
    if (foc_i == -1) {
        focus_handler(hand);
        return true;
    }

    handler * const foc_hand = event_contexts.back().handlers[foc_i];
    if (foc_hand == hand) {
        return true;
    } else if ( !foc_hand->requires_event_focus(event)) {
        //if the currently focused handler doesn't need focus for this event
        //allow the most recent interested handler to take care of it
        int back_i = event_contexts.back().handlers.size() - 1;
        for (int i = back_i; i >= 0; --i) {
            handler * const thief_hand = event_contexts.back().handlers[i];
            if (i != foc_i && thief_hand->requires_event_focus(event)) {
                //steal focus
                focus_handler(thief_hand);
                if (foc_i < back_i) {
                    //position the previously focused handler to allow stealing back
                    event_contexts.back().delete_handler_index(foc_i);
                    event_contexts.back().add_handler(foc_hand);
                }
                return thief_hand == hand;
            }
        }
    }
    return false;
}

void handler(const SDL_Event* evt) {
    switch (evt->type) {
        // Application events -----------------------------------------------------------------
        case SDL_QUIT:      // Send Quit to daemon
            do_quit(); break;
        // Android and iOS events IGNORED -----------------------------------------------------
        case SDL_APP_TERMINATING: break;
        case SDL_APP_LOWMEMORY: break;
        case SDL_APP_WILLENTERBACKGROUND: break;
        case SDL_APP_DIDENTERBACKGROUND: break;
        case SDL_APP_WILLENTERFOREGROUND: break;
        case SDL_APP_DIDENTERFOREGROUND: break;
        // Window events ----------------------------------------------------------------------
        case SDL_WINDOWEVENT: handle_window_event(evt); break;
        case SDL_SYSWMEVENT: break;
        // Keyboard events --------------------------------------------------------------------
        case SDL_KEYDOWN: break;
        case SDL_KEYUP: break;
        case SDL_TEXTEDITING: break;
        case SDL_TEXTINPUT:
            handle_text_input(evt); break;
        // Mouse events -----------------------------------------------------------------------
        case SDL_MOUSEMOTION:
            handle_mouse_motion(evt); break;
        case SDL_MOUSEBUTTONDOWN: break;
        case SDL_MOUSEBUTTONUP: break;
        case SDL_MOUSEWHEEL: break;
        // Joystick events --------------------------------------------------------------------
        case SDL_JOYAXISMOTION: break;
        case SDL_JOYBALLMOTION: break;
        case SDL_JOYHATMOTION: break;
        case SDL_JOYBUTTONDOWN: break;
        case SDL_JOYBUTTONUP: break;
        case SDL_JOYDEVICEADDED: break;
        case SDL_JOYDEVICEREMOVED: break;
        // Controller events ------------------------------------------------------------------
        case SDL_CONTROLLERAXISMOTION: break;
        case SDL_CONTROLLERBUTTONDOWN: break;
        case SDL_CONTROLLERBUTTONUP: break;
        case SDL_CONTROLLERDEVICEADDED: break;
        case SDL_CONTROLLERDEVICEREMOVED: break;
        case SDL_CONTROLLERDEVICEREMAPPED: break;
        // Touch events -----------------------------------------------------------------------
        case SDL_FINGERDOWN: break;
        case SDL_FINGERUP: break;
        case SDL_FINGERMOTION: break;
        // Gesture events ---------------------------------------------------------------------
        case SDL_DOLLARGESTURE: break;
        case SDL_DOLLARRECORD: break;
        case SDL_MULTIGESTURE: break;
        // Clipboard events -------------------------------------------------------------------
        case SDL_CLIPBOARDUPDATE: break;
        // Drag and drop events ---------------------------------------------------------------
        case SDL_DROPFILE: break;
        // These are for your use, and should be allocated with SDL_RegisterEvents() ----------
        case SDL_USEREVENT: break;
        case SDL_LASTEVENT: break;
    }
}

void handle_window_event(const SDL_Event* event) {
    if (event->type == SDL_WINDOWEVENT) {
        switch (event->window.event) {
            case SDL_WINDOWEVENT_SHOWN: break;
            case SDL_WINDOWEVENT_HIDDEN: break;
            case SDL_WINDOWEVENT_EXPOSED: break;
            case SDL_WINDOWEVENT_MOVED: break;
            case SDL_WINDOWEVENT_RESIZED:
                handle_window_resized(event); break;
            case SDL_WINDOWEVENT_MINIMIZED: break;
            case SDL_WINDOWEVENT_MAXIMIZED: break;
            case SDL_WINDOWEVENT_RESTORED: break;
            case SDL_WINDOWEVENT_ENTER: break;
            case SDL_WINDOWEVENT_LEAVE: break;
            case SDL_WINDOWEVENT_FOCUS_GAINED: break;
            case SDL_WINDOWEVENT_FOCUS_LOST: break;
            case SDL_WINDOWEVENT_CLOSE: break;
            default:
                fprintf(stdout, "Window %d got unknown event %d\n", event->window.windowID,
                        event->window.event);
                break;
        }
    }
}

void handle_window_resized(const SDL_Event* evt) {
    SDL_WindowEvent* win_evt = (SDL_WindowEvent*)evt;
    win_size[0] = win_evt->data1;
    win_size[0] = win_evt->data2;
}



void pump() {
    SDL_PumpEvents();

    pump_info info;

    //used to keep track of double click events
    static int last_mouse_down = -1;
    static int last_click_x = -1, last_click_y = -1;

    SDL_Event temp_event;
    int poll_count = 0;
    int begin_ignoring = 0;
    std::vector<SDL_Event> events;
    while (SDL_PollEvent( &temp_event)) {
        ++poll_count;
        if (!begin_ignoring) {
            //&&  temp_event.type == SDL_ACTIVEEVENT) {
            begin_ignoring = poll_count;
        } else if (begin_ignoring > 0 && is_input(temp_event)) {
            //ignore user input events that occurred after the window was activated
            continue;
        }
        events.push_back(temp_event);
    }
    std::vector<SDL_Event>::iterator ev_it = events.begin();
    for (int i = 1; i < begin_ignoring; ++i) {
        if (is_input( *ev_it)) {
            //ignore user input events that occurred before the window was activated
            ev_it = events.erase(ev_it);
        } else {
            ++ev_it;
        }
    }
    std::vector<SDL_Event>::iterator ev_end = events.end();
    for (ev_it = events.begin(); ev_it != ev_end; ++ev_it) {
        SDL_Event &event = *ev_it;
        switch (event.type) {

            case SDL_ACTIVEEVENT: {
                SDL_ActiveEvent& ae = reinterpret_cast<SDL_ActiveEvent&>(event);
                if ( (ae.state & SDL_APPMOUSEFOCUS) != 0 || (ae.state & SDL_APPINPUTFOCUS)
                    != 0) {
                    cursor::set_focus(ae.gain != 0);
                }
                break;
            }

                //if the window must be redrawn, update the entire screen
            case SDL_VIDEOEXPOSE: {
                update_whole_screen();
                break;
            }

            case SDL_VIDEORESIZE: {
                const SDL_ResizeEvent* const resize =
                    reinterpret_cast<SDL_ResizeEvent*>( &event);
                info.resize_dimensions.first = resize->w;
                info.resize_dimensions.second = resize->h;
                break;
            }

            case SDL_MOUSEMOTION: {
                //always make sure a cursor is displayed if the
                //mouse moves or if the user clicks
                cursor::set_focus(true);
                raise_help_string_event(event.motion.x, event.motion.y);
                break;
            }

            case SDL_MOUSEBUTTONDOWN: {
                //always make sure a cursor is displayed if the
                //mouse moves or if the user clicks
                cursor::set_focus(true);
                if (event.button.button == SDL_BUTTON_LEFT) {
                    static const int DoubleClickTime = 500;
                    static const int DoubleClickMaxMove = 3;
                    if (last_mouse_down >= 0 && info.ticks() - last_mouse_down < DoubleClickTime
                        && abs(event.button.x - last_click_x) < DoubleClickMaxMove
                        && abs(event.button.y - last_click_y) < DoubleClickMaxMove) {
                        SDL_UserEvent user_event;
                        user_event.type = DOUBLE_CLICK_EVENT;
                        user_event.code = 0;
                        user_event.data1 = reinterpret_cast<void*>(event.button.x);
                        user_event.data2 = reinterpret_cast<void*>(event.button.y);
                        ::SDL_PushEvent(reinterpret_cast<SDL_Event*>( &user_event));
                    }
                    last_mouse_down = info.ticks();
                    last_click_x = event.button.x;
                    last_click_y = event.button.y;
                }
                break;
            }

#if defined(_X11) && !defined(__APPLE__)
            case SDL_SYSWMEVENT: {
                //clipboard support for X11
                handle_system_event(event);
                break;
            }
#endif

#if defined _WIN32
                case SDL_SYSWMEVENT: {
                    windows_tray_notification::handle_system_event(event);
                    break;
                }
#endif

            case SDL_QUIT: {
                throw CVideo::quit();
            }
        }

        if (event_contexts.empty() == false) {

            const std::vector<handler*>& event_handlers = event_contexts.back().handlers;

            //events may cause more event handlers to be added and/or removed,
            //so we must use indexes instead of iterators here.
            for (size_t i1 = 0, i2 = event_handlers.size();
                i1 != i2 && i1 < event_handlers.size(); ++i1) {
                event_handlers[i1]->handle_event(event);
            }
        }
    }

    //inform the pump monitors that an events::pump() has occurred
    for (size_t i1 = 0, i2 = pump_monitors.size(); i1 != i2 && i1 < pump_monitors.size();
        ++i1) {
        pump_monitors[i1]->process(info);
    }
}

void raise_process_event() {
    if (event_contexts.empty() == false) {

        const std::vector<handler*>& event_handlers = event_contexts.back().handlers;

        //events may cause more event handlers to be added and/or removed,
        //so we must use indexes instead of iterators here.
        for (size_t i1 = 0, i2 = event_handlers.size(); i1 != i2 && i1 < event_handlers.size();
            ++i1) {
            event_handlers[i1]->process_event();
        }
    }
}

void raise_draw_event() {
    if (event_contexts.empty() == false) {

        const std::vector<handler*>& event_handlers = event_contexts.back().handlers;

        //events may cause more event handlers to be added and/or removed,
        //so we must use indexes instead of iterators here.
        for (size_t i1 = 0, i2 = event_handlers.size(); i1 != i2 && i1 < event_handlers.size();
            ++i1) {
            event_handlers[i1]->draw();
        }
    }
}

void raise_volatile_draw_event() {
    if (event_contexts.empty() == false) {

        const std::vector<handler*>& event_handlers = event_contexts.back().handlers;

        //events may cause more event handlers to be added and/or removed,
        //so we must use indexes instead of iterators here.
        for (size_t i1 = 0, i2 = event_handlers.size(); i1 != i2 && i1 < event_handlers.size();
            ++i1) {
            event_handlers[i1]->volatile_draw();
        }
    }
}

void raise_volatile_undraw_event() {
    if (event_contexts.empty() == false) {

        const std::vector<handler*>& event_handlers = event_contexts.back().handlers;

        //events may cause more event handlers to be added and/or removed,
        //so we must use indexes instead of iterators here.
        for (size_t i1 = 0, i2 = event_handlers.size(); i1 != i2 && i1 < event_handlers.size();
            ++i1) {
            event_handlers[i1]->volatile_undraw();
        }
    }
}

void raise_help_string_event(int mousex, int mousey) {
    if (event_contexts.empty() == false) {

        const std::vector<handler*>& event_handlers = event_contexts.back().handlers;

        for (size_t i1 = 0, i2 = event_handlers.size(); i1 != i2 && i1 < event_handlers.size();
            ++i1) {
            event_handlers[i1]->process_help_string(mousex, mousey);
            event_handlers[i1]->process_tooltip_string(mousex, mousey);
        }
    }
}

int pump_info::ticks(unsigned *refresh_counter, unsigned refresh_rate) {
    if ( !ticks_ && ! (refresh_counter && ++ *refresh_counter % refresh_rate)) {
        ticks_ = ::SDL_GetTicks();
    }
    return ticks_;
}

#if SDL_VERSION_ATLEAST(2,0,0)

/* The constants for the minimum and maximum are picked from the headers. */
#define INPUT_MIN 0x200
#define INPUT_MAX 0x8FF

bool is_input(const SDL_Event& event)
{
    return event.type >= INPUT_MIN && event.type <= INPUT_MAX;
}

void discard_input() {
    SDL_FlushEvents(INPUT_MIN, INPUT_MAX);
}

#else

#define INPUT_MASK (SDL_EVENTMASK(SDL_KEYDOWN)|\
		SDL_EVENTMASK(SDL_KEYUP)|\
		SDL_EVENTMASK(SDL_MOUSEBUTTONDOWN)|\
		SDL_EVENTMASK(SDL_MOUSEBUTTONUP)|\
		SDL_EVENTMASK(SDL_JOYBUTTONDOWN)|\
		SDL_EVENTMASK(SDL_JOYBUTTONUP))

bool is_input(const SDL_Event& event)
{
    return SDL_EVENTMASK(event.type) & INPUT_MASK;
}

static void discard(Uint32 event_mask)
{
    SDL_Event temp_event;
    std::vector< SDL_Event > keepers;
    SDL_Delay(10);
    while(SDL_PollEvent(&temp_event) > 0) {
        if((SDL_EVENTMASK(temp_event.type) & event_mask) == 0) {
            keepers.push_back( temp_event );
        }
    }

    //FIXME: there is a chance new events are added before kept events are replaced
    for (unsigned int i=0; i < keepers.size(); ++i)
    {
        if(SDL_PushEvent(&keepers[i]) != 0) {
            ERR_GEN << "failed to return an event to the queue.";
        }
    }
}

void discard_input()
{
    discard(INPUT_MASK);
}

#endif

}
  //end events namespace

#if !SDL_VERSION_ATLEAST(2,0,0)

void SDL_FlushEvent(Uint32 type)
{
events::discard(SDL_EVENTMASK(type));
}

#endif
