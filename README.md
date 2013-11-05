# The Battle for Wesnoth 

The Battle for Wesnoth is a Free, turn-based tactical strategy game with a high
fantasy theme, featuring both single-player, and online/hotseat multiplayer
combat. Fight a desperate battle to reclaim the throne of Wesnoth, or take hand
in any number of other adventures.

## The wesnoth-ng Fork

This is a personal fork of the mailine codebase used for experimentation with the Wesnoth code. 
Though I hope that some of these experiments will be successful enough for inclusion in the 
mainline tree, noone should expect that anything here works well anough to be used for 
normal gameplay.

## The SDL2 Port

A major focus of this branch is the port from SDL1.2 to SDL2. This entails significant changes 
to the UI code. During this transition, I hope to remove some existing cruft and include new
functionality. The goals, at the moment are:

- Use SDL2 not SDL1.2.
- Use OpenCV2 for image operations where possible.
- Reorganize code into logical structures and adapt SCons.
- Reformat code to modified K&R
- Use include-what-you-use to optimize includes
- Use c++11, not c++98
- ...
- Profit (?)

## License

The game's source code and artwork, sound, and music assets are provided under
the terms of the GNU General Public License version 2, or (at your option) any
later version. Note that for artwork, sound, and music, we interpret
"preferred form of the work for making modifications" as the modifiable form
that the author chooses to ship us for the source tree. For convenience, a
reference copy of the GNU GPL version 2 is provided in the COPYING file in
this distribution.

Some portions of the source code can be used under different license terms,
whenever stated as such in the source.
