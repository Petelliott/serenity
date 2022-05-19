#!/usr/bin/env -S bash ../.port_include.sh
port=stockfish
version=15
files="https://github.com/official-stockfish/Stockfish/archive/refs/tags/sf_${version}.tar.gz sf_${version}.tar.gz 0553fe53ea57ce6641048049d1a17d4807db67eecd3531a3749401362a27c983"
auth_type=sha256
workdir="Stockfish-sf_${version}"

makeopts=("-Csrc/" "build" "ARCH=x86-32" "COMP=${CC}" "COMPCXX=${CXX}")
installopts=("-Csrc/" "PREFIX=${DESTDIR}/usr/local")
