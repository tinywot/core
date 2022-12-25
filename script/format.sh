#!/bin/sh
# SPDX-FileCopyrightText: 2021-2022 Junde Yhi <junde@yhi.moe>
# SPDX-License-Identifier: MIT
#
# Format source code files using clang-format.

set -e

SRCS="$(find include/ src/ -name '*.[hc]' -or -name '*.ino' -type f)"

clang-format --version

echo 'Files to format:'
for SRC in $SRCS; do
  echo "- $SRC"
done

clang-format -i $SRCS
