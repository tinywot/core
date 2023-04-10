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

#define DUMMY_USER_DATA_FOO_READPROPERTY ((char const *)"-foof--f--")
#define DUMMY_USER_DATA_FOO_WRITEPROPERTY ((char const *)"-foofo-f--")
#define DUMMY_USER_DATA_BAR_READPROPERTY ((char const *)"-barb--b--")

static int dummy_handler_foo_readproperty(
  struct tinywot_scratchpad *inout, void *user_data
)
{
  TEST_ASSERT_NOT_NULL(inout);
  TEST_ASSERT_EQUAL_STRING(DUMMY_USER_DATA_FOO_READPROPERTY, user_data);

  size_t data_length = strlen(DUMMY_USER_DATA_FOO_READPROPERTY) + 1;

  if (!inout->read_write) {
    return TINYWOT_ERROR_NOT_ALLOWED;
  }

  if (inout->size_byte < data_length) {
    return TINYWOT_ERROR_NOT_ENOUGH_MEMORY;
  }

  inout->valid_byte = data_length;
  memcpy(inout->data, DUMMY_USER_DATA_FOO_READPROPERTY, data_length);

  return TINYWOT_SUCCESS;
}

static int dummy_handler_foo_writeproperty(
  struct tinywot_scratchpad *inout, void *user_data
)
{
  TEST_ASSERT_NOT_NULL(inout);
  TEST_ASSERT_EQUAL_STRING(DUMMY_USER_DATA_FOO_WRITEPROPERTY, user_data);

  return TINYWOT_SUCCESS;
}

static int dummy_handler_bar_readproperty(
  struct tinywot_scratchpad *inout, void *user_data
)
{
  TEST_ASSERT_NOT_NULL(inout);
  TEST_ASSERT_EQUAL_STRING(DUMMY_USER_DATA_BAR_READPROPERTY, user_data);

  size_t data_length = strlen(DUMMY_USER_DATA_BAR_READPROPERTY) + 1;

  if (!inout->read_write) {
    return TINYWOT_ERROR_NOT_ALLOWED;
  }

  if (inout->size_byte < data_length) {
    return TINYWOT_ERROR_NOT_ENOUGH_MEMORY;
  }

  inout->valid_byte = data_length;
  memcpy(inout->data, DUMMY_USER_DATA_BAR_READPROPERTY, data_length);

  return TINYWOT_SUCCESS;
}

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

void test_tinywot_thing_set_handler_until_full(void)
{
  struct tinywot_thing thing;
  /* We need to make the 3rd set fail. */
  void *memory = malloc(2 * sizeof(struct tinywot_form));
  int r = 0;

  if (!memory) {
    TEST_ABORT();
  }

  tinywot_thing_initialize_with_memory(
    &thing, memory, 2 * sizeof(struct tinywot_form)
  );

  r = tinywot_thing_set_handler(
    &thing,
    "foo",
    "/foo",
    TINYWOT_OPERATION_TYPE_READPROPERTY,
    dummy_handler_foo_readproperty,
    NULL
  );
  TEST_ASSERT_EQUAL_INT(TINYWOT_SUCCESS, r);

  r = tinywot_thing_set_handler(
    &thing,
    "foo",
    "/foo",
    TINYWOT_OPERATION_TYPE_WRITEPROPERTY,
    dummy_handler_foo_writeproperty,
    NULL
  );
  TEST_ASSERT_EQUAL_INT(TINYWOT_SUCCESS, r);

  r = tinywot_thing_set_handler(
    &thing,
    "bar",
    "/bar",
    TINYWOT_OPERATION_TYPE_READPROPERTY,
    dummy_handler_foo_readproperty,
    NULL
  );
  TEST_ASSERT_EQUAL_INT(TINYWOT_ERROR_NOT_ENOUGH_MEMORY, r);

  free(memory);
}

