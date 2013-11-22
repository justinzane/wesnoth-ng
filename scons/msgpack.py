##---------------------------------------------------------------------------------------------
# @file    msgpack.py
# @brief   SCons script that checks for presence of MsgPack and adds build configuration if found.
# @author  Justin Zane Chudgar <justin@justinzane.com>
# @copyright   Copyright justin, Nov 21, 2013
# @section License GPLv3+
# wesnoth-ng is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# wesnoth-ng is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with wesnoth-ng.  If not, see http://www.gnu.org/licenses/.
# @note    This is currently ONLY setup for GNU/Linux.
#----------------------------------------------------------------------------------------------
from config_check_utils import *
from os.path import join, dirname, basename
from glob import glob
import re

def CheckMsgPack(context, conf, require_version=None):
    env = context.env

    if not conf.CheckCXXHeader("msgpack.hpp"):
        return false

    if not conf.CheckLib("msgpack"):
        return false

    env.AppendUnique(LIBS="msgpack")
    return True

config_checks = { "CheckMsgPack" : CheckMsgPack, }
