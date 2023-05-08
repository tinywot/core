/*
  SPDX-FileCopyrightText: 2023 Junde Yhi <junde@yhi.moe>
  SPDX-License-Identifier: MIT
*/

#ifndef TINYWOT_TEST_EXAMPLE_H
#define TINYWOT_TEST_EXAMPLE_H

#include <tinywot.h>

struct tinywot_thing *example_thing_new(void);
void example_thing_free(struct tinywot_thing *thing);
struct tinywot_form const *example_form_0(void);
struct tinywot_form const *example_form_1(void);

#endif
