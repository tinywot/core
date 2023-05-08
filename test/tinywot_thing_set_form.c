/*
  SPDX-FileCopyrightText: 2023 Junde Yhi <junde@yhi.moe>
  SPDX-License-Identifier: MIT
*/

#include "example.h"
#include <string.h>
#include <tinywot.h>
#include <unity.h>

void test_tinywot_thing_set_form_overlap_opty(void) {
  struct tinywot_thing *thing = example_thing_new();
  struct tinywot_form const *form = example_form_1();
  tinywot_status_t status = TINYWOT_ERROR_GENERAL;

  status = tinywot_thing_set_form(thing, form);

  TEST_ASSERT_EQUAL(TINYWOT_SUCCESS, status);

  example_thing_free(thing);
}

void test_tinywot_thing_set_form_read_only(void) {
  struct tinywot_thing *thing = example_thing_new();
  struct tinywot_form form = {0};
  tinywot_status_t status = TINYWOT_ERROR_GENERAL;

  thing->flags = TINYWOT_FLAG_READ_ONLY;

  status = tinywot_thing_set_form(thing, &form);

  TEST_ASSERT_EQUAL(TINYWOT_ERROR_READ_ONLY, status);

  example_thing_free(thing);
}
