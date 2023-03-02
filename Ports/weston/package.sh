#!/usr/bin/env -S bash ../.port_include.sh
port='weston'
version='11.0.1'
useconfigure='true'
configopts=(
    "--buildtype=release"
    "--cross-file=${SERENITY_BUILD_DIR}/meson-cross-file.txt"
    "-Dimage-jpeg=false"
    "-Dimage-webp=false"
)
depends=(
    'wayland'
    'xkbcommon'
    'cairo'
)
files="https://gitlab.freedesktop.org/wayland/weston/uploads/f5648c818fba5432edc3ea63c4db4813/weston-${version}.tar.xz weston-${version}.tar.xz a413f68c252957fc3191c3650823ec356ae8c124ccc0cb440da5cdc4e2cb9e57"
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
