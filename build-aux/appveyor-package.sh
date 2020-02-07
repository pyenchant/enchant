set -x
set -e

cd c:/projects/enchant
./bootstrap
./configure --enable-relocatable --disable-gcc-warnings --with-hunspell-dir=/mingw64/share/hunspell
make
make install DESTDIR=$(pwd)/dist

cp -v /mingw32/bin/*.dll c:/projects/enchant/dist/mingw32/bin/
mkdir -p c:/projects/enchant/dist/mingw32/share/enchant/hunspell/
cp -v /mingw32/share/hunspell/*  c:/projects/enchant/dist/mingw32/share/enchant/hunspell
