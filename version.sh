#!/usr/bin/env bash
set -e

VERSION="0.0.1"
[ -z "$BUILDER" ] && BUILDER=`whoami`
CURRENT_TIME=$(date "+%Y.%m.%d-%H.%M.%S")

if [[ "$1" = "--version" ]]; then
    echo "$VERSION"
fi
if [[ "$1" = "--builder" ]]; then
    echo "$BUILDER"
fi
if [[ "$1" = "--date" ]]; then
    echo "$CURRENT_TIME"
fi
