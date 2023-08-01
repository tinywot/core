/*
  SPDX-FileCopyrightText: 2023 Junde Yhi <junde@yhi.moe>
  SPDX-License-Identifier: MIT
*/

/*
  Implementation of the supporting library for testing TinyWoT Core.
*/

#include <stdlib.h>
#include <string.h>

#include <tinywot/core.h>
#include <unity.h>

#include <tinywot-test.h>

static tinywot_status_t form_example_foo_handler(
  struct tinywot_response *response,
  struct tinywot_request *request,
  void *context
);
static tinywot_status_t form_example_bar_handler(
  struct tinywot_response *response,
  struct tinywot_request *request,
  void *context
);
static tinywot_status_t form_example_baz_handler(
  struct tinywot_response *response,
  struct tinywot_request *request,
  void *context
);

static char const foo[] = "foo";
static char const bar[] = "bar";
static char const baz[] = "baz";

static char buffer_foo[TINYWOT_TEST_MEMORY_SIZE_BYTE] = "foo: hi!";
static char buffer_bar[TINYWOT_TEST_MEMORY_SIZE_BYTE] = "bar: hi!";
static char buffer_baz[TINYWOT_TEST_MEMORY_SIZE_BYTE] = "baz: hi!";

static struct tinywot_form const form_example_foo = {
  .target = foo,
  .allowed_operation_types = TINYWOT_OPERATION_TYPE_READPROPERTY
                             | TINYWOT_OPERATION_TYPE_WRITEPROPERTY,
  .handler = form_example_foo_handler,
  .context = NULL,
};

static struct tinywot_form const form_example_bar = {
  .target = bar,
  .allowed_operation_types = TINYWOT_OPERATION_TYPE_INVOKEACTION,
  .handler = form_example_bar_handler,
  .context = NULL,
};

static struct tinywot_form const form_example_baz = {
  .target = baz,
  .allowed_operation_types = TINYWOT_OPERATION_TYPE_SUBSCRIBEEVENT,
  .handler = form_example_baz_handler,
  .context = NULL,
};

static struct tinywot_form const form_example_list[] = {
  form_example_foo, form_example_bar, form_example_baz
};

static struct tinywot_thing_static const thing_static_example = {
  .base = {
    .forms = (struct tinywot_form *)form_example_list,
    .forms_count_n = sizeof(form_example_list) / sizeof(struct tinywot_form),
  },
};

static tinywot_status_t form_example_foo_handler(
  struct tinywot_response *response,
  struct tinywot_request *request,
  void *context
) {
  TEST_ASSERT_NOT_NULL(response);
  TEST_ASSERT_NOT_NULL(request);
  TEST_ASSERT_NULL(context);
  TEST_ASSERT_EQUAL_STRING(foo, request->target);

  switch (request->operation_type) {
    case TINYWOT_OPERATION_TYPE_WRITEPROPERTY:
      TEST_ASSERT_NOT_NULL(request->payload.content);
      TEST_ASSERT_GREATER_THAN(0, request->payload.content_length_byte);

      memcpy(
        buffer_foo,
        request->payload.content,
        request->payload.content_length_byte
      );

      /* fall through */

    case TINYWOT_OPERATION_TYPE_READPROPERTY:
      response->status = TINYWOT_RESPONSE_STATUS_OK;
      response->payload = (struct tinywot_payload) {
        .content = buffer_foo,
        .content_buffer_size_byte = TINYWOT_TEST_MEMORY_SIZE_BYTE,
        .content_length_byte = strlen(buffer_foo),
        .content_type = TINYWOT_TEST_CONTENT_TYPE_TEXT_PLAIN
      };

      break;

    default:
      TEST_FAIL();
  }

  return TINYWOT_STATUS_SUCCESS;
}

static tinywot_status_t form_example_bar_handler(
  struct tinywot_response *response,
  struct tinywot_request *request,
  void *context
) {
  TEST_ASSERT_NOT_NULL(response);
  TEST_ASSERT_NOT_NULL(request);
  TEST_ASSERT_NULL(context);

  return TINYWOT_STATUS_ERROR_NOT_IMPLEMENTED;
}

static tinywot_status_t form_example_baz_handler(
  struct tinywot_response *response,
  struct tinywot_request *request,
  void *context
) {
  TEST_ASSERT_NOT_NULL(response);
  TEST_ASSERT_NOT_NULL(request);
  TEST_ASSERT_NULL(context);

  return TINYWOT_STATUS_ERROR_NOT_IMPLEMENTED;
}

void *tinywot_test_malloc(size_t size) {
  void *mem = malloc(size);
  if (!mem) {
    TEST_ABORT();
  }

  return mem;
}

void tinywot_test_free(void *mem) {
  free(mem);
}

struct tinywot_payload *tinywot_test_payload_new(void) {
  struct tinywot_payload *pl =
    tinywot_test_malloc(sizeof(struct tinywot_payload));

  memset(pl, 0, sizeof(struct tinywot_payload));

  return pl;
}

struct tinywot_payload *tinywot_test_payload_with_memory(
  void *mem, size_t mem_size_byte
) {
  struct tinywot_payload *pl = tinywot_test_payload_new();

  pl->content = mem;
  pl->content_buffer_size_byte = mem_size_byte;

  return pl;
}

struct tinywot_payload *tinywot_test_payload_with_data(
  void *data,
  size_t data_size_byte,
  size_t content_length_byte,
  uint_fast16_t content_type
) {
  struct tinywot_payload *pl =
    tinywot_test_payload_with_memory(data, data_size_byte);

  pl->content_length_byte = content_length_byte;
  pl->content_type = content_type;

  return pl;
}

void tinywot_test_payload_free(struct tinywot_payload *pl) {
  if (pl->content) {
    tinywot_test_free(pl->content);
  }

  tinywot_test_free(pl);
}

struct tinywot_thing_base *tinywot_test_thing_base_new(void) {
  struct tinywot_thing_base *th =
    tinywot_test_malloc(sizeof(struct tinywot_thing_base));

  memset(th, 0, sizeof(struct tinywot_thing_base));

  return th;
}

struct tinywot_form const *tinywot_test_form_example_foo(void) {
  return &form_example_foo;
}

struct tinywot_form const *tinywot_test_form_example_bar(void) {
  return &form_example_bar;
}

struct tinywot_form const *tinywot_test_form_example_baz(void) {
  return &form_example_baz;
}

struct tinywot_thing_static const *tinywot_test_thing_static_example_new(
  void
) {
  return &thing_static_example;
}

struct tinywot_thing_dynamic *tinywot_test_thing_dynamic_example_new(void) {
  struct tinywot_thing_dynamic *thing =
    tinywot_test_malloc(sizeof(struct tinywot_thing_dynamic));
  struct tinywot_form *forms =
    tinywot_test_malloc(8 * sizeof(struct tinywot_form));

  memcpy(forms, form_example_list, sizeof(form_example_list));

  thing->base.forms = forms;
  thing->base.forms_count_n =
    sizeof(form_example_list) / sizeof(struct tinywot_form *);
  thing->forms_max_n = 8;

  return thing;
}

void tinywot_test_thing_dynamic_example_free(
  struct tinywot_thing_dynamic *thing
) {
  tinywot_test_free(thing->base.forms);
  tinywot_test_free(thing);
}
