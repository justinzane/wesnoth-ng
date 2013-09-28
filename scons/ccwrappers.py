# vi: syntax=python:et:ts=4
# Shamelessly stolen from FreeOrion's SConstruct
# http://freeorion.svn.sourceforge.net/viewvc/freeorion/trunk/FreeOrion/SConstruct?revision=2478&view=markup

import os

def exists():
    return True

def generate(env):
    # check ccache first
    if env['ccache']:
        env['CCACHE'] = env.WhereIs("ccache")
        env['CCACHE_TEMPDIR'] = '/tmp/ccache_temp/'
        env['CCACHE_LOGFILE'] = '/tmp/ccache.log'
        for i in ['HOME',
                  'CCACHE_DIR',
                  'CCACHE_PATH',
                  'CCACHE_DISABLE',
                  'CCACHE_READONLY',
                  'CCACHE_CPP2',
                  'CCACHE_NOSTATS',
                  'CCACHE_NLEVELS',
                  'CCACHE_HARDLINK',
                  'CCACHE_RECACHE',
                  'CCACHE_UMASK',
                  'CCACHE_HASHDIR',
                  'CCACHE_UNIFY',
                  'CCACHE_EXTENSION']:
            if os.environ.has_key(i) and not env.has_key(i):
                env['ENV'][i] = os.environ[i]


    # Check distcc second.
    if env['distcc']:
        env['DISTCC'] = env.WhereIs("distcc")
        env['DISTCC_VERBOSE'] = '1'
        env['DISTCC_LOG'] = '/tmp/distcc.log'
        for i in ['HOME',
                  'DISTCC_HOSTS',
                  'DISTCC_FALLBACK',
                  'DISTCC_MMAP',
                  'DISTCC_SAVE_TEMPS',
                  'DISTCC_TCP_CORK',
                  'DISTCC_SSH'
                  ]:
            if os.environ.has_key(i) and not env.has_key(i):
                env['ENV'][i] = os.environ[i]

    # distcc AND ccache
    if env['ccache'] and env['distcc']:
        env['CC'] = '$CCACHE $DISTCC %s' % env['CC']
        env['CXX'] = '$CCACHE $DISTCC %s' % env['CXX']
        env['CCACHE_PREFIX'] = 'distcc'
    # just distcc
    elif env['distcc']:
        env['CC'] = '$DISTCC %s' % env['CC']
        env['CXX'] = '$DISTCC %s' % env['CXX']
        env['CCACHE_PREFIX'] = 'distcc'
    #just ccache
    elif env['ccache']:
        env['CC'] = '$CCACHE %s' % env['CC']
        env['CXX'] = '$CCACHE %s' % env['CXX']
