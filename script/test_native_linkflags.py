# Extra script supplying link-time compiler options, invoked by PlatformIO.
#
# See also:
#
# - https://docs.platformio.org/en/latest/scripting/examples/extra_linker_flags.html
#
# SPDX-FileCopyrightText: 2023 Junde Yhi <junde@yhi.moe>
# SPDX-License-Identifier: MIT

Import("env")

env.Append(
  LINKFLAGS=[
    "-fsanitize=address,undefined"
  ]
)
