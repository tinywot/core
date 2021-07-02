#!/bin/sh
#
# Convenient script for invoking PlatformIO automatic (CI) builds.
#
# SPDX-FileCopyrightText: 2021 Junde Yhi <junde@yhi.moe>
# SPDX-License-Identifier: MIT

set -e

platformio ci \
  --lib . \
  --board uno \
  example/arduino-led/main.ino
