# vi: syntax=python:et:ts=4
import os
from SCons.Script import *
from config_check_utils import *

def CheckSDL(context, sdl_lib="SDL", require_version=False, use_sdl2=False):
    if require_version:
        version = require_version.split(".", 2)
        major_version = int(version[0])
        minor_version = int(version[1])
        try:
            patchlevel = int(version[2])
        except (ValueError, IndexError):
            patch_level = 0

    if use_sdl2:
        sdl_path = "SDL2"
    else:
        sdl_path = "SDL"

    backup = backup_env(context.env, ["CPPPATH", "LIBPATH", "LIBS"])

    sdldir = context.env.get("sdldir", "")
    if sdl_lib == "SDL":
        if require_version:
            context.Message("Checking for SDL version >= %d.%d.%d... " %
                            (major_version, minor_version, patchlevel))
        else:
            context.Message("Checking for SDL... ")
        env = context.env
        if sdldir and use_sdl2:
            env.AppendUnique(CPPPATH=[os.path.join(sdldir, "include/SDL2")],
                             LIBPATH=[os.path.join(sdldir, "lib")])
        elif sdldir and not use_sdl2:
            env.AppendUnique(CPPPATH=[os.path.join(sdldir, "include/SDL")],
                             LIBPATH=[os.path.join(sdldir, "lib")])
        elif not sdldir and use_sdl2:
            for foo_config in ["pkg-config --cflags --libs sdl2",
                               "sdl2-config --cflags --libs"]:
                try:
                    env.ParseConfig(foo_config)
                except OSError:
                    pass
                else:
                    break
        else:
            for foo_config in ["pkg-config --cflags --libs sdl",
                               "sdl-config --cflags --libs"]:
                try:
                    env.ParseConfig(foo_config)
                except OSError:
                    pass
                else:
                    break
        if env["PLATFORM"] == "win32":
            env.AppendUnique(CCFLAGS=["-D_GNU_SOURCE"])
            env.AppendUnique(LIBS=Split("mingw32 SDLmain SDL"))
            env.AppendUnique(LINKFLAGS=["-mwindows"])
    else:
        if require_version:
            context.Message("Checking for %s version >= %d.%d.%d... " %
                            (sdl_lib, major_version, minor_version, patchlevel))
        else:
            context.Message("Checking for %s library... " % sdl_lib)
        context.env.AppendUnique(LIBS=[sdl_lib])
    test_program = ("#include <%s/%s.h>\n" % (sdl_path, sdl_lib))
    if require_version:
        test_program += ("#if SDL_VERSIONNUM(%s, %s, %s) < " %
                         (sdl_lib.upper() + "_MAJOR_VERSION",
                          sdl_lib.upper() + "_MINOR_VERSION",
                          sdl_lib.upper() + "_PATCHLEVEL",))
        test_program += ("SDL_VERSIONNUM(%d, %d, %d)\n#error Library is too old!\n#endif\n" %
                         (major_version, minor_version, patchlevel))
    test_program += "int main(int argc, char** argv) { }\n"
    if context.TryLink(test_program, ".c"):
        context.Result("yes")
        return True
    else:
        context.Result("no")
        restore_env(context.env, backup)
        return False

# #
# @brief Check if SDL supports libpng
def CheckPNG(context, use_sdl2=False):
    if use_sdl2:
        sdl_path = "SDL2"
        test_program = ('''#include <%s/SDL_image.h>
#include <stdlib.h>
int main(int argc, char **argv) {
    SDL_Surface* src = IMG_Load("images/buttons/button_normal/button_H22-pressed.png");
    if (src == NULL) { exit(2); } else { exit(0); } }\n
''' % sdl_path)
    else:
        sdl_path = "SDL"
        test_program = ('''
#include <%s/SDL_image.h>
#include <stdlib.h>
int main(int argc, char **argv) {
    SDL_RWops *src;
    char *testimage = "images/buttons/button_normal/button_H22-pressed.png";
    src = SDL_RWFromFile(testimage, "rb");
    if (src == NULL) { exit(2); }
    exit(!IMG_isPNG(src));
}\n
''' % sdl_path)
    context.Message("Checking for PNG support in %s... " % sdl_path)
    (result, output) = context.TryRun(test_program, ".c")
    if result:
        context.Result("yes")
        return True
    else:
        context.Result("NO -- %s" % output)
        return False

# #
# @brief List of checks defined in this file.
config_checks = {'CheckSDL' : CheckSDL,
                 'CheckPNG' : CheckPNG }
