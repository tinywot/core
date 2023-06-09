/*
  SPDX-FileCopyrightText: 2023 Junde Yhi <junde@yhi.moe>
  SPDX-License-Identifier: MIT
*/

#ifndef TINYWOT_TEST_EXAMPLE_THING_H
#define TINYWOT_TEST_EXAMPLE_THING_H

#include <tinywot/core.h>

void example_thing_delete(struct tinywot_thing_dynamic **thing);
struct tinywot_thing_dynamic *example_thing_new(void) __attribute__((__malloc__));
struct tinywot_thing_dynamic *example_thing_new_with_forms(void) __attribute__((__malloc__));

#endif
