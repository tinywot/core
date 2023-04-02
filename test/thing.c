/*
  SPDX-FileCopyrightText: 2023 Junde Yhi <junde@yhi.moe>
  SPDX-License-Identifier: MIT
*/

#include <stdlib.h>
#include <string.h>
#include <tinywot.h>
#include <unity.h>

/* A mock thing is defined below; distinctive data are used to ensure that the
   correct information is returned by the library. */

static int dummy_handler_foo_readproperty(
  struct tinywot_scratchpad *inout, void *user_data
)
{
  (void)inout;
  (void)user_data;

  return TINYWOT_SUCCESS;
}

static int dummy_handler_foo_writeproperty(
  struct tinywot_scratchpad *inout, void *user_data
)
{
  (void)inout;
  (void)user_data;

  return TINYWOT_SUCCESS;
}

static int dummy_handler_bar_readproperty(
  struct tinywot_scratchpad *inout, void *user_data
)
{
  (void)inout;
  (void)user_data;

  return TINYWOT_SUCCESS;
}

#define DUMMY_USER_DATA_FOO_READPROPERTY ((char const *)"-foof--f--")
#define DUMMY_USER_DATA_FOO_WRITEPROPERTY ((char const *)"-foofo-f--")
#define DUMMY_USER_DATA_BAR_READPROPERTY ((char const *)"-barb--b--")

static struct tinywot_form const forms[] = {
  {
    .name = "foo",
    .path = "/foo",
    .op = TINYWOT_OPERATION_TYPE_READPROPERTY,
    .handler = dummy_handler_foo_readproperty,
    .user_data = (void *)DUMMY_USER_DATA_FOO_READPROPERTY,
  },
  {
    .name = "foo",
    .path = "/foo",
    .op = TINYWOT_OPERATION_TYPE_WRITEPROPERTY,
    .handler = dummy_handler_foo_writeproperty,
    .user_data = (void *)DUMMY_USER_DATA_FOO_WRITEPROPERTY,
  },
  {
    .name = "bar",
    .path = "/bar",
    .op = TINYWOT_OPERATION_TYPE_READPROPERTY,
    .handler = dummy_handler_bar_readproperty,
    .user_data = (void *)DUMMY_USER_DATA_BAR_READPROPERTY,
  },
};

/* Test suites start here. */

void test_tinywot_thing_initialize_with_forms_normal(void)
{
  struct tinywot_thing thing;

  tinywot_thing_initialize_with_forms(
    &thing, forms, sizeof(forms) / sizeof(struct tinywot_form)
  );

  TEST_ASSERT_EQUAL_UINT(0, thing.read_write);
  TEST_ASSERT_EQUAL_UINT(3, thing.forms_count_n);
  TEST_ASSERT_EQUAL_UINT(3, thing.forms_max_n);
  TEST_ASSERT_EQUAL_PTR(forms, thing.forms);
}

void test_tinywot_thing_initialize_with_memory_normal(void)
{
  struct tinywot_thing thing;
  void *memory = malloc(256);

  if (!memory) {
    TEST_ABORT();
  }

  tinywot_thing_initialize_with_memory(&thing, memory, 256);

  TEST_ASSERT_EQUAL_UINT(1, thing.read_write);
  TEST_ASSERT_EQUAL_UINT(0, thing.forms_count_n);
  TEST_ASSERT_EQUAL_UINT(256u / sizeof(struct tinywot_form), thing.forms_max_n);
  TEST_ASSERT_EQUAL_PTR(memory, thing.forms);

  free(memory);
}

void test_tinywot_thing_get_handler_normal(void)
{
  struct tinywot_thing thing;
  tinywot_form_handler_t *handler;
  void *user_data;
  int r = 0;

  tinywot_thing_initialize_with_forms(
    &thing, forms, sizeof(forms) / sizeof(struct tinywot_form)
  );

  /* Have these filled with special values so that we can know if they are
     touched or not. */
  memset(&handler, 0xaa, sizeof(tinywot_form_handler_t *));
  memset(&user_data, 0xaa, sizeof(void *));

  r = tinywot_thing_get_handler(
    &thing,
    NULL,
    NULL,
    TINYWOT_OPERATION_TYPE_WRITEPROPERTY,
    &handler,
    &user_data
  );
  TEST_ASSERT_EQUAL_INT(TINYWOT_SUCCESS, r);
  TEST_ASSERT_EQUAL_PTR(dummy_handler_foo_writeproperty, handler);
  TEST_ASSERT_EQUAL_STRING(DUMMY_USER_DATA_FOO_WRITEPROPERTY, user_data);

  r = tinywot_thing_get_handler_by_name(
    &thing, "bar", TINYWOT_OPERATION_TYPE_READPROPERTY, &handler, &user_data
  );
  TEST_ASSERT_EQUAL_INT(TINYWOT_SUCCESS, r);
  TEST_ASSERT_EQUAL_PTR(dummy_handler_bar_readproperty, handler);
  TEST_ASSERT_EQUAL_STRING(DUMMY_USER_DATA_BAR_READPROPERTY, user_data);

  r = tinywot_thing_get_handler_by_path(
    &thing, "/foo", TINYWOT_OPERATION_TYPE_READPROPERTY, &handler, &user_data
  );
  TEST_ASSERT_EQUAL_INT(TINYWOT_SUCCESS, r);
  TEST_ASSERT_EQUAL_PTR(dummy_handler_foo_readproperty, handler);
  TEST_ASSERT_EQUAL_STRING(DUMMY_USER_DATA_FOO_READPROPERTY, user_data);
}

void test_tinywot_thing_get_handler_non_existent(void)
{
  struct tinywot_thing thing;
  tinywot_form_handler_t *handler;
  void *user_data, *nonsense;
  int r = 0;

  tinywot_thing_initialize_with_forms(
    &thing, forms, sizeof(forms) / sizeof(struct tinywot_form)
  );

  /* Have these filled with special values so that we can know if they are
     touched or not. */
  memset(&handler, 0xaa, sizeof(tinywot_form_handler_t *));
  memset(&user_data, 0xaa, sizeof(void *));
  memset(&nonsense, 0xaa, sizeof(void *));

  r = tinywot_thing_get_handler(
    &thing, NULL, NULL, TINYWOT_OPERATION_TYPE_UNKNOWN, &handler, &user_data
  );
  TEST_ASSERT_EQUAL_INT(TINYWOT_ERROR_NOT_FOUND, r);
  TEST_ASSERT_EQUAL_PTR(nonsense, handler);
  TEST_ASSERT_EQUAL_PTR(nonsense, user_data);

  r = tinywot_thing_get_handler_by_name(
    &thing, "baa", TINYWOT_OPERATION_TYPE_READPROPERTY, &handler, &user_data
  );
  TEST_ASSERT_EQUAL_INT(TINYWOT_ERROR_NOT_FOUND, r);
  TEST_ASSERT_EQUAL_PTR(nonsense, handler);
  TEST_ASSERT_EQUAL_PTR(nonsense, user_data);

  r = tinywot_thing_get_handler_by_path(
    &thing, "/foo", TINYWOT_OPERATION_TYPE_INVOKEACTION, &handler, &user_data
  );
  TEST_ASSERT_EQUAL_INT(TINYWOT_ERROR_NOT_FOUND, r);
  TEST_ASSERT_EQUAL_PTR(nonsense, handler);
  TEST_ASSERT_EQUAL_PTR(nonsense, user_data);
}
