/*
  SPDX-FileCopyrightText: 2023 Junde Yhi <junde@yhi.moe>
  SPDX-License-Identifier: MIT
*/

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tinywot/core.h>
#include <unity.h>

struct scratchpad {
  unsigned char *data;
  size_t data_length_byte;
  size_t data_buffer_size_byte;
};

/******************** Private Text ********************/

static char const content_a_hello[] = "Hello world!";
static char const content_a_done[] = "Done";

/******************** Handlers ********************/

tinywot_status_t handler_a(
  struct tinywot_response *response,
  struct tinywot_request *request,
  void *context
) {
  TEST_ASSERT_NOT_NULL(request);
  TEST_ASSERT_NOT_NULL(response);
  TEST_ASSERT_NOT_NULL(context);

  struct tinywot_payload const *input = &request->payload;
  struct scratchpad *scratchpad = context;

  if (request->operation_type == TINYWOT_OPERATION_TYPE_READPROPERTY) {
    if (response->payload.content_buffer_size_byte < sizeof(content_a_hello)) {
      return TINYWOT_STATUS_ERROR_NOT_ENOUGH_MEMORY;
    }

    response->payload.content_type = 0;
    response->payload.content_length_byte = sizeof(content_a_hello);

    memcpy(response->payload.content, content_a_hello, sizeof(content_a_hello));
  } else if (request->operation_type == TINYWOT_OPERATION_TYPE_WRITEPROPERTY) {
    if (input->content_length_byte > scratchpad->data_buffer_size_byte) {
      return TINYWOT_STATUS_ERROR_NOT_ENOUGH_MEMORY; // xxx
    }

    memcpy(scratchpad->data, input->content, scratchpad->data_buffer_size_byte);

    if (response->payload.content_buffer_size_byte < sizeof(content_a_done)) {
      return TINYWOT_STATUS_ERROR_NOT_ENOUGH_MEMORY;
    }

    response->payload.content_type = 0;
    response->payload.content_length_byte = sizeof(content_a_done);

    memcpy(response->payload.content, content_a_done, sizeof(content_a_done));
  } else {
    TEST_FAIL();
  }

  return TINYWOT_STATUS_SUCCESS;
}

/******************** Public Interfaces ********************/

void example_thing_delete(struct tinywot_thing_dynamic **thing) {
  free((*thing)->forms);
  free(*thing);
  *thing = NULL;
}

__attribute__((__malloc__))
struct tinywot_thing_dynamic *example_thing_new(void) {
  struct tinywot_thing_dynamic *new_thing =
    malloc(sizeof(struct tinywot_thing_dynamic));

  TEST_ASSERT_NOT_NULL(new_thing);

  struct tinywot_form *new_thing_forms =
    malloc(3 * sizeof(struct tinywot_form));

  TEST_ASSERT_NOT_NULL(new_thing_forms);

  new_thing->forms = new_thing_forms;
  new_thing->forms_count_n = 0;
  new_thing->forms_max_n = 3;

  return new_thing;
}

__attribute__((__malloc__))
struct tinywot_thing_dynamic *example_thing_new_with_forms(void) {
  struct tinywot_thing_dynamic *new_thing = example_thing_new();

  new_thing->forms[0] = (struct tinywot_form) {
    .target = "/a",
    .allowed_operation_types =
      TINYWOT_OPERATION_TYPE_READPROPERTY |
      TINYWOT_OPERATION_TYPE_WRITEPROPERTY,
    .handler = handler_a,
    .context = NULL,
  };

  new_thing->forms[1] = (struct tinywot_form) {
    .target = "/b",
    .allowed_operation_types = TINYWOT_OPERATION_TYPE_READPROPERTY,
    .handler = NULL,
    .context = NULL,
  };

  new_thing->forms_count_n = 2;

  return new_thing;
}
