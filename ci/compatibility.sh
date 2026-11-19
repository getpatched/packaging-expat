#!/usr/bin/env bash
set -euo pipefail
work=$(mktemp -d)
trap 'rm -rf "$work"' EXIT
cc -std=c99 -Wall -Wextra -Werror /recipe/ci/compatibility.c -lexpat -o "$work/consumer"
"$work/consumer"
