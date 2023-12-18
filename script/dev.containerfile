# Standard development environment for TinyWoT Core.
#
# SPDX-FileCopyrightText: 2023 Junde Yhi <junde@yhi.moe>
# SPDX-License-Identifier: MIT

FROM ubuntu:22.04

ENV PATH="/root/.local/bin:$PATH"

# Install packages provided by Ubuntu.
RUN apt-get update --yes && \
apt-get install --yes pipx gcc gdb && \
rm --force --recursive /var/lib/apt/lists/*

# Install packages from PyPI.
RUN pipx install platformio && pipx install reuse

# Set up PlatformIO.
RUN platformio settings set check_platformio_interval 999999 && \
platformio settings set enable_cache no && \
platformio settings set enable_telemetry no

# Pre-install packages from PlatformIO Registry, so that the subsequent
# operations can be carried out without Internet.
#
# Version constraints of the first few tools are from
# platformio/__init__.py: the registry may already have newer versions,
# but PlatformIO doesn't want to use.
RUN platformio pkg install --global \
--tool platformio/contrib-piohome@~3.4.2 \
--tool contrib-pioremote@~1.0.0 \
--tool platformio/tool-scons@~4.40502.0 \
--tool platformio/tool-cppcheck@~1.21100.0 \
--platform platformio/native \
--platform platformio/atmelavr \
--tool platformio/toolchain-atmelavr \
--tool platformio/framework-arduino-avr \
# Required by (platform) platformio/atmelavr
--tool platformio/tool-avrdude@~1.60300.0

# Open this port for PlatformIO Home.
EXPOSE 8008

LABEL org.opencontainers.image.title="TinyWoT Core Dev Container" \
org.opencontainers.image.description="Standard development environment for TinyWoT Core." \
org.opencontainers.image.authors="Junde Yhi <junde@yhi.moe>"
