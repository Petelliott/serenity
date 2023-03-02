#!/usr/bin/env -S bash ../.port_include.sh
port='cairo'
version='1.17.8'
files="https://cairographics.org/snapshots/cairo-${version}.tar.xz cairo-${version}.tar.xz 5b10c8892d1b58d70d3f0ba5b47863a061262fa56b9dc7944161f8c8b783bc64"
auth_type='sha256'

useconfigure='true'
configopts=(
    "--buildtype=release"
    "--cross-file=${SERENITY_BUILD_DIR}/meson-cross-file.txt"
)

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
