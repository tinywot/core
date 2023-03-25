/*
  SPDX-FileCopyrightText: 2023 Junde Yhi <junde@yhi.moe>
  SPDX-License-Identifier: MIT
*/

#include <stdlib.h>
#include <tinywot.h>
#include <unity.h>

#include "thing.h"

static int dummy_handler(struct tinywot_scratchpad *inout, void *user_data)
{
  (void)inout;
  (void)user_data;

  return TINYWOT_SUCCESS;
}

static struct tinywot_form const forms[] = {
  {
    .name = "foo",
    .path = "/foo",
    .op = TINYWOT_OPERATION_TYPE_READPROPERTY,
    .handler = dummy_handler,
    .user_data = NULL,
  },
  {
    .name = "foo",
    .path = "/foo",
    .op = TINYWOT_OPERATION_TYPE_WRITEPROPERTY,
    .handler = dummy_handler,
    .user_data = NULL,
  },
  {
    .name = "bar",
    .path = "/bar",
    .op = TINYWOT_OPERATION_TYPE_READPROPERTY,
    .handler = dummy_handler,
    .user_data = NULL,
  },
};

void test_tinywot_thing_initialize_with_forms_normal(void)
{
  struct tinywot_thing thing;

  tinywot_thing_initialize_with_forms(
    &thing, forms, sizeof(forms) / sizeof(struct tinywot_form)
  );

  TEST_ASSERT_EQUAL_UINT(0u, thing.read_write);
  TEST_ASSERT_EQUAL_UINT(3u, thing.forms_count_n);
  TEST_ASSERT_EQUAL_UINT(3u, thing.forms_max_n);
  TEST_ASSERT_EQUAL_PTR(forms, thing.forms);
}

void test_tinywot_thing_initialize_with_memory_normal(void)
{
  struct tinywot_thing thing;
  void *memory;

  memory = malloc(256u);
  if (!memory) {
    TEST_ABORT();
  }

  tinywot_thing_initialize_with_memory(&thing, memory, 256u);

  TEST_ASSERT_EQUAL_UINT(1u, thing.read_write);
  TEST_ASSERT_EQUAL_UINT(0u, thing.forms_count_n);
  TEST_ASSERT_EQUAL_UINT(256u / sizeof(struct tinywot_form), thing.forms_max_n);
  TEST_ASSERT_EQUAL_PTR(memory, thing.forms);
}
