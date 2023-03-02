#!/usr/bin/env -S bash ../.port_include.sh
port='wayland-protocols'
version='1.31'
useconfigure='true'
configopts=(
    "--buildtype=release"
    "--cross-file=${SERENITY_BUILD_DIR}/meson-cross-file.txt"
    "-Dtests=false"
)
depends=()
files="https://gitlab.freedesktop.org/wayland/wayland-protocols/-/releases/${version}/downloads/wayland-protocols-${version}.tar.xz wayland-protocols-${version}.tar.xz a07fa722ed87676ec020d867714bc9a2f24c464da73912f39706eeef5219e238"
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
