# Source Generated with Decompyle++
# File: llvm.pyc (Python 2.7)

'''llvm

Tool-specific initialization for LLVM

License: GPLv2
Author: Justin Zane Chudgar, <justin@justinzane.com>
Date: 2013
Note: Much of this was copied from the Mesa project.
'''
import os
import subprocess
import distutils.version
import SCons.Errors
import SCons.Util
from config_check_utils import *

def exists():
    return True


def generate(env):
    if not env.Detect('llvm-config'):
        print 'Error: llvm-config not found.'
        context.Result('no')
        return False

    try:
        sp = subprocess.Popen(['llvm-config', '--version'], stdout=subprocess.PIPE)
        o,e = sp.communicate()
        llvm_version = distutils.version.LooseVersion(str(float(o)))
    except Exception as e:
        print e.message
        return False

    print 'scons: Found LLVM version %s' % llvm_version
    env['LLVM_VERSION'] = llvm_version
    llvm_version_major = int(llvm_version.version[0])
    llvm_version_minor = int(llvm_version.version[1])
    llvm_version_hex = '0x%02x%02x' % (llvm_version_major, llvm_version_minor)
    env.Prepend(CPPDEFINES = [('HAVE_LLVM', llvm_version_hex)])
    return True


def CheckLLVM(context):
    ''' TODO: WRITEME since I'm currently totally bogus! '''
    print 'Checking the clang++ compiler...'
    context.Result('yes')
    return True

config_checks = {
    'CheckLLVM': CheckLLVM
}
