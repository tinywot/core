#!/bin/sh
#
# Use or manager a development container for TinyWoT.
#
# A development container acts as a standard development environment for the
# project. This script installs and configures one if it doesn't exist.
# Otherwise, it starts the container.
#
# This script assumes that the current working directory is TinyWoT source root.
# It is mounted on /tinywot inside the container.
#
# SPDX-FileCopyrightText: 2023 Junde Yhi <junde@yhi.moe>
# SPDX-License-Identifier: MIT

set -e

SUBCMD="$1"

if test -z "$CONTAINER_ID"; then
  CONTAINER_ID='build-tinywot'
fi

if ! command -v podman > /dev/null; then
  echo '** Podman is required to run the script.'
  exit 2
fi

if test "$SUBCMD" = 'rm'; then
  echo ':: Removing TinyWoT development container...'
  podman container rm "$CONTAINER_ID"
  exit $?
fi

if ! podman container exists "$CONTAINER_ID"; then
  echo ':: Creating TinyWoT development container...'
  podman container create \
    --hostname "$CONTAINER_ID" \
    --mount 'type=bind,src=.,dst=/tinywot' \
    --name "$CONTAINER_ID" \
    --interactive --tty \
    'ubuntu:22.04'

  echo ':: Configuring TinyWoT development container...'
  podman container start "$CONTAINER_ID"
  podman container exec "$CONTAINER_ID" sh -c '\
    apt-get update && apt-get full-upgrade --autoremove --purge --yes && \
    apt-get install --yes build-essential python3 python3-pip clang-format \
      doxygen graphviz && \
    python3 -m pip install platformio reuse'
else
  echo ':: Starting existing TinyWoT development container...'
  podman container start "$CONTAINER_ID"
fi

echo ':: Attaching...'
podman container attach "$CONTAINER_ID"
