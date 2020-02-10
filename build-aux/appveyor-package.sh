set -x
set -e

case $MSYSTEM in
    MINGW32)
        MINGW_ARCH=mingw32
        ;;
    MINGW64)
        MINGW_ARCH=mingw64
        ;;
esac

cd c:/projects/enchant
./bootstrap
./configure --enable-relocatable --disable-gcc-warnings --with-hunspell-dir=/$MINGW_ARCH/share/hunspell
make
make install DESTDIR=$(pwd)/dist

cp -v /$MINGW_ARCH/bin/*.dll c:/projects/enchant/dist/$MINGW_ARCH/bin/
mkdir -p c:/projects/enchant/dist/$MINGW_ARCH/share/enchant/hunspell/
cp -v /$MINGW_ARCH/share/hunspell/*  c:/projects/enchant/dist/$MINGW_ARCH/share/enchant/hunspell
