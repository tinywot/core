# Recipe for building the standard development environment for TinyWoT Core.
#
# SPDX-FileCopyrightText: 2023-2024 Junde Yhi <junde@yhi.moe>
# SPDX-License-Identifier: MIT

FROM ubuntu:22.04

# These parameters specify the normal user to create in the container.
ARG USER=user
ARG GROUP=user
ARG SHELL=/bin/bash
ARG UID=1000
ARG GID=1000

# With root, install system-wide packages provided by Ubuntu.
RUN apt-get update --yes && \
apt-get install --yes pipx gcc gdb graphviz doxygen && \
rm --force --recursive /var/lib/apt/lists/*

# Create a normal user.
RUN groupadd --gid $GID $GROUP && \
useradd --uid $UID --gid $GID --shell $SHELL --create-home $USER

# Switch to this normal user.
WORKDIR /home/$USER
USER $UID:$GID

# Prepend PATH where PipX places executables.
ENV PATH="/home/$USER/.local/bin:$PATH"

# Install Python packages.
RUN pipx install platformio && \
pipx install reuse && \
rm --force --recursive /home/$USER/.cache

# Tweak PlatformIO.
RUN platformio settings set check_platformio_interval 999999 && \
platformio settings set projects_dir /home/$USER

# In case one wants to use PlatformIO Home.
EXPOSE 8008

LABEL org.opencontainers.image.title="TinyWoT Core Dev Container" \
org.opencontainers.image.description="Standard development environment for TinyWoT Core." \
org.opencontainers.image.authors="Junde Yhi <junde@yhi.moe>"
