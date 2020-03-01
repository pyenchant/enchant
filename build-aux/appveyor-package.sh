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

PROJECT_DIR="c:/projects/enchant"

build() {
  (
    cd $PROJECT_DIR
    ./bootstrap
    ./configure \
        --enable-relocatable \
        --disable-gcc-warnings \
        --with-hunspell-dir=/$MINGW_ARCH/share/hunspell
    make
  )
}

install() {
  (
    cd $PROJECT_DIR
    make install DESTDIR=$PROJECT_DIR/dist
  )
}

copy_dlls() {
  dest="$PROJECT_DIR/dist/$MINGW_ARCH/bin/"
  mkdir -p $dest
  dlls=(
    libgcc_s_seh-1.dll
    libglib-2.0-0.dll
    libgmodule-2.0-0.dll
    libgobject-2.0-0.dll
    libhunspell-1.6-0.dll
    libiconv-2.dll
    libintl-8.dll
    libp11-kit-0.dll
    libpcre-1.dll
    libpcre16-0.dll
    libpcre32-0.dll
    libpcrecpp-0.dll
    libpcreposix-0.dll
    libstdc++-6.dll
    libwinpthread-1.dll
  )
  for dll in "${dlls[@]}" ; do
    cp -v /$MINGW_ARCH/bin/$dll $dest
  done
}

copy_licenses() {
  dest="$PROJECT_DIR/dist/$MINGW_ARCH/share/licences/"
  mkdir -p $dest
  licenses=(
    gcc-libs
    glib2
    libiconv
    gettext
    pcre
  )

  for license in "${licenses[@]}" ; do
    cp -rv /$MINGW_ARCH/share/licenses/$license $dest
  done

  mkdir -p $dest/enchant
  cp -v $PROJECT_DIR/COPYING.lib $dest/enchant/
}

copy_dicts() {
  dest=$PROJECT_DIR/dist/$MINGW_ARCH/share/enchant/hunspell
  mkdir -p $dest
  cp -v /$MINGW_ARCH/share/hunspell/*  $dest
}

main() {
  build
  install
  copy_dlls
  copy_licenses
  copy_dicts
}

main
