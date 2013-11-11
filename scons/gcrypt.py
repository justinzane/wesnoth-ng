# vi: syntax=python:et:ts=4
import os, sys
from os.path import join
from os import environ
from SCons.Util import AppendPath

def CheckGcrypt(context, require_version = None):
    env = context.env
    if require_version:
        context.Message("Checking for Gcrypt version " + str(version) + " ... ")
    else:
        context.Message("Checking for Gcrypt ... ")

    try:
        env["ENV"]["PKG_CONFIG_PATH"] = environ.get("PKG_CONFIG_PATH")
        version_arg = ""
        if require_version:
            version_arg = " \\>= " + require_version
        env.ParseConfig("libgcrypt-config --cflags --libs")
        context.Result("yes")
        return True
    except OSError:
        context.Result("no")
        return False

config_checks = { "CheckGcrypt" : CheckGcrypt }
