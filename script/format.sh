#!/bin/sh
#
# Format all source code files using clang-format.
#
# SPDX-FileCopyrightText: 2021-2023 Junde Yhi <junde@yhi.moe>
# SPDX-License-Identifier: MIT

set -e
clang-format --version
find include/ src/ test/ -type f -print -exec clang-format -i {} \+
