#!/bin/sh
# Pre-install script for appveyor: install build deps

set -x

# Get mingw type, if any, from MSYSTEM
case $MSYSTEM in
    MINGW32)
        MINGW_BITS=i686
        PREFIX=/mingw32
        ;;
    MINGW64)
        MINGW_BITS=x86_64
        PREFIX=/mingw64
        ;;
    MSYS)
        PREFIX=/usr
        ;;
esac

# GLib
if test -n "$MINGW_BITS"; then
    pacman --noconfirm -S mingw-w64-$MINGW_BITS-glib2
fi
pacman --noconfirm -S glib2-devel

# Hunspell dictionary (only need one flavour)
wget http://repo.msys2.org/mingw/$MINGW_BITS/mingw-w64-$MINGW_BITS-hunspell-en-2016.11.20-2-any.pkg.tar.xz
pacman --noconfirm -U mingw-w64-$MINGW_BITS-hunspell-en-2016.11.20-2-any.pkg.tar.xz

# Hunspell
wget https://github.com/hunspell/hunspell/archive/v1.6.0.tar.gz
tar zxvf v1.6.0.tar.gz || true # Error in unpacking (symlink README before file README.md)
cd hunspell-1.6.0
ln -s README.md README && autoreconf -vfi
./configure --prefix=$PREFIX && make && make install
cd ..
