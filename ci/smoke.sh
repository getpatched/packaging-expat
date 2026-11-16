#!/usr/bin/env bash
set -euo pipefail
work=$(mktemp -d)
trap 'rm -rf "$work"' EXIT
cd "$work"
printf '%s\n' '<root><item>value</item></root>' > sample.xml
xmlwf sample.xml
