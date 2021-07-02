#!/bin/sh
#
# Format source code files using clang-format.
#
# SPDX-FileCopyrightText: 2021 Junde Yhi <junde@yhi.moe>
# SPDX-License-Identifier: MIT

set -e

SRCS=$(find include/ src/ example/ -name '*.[hc]' -or -name '*.ino' -type f)

echo 'Files to format:'
for SRC in $SRCS; do
  echo "- $SRC"
done

clang-format -i $SRCS
