#!/usr/bin/env bash
set -euo pipefail

# Bookworm's debian/rules builds the test binaries without running make check.
# Reuse its configured build tree to test the patched code and distro flags.
make -C build -j1 check
