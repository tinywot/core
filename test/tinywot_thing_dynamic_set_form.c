/*
  SPDX-FileCopyrightText: 2023 Junde Yhi <junde@yhi.moe>
  SPDX-License-Identifier: MIT
*/

#include "example_thing.h"
#include <string.h>
#include <tinywot/core.h>
#include <unity.h>

void test_tinywot_thing_dynamic_set_form_normal(void) {
  struct tinywot_thing_dynamic *thing = example_thing_new();
  tinywot_status_t status = TINYWOT_STATUS_SUCCESS;

  struct tinywot_form form_to_set = {
    .target = "/z",
    .allowed_operation_types = TINYWOT_OPERATION_TYPE_INVOKEACTION,
    .handler = NULL,
    .context = NULL,
  };

  status = tinywot_thing_dynamic_set_form(thing, &form_to_set);

  TEST_ASSERT_EQUAL(TINYWOT_STATUS_SUCCESS, status);

  example_thing_delete(&thing);
}
