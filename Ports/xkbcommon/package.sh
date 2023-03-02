#!/usr/bin/env -S bash ../.port_include.sh
port='xkbcommon'
version='1.5.0'
useconfigure='true'
workdir="libxkbcommon-xkbcommon-${version}"
configopts=(
    "--buildtype=release"
    "--cross-file=${SERENITY_BUILD_DIR}/meson-cross-file.txt"
    "-Denable-x11=false"
    "-Denable-docs=false"
)
depends=('wayland-protocols')
files="https://github.com/xkbcommon/libxkbcommon/archive/refs/tags/xkbcommon-${version}.tar.gz xkbcommon-${version}.tar.gz 053e6a6a2c3179eba20c3ada827fb8833a6663b7ffd278fdb8530c3cbf924780"
auth_type='sha256'

configure() {
    # TODO: Figure out why GCC doesn't autodetect that libgcc_s is needed.
    if [ "${SERENITY_TOOLCHAIN}" = "GNU" ]; then
        export LDFLAGS="-lgcc_s"
    fi

    run meson build "${configopts[@]}"
}

build() {
    run ninja -C build
}

install() {
    export DESTDIR="${SERENITY_INSTALL_ROOT}"
    run meson install -C build
}
