/*
  SPDX-FileCopyrightText: 2023 Junde Yhi <junde@yhi.moe>
  SPDX-License-Identifier: MIT
*/

#include "example.h"
#include <tinywot.h>
#include <unity.h>

static void test_tinywot_thing_get_form_normal(void) {
  struct tinywot_thing const *thing = NULL;
  struct tinywot_form const *form = NULL;
  tinywot_status_t status = TINYWOT_ERROR_GENERAL;

  init_example_thing(&thing);

  /* Normal, valid. */

  status = tinywot_thing_get_form(
    thing, "/a0", TINYWOT_OPERATION_TYPE_READPROPERTY, &form
  );

  TEST_ASSERT_TRUE(status == TINYWOT_SUCCESS);
  TEST_ASSERT_NOT_NULL(form);

  form = NULL;
  status = TINYWOT_ERROR_GENERAL;

  /* Operation type(s) mismatch. */

  status = tinywot_thing_get_form(
    thing,
    "/a0",
    TINYWOT_OPERATION_TYPE_READPROPERTY | TINYWOT_OPERATION_TYPE_WRITEPROPERTY,
    &form
  );

  TEST_ASSERT_TRUE(status == TINYWOT_ERROR_NOT_FOUND);
  TEST_ASSERT_NULL(form);

  form = NULL;
  status = TINYWOT_ERROR_GENERAL;

  /* Target mismatch. */

  status = tinywot_thing_get_form(
    thing, "/doesnotexist", TINYWOT_OPERATION_TYPE_READPROPERTY, &form
  );

  TEST_ASSERT_TRUE(status == TINYWOT_ERROR_NOT_FOUND);
  TEST_ASSERT_NULL(form);
}

void suite_tinywot_thing_get_form(void) {
  RUN_TEST(test_tinywot_thing_get_form_normal);
}