void test_tinywot_thing_do(void)
{
  struct tinywot_thing thing;
  struct tinywot_scratchpad scratchpad;
  void *memory = malloc(256);
  int r = 0;

  if (!memory) {
    TEST_ABORT();
  }

  tinywot_thing_initialize_with_forms(&thing, forms, sizeof(forms) / sizeof(struct tinywot_form));
  tinywot_scratchpad_initialize(&scratchpad);
  scratchpad.read_write = true;
  scratchpad.size_byte = 256;
  scratchpad.data = memory;

  r = tinywot_thing_do(&thing, "foo", "/foo", TINYWOT_OPERATION_TYPE_READPROPERTY, &scratchpad);
  TEST_ASSERT_EQUAL_INT(TINYWOT_SUCCESS, r);
  TEST_ASSERT_EQUAL_UINT(strlen(DUMMY_USER_DATA_FOO_READPROPERTY) + 1, scratchpad.valid_byte);
  TEST_ASSERT_EQUAL_STRING(DUMMY_USER_DATA_FOO_READPROPERTY, scratchpad.data);

  r = tinywot_thing_do(&thing, NULL, "/foo", TINYWOT_OPERATION_TYPE_WRITEPROPERTY, &scratchpad);
  TEST_ASSERT_EQUAL_INT(TINYWOT_SUCCESS, r);

  r = tinywot_thing_do(&thing, "bar", NULL, TINYWOT_OPERATION_TYPE_READPROPERTY, &scratchpad);
  TEST_ASSERT_EQUAL_INT(TINYWOT_SUCCESS, r);
  TEST_ASSERT_EQUAL_UINT(strlen(DUMMY_USER_DATA_BAR_READPROPERTY) + 1, scratchpad.valid_byte);
  TEST_ASSERT_EQUAL_STRING(DUMMY_USER_DATA_BAR_READPROPERTY, scratchpad.data);

  r = tinywot_thing_do(&thing, "baz", NULL, TINYWOT_OPERATION_TYPE_READPROPERTY, &scratchpad);
  TEST_ASSERT_EQUAL_INT(TINYWOT_ERROR_NOT_FOUND, r);

  free(memory);
}

void test_tinywot_thing_process_request(void)
{
  /* This would be the main scratchpad provided by the user application. */
  struct tinywot_scratchpad scratchpad;

  struct tinywot_thing thing;
  struct tinywot_request request;
  struct tinywot_response response;
  void *memory = malloc(256);
  int r = 0;

  if (!memory) {
    TEST_ABORT();
  }

  tinywot_scratchpad_initialize(&scratchpad);
  tinywot_thing_initialize_with_forms(&thing, forms, sizeof(forms) / sizeof(struct tinywot_form));
  memset(&request, 0, sizeof(struct tinywot_request));
  memset(&response, 0, sizeof(struct tinywot_response));

  /* Configure scratchpad. This would be done in the user application. */
  scratchpad.read_write = true;
  scratchpad.size_byte = 256;
  scratchpad.data = memory;

  /* Note that both operands need to be read-write. */
  request.path.read_write = true;

  /* Splitting the main scratchpad to a path one and a content one. This would
     be done in protocol implementation libraries. */
  r = tinywot_scratchpad_split(&scratchpad, &request.path, strlen("/foo") + 1);
  TEST_ASSERT_EQUAL_INT(TINYWOT_SUCCESS, r);
  TEST_ASSERT_TRUE(request.path.read_write);
  TEST_ASSERT_EQUAL_UINT(strlen("/foo") + 1, request.path.size_byte);
  TEST_ASSERT_EQUAL_UINT(0, request.path.valid_byte);
  TEST_ASSERT_NOT_NULL(request.path.data);

  /* Mock a request. Path uses the split scratchpad above. */
  request.path.valid_byte = strlen("/foo") + 1;
  memcpy(request.path.data, "/foo", strlen("/foo") + 1);
  request.op = TINYWOT_OPERATION_TYPE_READPROPERTY;
  request.content = &scratchpad;

  /* Invoke and examine the result. */
  r = tinywot_thing_process_request(&thing, &request, &response);
  TEST_ASSERT_EQUAL_INT(TINYWOT_SUCCESS, r);
  TEST_ASSERT_EQUAL_INT(TINYWOT_RESPONSE_STATUS_OK, response.status);
  TEST_ASSERT_NOT_NULL(response.content);

  TEST_ASSERT_EQUAL_UINT(strlen(DUMMY_USER_DATA_FOO_READPROPERTY) + 1, response.content->valid_byte);
  TEST_ASSERT_EQUAL_STRING(DUMMY_USER_DATA_FOO_READPROPERTY, response.content->data);

  free(memory);
}
