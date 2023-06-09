/*
  SPDX-FileCopyrightText: 2023 Junde Yhi <junde@yhi.moe>
  SPDX-License-Identifier: MIT
*/

#include "example_thing.h"
#include <tinywot/core.h>
#include <unity.h>

void test_tinywot_thing_dynamic_get_form_normal(void) {
  struct tinywot_thing_dynamic *thing = example_thing_new_with_forms();
  struct tinywot_form const *form = NULL;
  tinywot_status_t status = TINYWOT_STATUS_SUCCESS;

  status = tinywot_thing_dynamic_get_form(
    thing, "/a", TINYWOT_OPERATION_TYPE_READPROPERTY, &form
  );

  TEST_ASSERT_EQUAL(TINYWOT_STATUS_SUCCESS, status);
  TEST_ASSERT_NOT_NULL(form);

  example_thing_delete(&thing);
}

void test_tinywot_thing_dynamic_get_form_mismatch_opty(void) {
  struct tinywot_thing_dynamic *thing = example_thing_new_with_forms();
  struct tinywot_form const *form = NULL;
  tinywot_status_t status = TINYWOT_STATUS_SUCCESS;

  status = tinywot_thing_dynamic_get_form(
    thing, "/a", TINYWOT_OPERATION_TYPE_INVOKEACTION, &form
  );

  TEST_ASSERT_EQUAL(TINYWOT_STATUS_ERROR_NOT_ALLOWED, status);
  TEST_ASSERT_NULL(form);

  example_thing_delete(&thing);
}

void test_tinywot_thing_dynamic_get_form_mismatch_target(void) {
  struct tinywot_thing_dynamic *thing = example_thing_new_with_forms();
  struct tinywot_form const *form = NULL;
  tinywot_status_t status = TINYWOT_STATUS_SUCCESS;

  status = tinywot_thing_dynamic_get_form(
    thing, "/?", TINYWOT_OPERATION_TYPE_READPROPERTY, &form
  );

  TEST_ASSERT_EQUAL(TINYWOT_STATUS_ERROR_NOT_FOUND, status);
  TEST_ASSERT_NULL(form);

  example_thing_delete(&thing);
}
