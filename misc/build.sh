#!/usr/bin/env bash

export ASAN_OPTIONS=verify_asan_link_order=0
mkdir -p build
cmake -Bbuild -G Ninja && pushd build && ninja & popd
