#!/bin/sh
#
# Format source code files using clang-format.
#
# SPDX-FileCopyrightText: 2021 Junde Yhi <junde@yhi.moe>
# SPDX-License-Identifier: MIT

which clang-format
if [ "$?" != '0' ]; then
  echo 'clang-format is not found; it needs to be installed first.'
  exit 1
fi

SRCS=$(find include/ src/ example/ -name '*.[hc]' -or -name '*.ino' -type f)

echo 'Files to format:'
for SRC in $SRCS; do
  echo "- $SRC"
done

clang-format -i $SRCS
