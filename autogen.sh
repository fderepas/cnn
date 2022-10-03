#!/bin/bash
cd `dirname $0`
if [ $# -eq 0 ]
then
    # No arguments supplied
    touch NEWS
    which autoreconf
    if [ $? -ne "0" ]; then
        echo "autoreconf not found try:"
        echo "   sudo apt-get install autoconf"
        exit 1
    fi
    autoreconf --install --force --verbose
    if [ `uname` == "Linux" ]; then echo 1 > uname_is_linux; else echo 0 > uname_is_linux; fi
else
    # one argument was supplied
    if [[ $1 == "clean" ]]; then
        if [[ -f Makefile ]]; then
            make clean
        fi
        find . -name \*~ -exec rm -f {} \;
        rm -f *.log 
        rm -f aclocal.m4 compile config.h config.status configure depcomp INSTALL install-sh config.in install.sh Makefile Makefile.in missing NEWS stamp-h1 ar-lib 
	rm -rf autom4te.cache
        find . -name .deps -exec rm -rf {} \;
        find . -name Makefile -exec rm -f {} \;
        find . -name Makefile.in -exec rm -f {} \;
        find . -name .DS_Store -exec rm -rf {} \;
	rm -f uname_is_linux
    else
        echo "unknown command $1"
    fi
fi
