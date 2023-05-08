/*
  SPDX-FileCopyrightText: 2023 Junde Yhi <junde@yhi.moe>
  SPDX-License-Identifier: MIT
*/

#include "example.h"
#include <tinywot.h>
#include <unity.h>

void test_tinywot_thing_get_form_normal(void) {
  struct tinywot_thing *thing = example_thing_new();
  struct tinywot_form const *form = NULL;
  tinywot_status_t status = TINYWOT_ERROR_GENERAL;

  status = tinywot_thing_get_form(
    thing, "/a0", TINYWOT_OPERATION_TYPE_READPROPERTY, &form
  );

  TEST_ASSERT_EQUAL(TINYWOT_SUCCESS, status);
  TEST_ASSERT_NOT_NULL(form);

  example_thing_free(thing);
}

void test_tinywot_thing_get_form_mismatch_opty(void) {
  struct tinywot_thing *thing = example_thing_new();
  struct tinywot_form const *form = NULL;
  tinywot_status_t status = TINYWOT_ERROR_GENERAL;

  status = tinywot_thing_get_form(
    thing, "/a0", TINYWOT_OPERATION_TYPE_INVOKEACTION, &form
  );

  TEST_ASSERT_EQUAL(TINYWOT_ERROR_NOT_ALLOWED, status);
  TEST_ASSERT_NULL(form);

  example_thing_free(thing);
}

void test_tinywot_thing_get_form_mismatch_target(void) {
  struct tinywot_thing *thing = example_thing_new();
  struct tinywot_form const *form = NULL;
  tinywot_status_t status = TINYWOT_ERROR_GENERAL;

  status = tinywot_thing_get_form(
    thing, "/az", TINYWOT_OPERATION_TYPE_READPROPERTY, &form
  );

  TEST_ASSERT_EQUAL(TINYWOT_ERROR_NOT_FOUND, status);
  TEST_ASSERT_NULL(form);

  example_thing_free(thing);
}
