/*
  SPDX-FileCopyrightText: 2023 Junde Yhi <junde@yhi.moe>
  SPDX-License-Identifier: MIT
*/

/*!
  \file
  \brief Implementation for the unit test supporting library of TinyWoT
  Core.
*/

#include <stdlib.h>
#include <string.h>
#include <tinywot/core.h>
#include <unity.h>

#include "tinywot-test.h"

/* application/json */
static unsigned int const example_thing_content_type = 50;

static char const example_thing_on_response[] = "true";
static char const example_thing_off_response[] = "false";

static bool example_thing_is_on = false;

enum tinywot_status handler_property_status_read(
  struct tinywot_payload *res, struct tinywot_payload *req, void *context
) {
  (void)context;
  (void)req;

  res->content_type = example_thing_content_type;
  res->content_length_byte = example_thing_is_on ?
      sizeof(example_thing_on_response) : sizeof(example_thing_off_response);

  if (res->content_buffer_size_byte == 0) {
    res->content = (char *)(example_thing_is_on ?
      example_thing_on_response : example_thing_off_response);
  } else if (res->content_buffer_size_byte > res->content_length_byte) {
    memcpy(
      res->content,
      example_thing_is_on ?
        example_thing_on_response : example_thing_off_response,
      res->content_length_byte
    );
  } else {
    return TINYWOT_STATUS_ERROR_NOT_ENOUGH_MEMORY;
  }

  return TINYWOT_STATUS_SUCCESS;
}

enum tinywot_status handler_property_status_write(
  struct tinywot_payload *res, struct tinywot_payload *req, void *context
) {
  (void)context;

  if (req->content_type != example_thing_content_type ||
      (req->content_length_byte != 4 && req->content_length_byte != 5)) {
    return TINYWOT_STATUS_ERROR_GENERIC; /* TODO: invalid parameter */
  }

  if (memcmp(example_thing_on_response,
    req->content,
    sizeof(example_thing_on_response)
  )) {
    example_thing_is_on = true;
  } else if (memcmp(example_thing_off_response,
    req->content,
    sizeof(example_thing_off_response)
  )) {
    example_thing_is_on = false;
  } else {
    return TINYWOT_STATUS_ERROR_GENERIC; /* TODO: invalid parameter */
  }

  return handler_property_status_read(res, req, context);
}

enum tinywot_status handler_action_toggle(
  struct tinywot_payload *res, struct tinywot_payload *req, void *context
) {
  (void)context;

  example_thing_is_on = !example_thing_is_on;

  return handler_property_status_read(res, req, context);
}

static struct tinywot_form const tinywot_test_forms[] = {
  {
    .name = "status",
    .target = "/status",
    .op = TINYWOT_OPERATION_TYPE_READPROPERTY,
    .handler = handler_property_status_read,
    .context = NULL,
  },
  {
    .name = "status",
    .target = "/status",
    .op = TINYWOT_OPERATION_TYPE_WRITEPROPERTY,
    .handler = handler_property_status_write,
    .context = NULL,
  },
  {
    .name = "toggle",
    .target = "/toggle",
    .op = TINYWOT_OPERATION_TYPE_INVOKEACTION,
    .handler = handler_action_toggle,
    .context = NULL,
  },
};

void tinywot_test_free(void *mem) {
  free(mem);
}

void *tinywot_test_mallocx(size_t size) {
  void *mem = malloc(size);
  if (!mem) {
    TEST_ABORT();
  }

  return mem;
}

void *tinywot_test_mallocd(size_t size) {
  return memset(tinywot_test_mallocx(size), 0xcd, size);
}

void *tinywot_test_malloc0(size_t size) {
  return memset(tinywot_test_mallocx(size), 0x00, size);
}

void tinywot_test_tinywot_payload_delete(struct tinywot_payload *pl) {
  tinywot_test_free(pl->content);
  tinywot_test_free(pl);
}

struct tinywot_payload *tinywot_test_tinywot_payload_new(size_t size) {
  struct tinywot_payload *pl =
    tinywot_test_malloc0(sizeof(struct tinywot_payload));

  pl->content = tinywot_test_mallocd(size);
  pl->content_buffer_size_byte = size;

  return pl;
}

void tinywot_test_tinywot_thing_delete(struct tinywot_thing *thing) {
  tinywot_test_free(thing->forms);
  tinywot_test_free(thing);
}

struct tinywot_thing *tinywot_test_tinywot_thing_new(void) {
  struct tinywot_thing *thing =
    tinywot_test_malloc0(sizeof(struct tinywot_thing));

  tinywot_thing_init_dynamic(
    thing,
    tinywot_test_mallocd(sizeof(tinywot_test_forms)),
    sizeof(tinywot_test_forms)
  );

  return thing;
}

struct tinywot_thing *tinywot_test_tinywot_thing_new_example(void) {
  struct tinywot_thing *thing =
    tinywot_test_malloc0(sizeof(struct tinywot_thing));

  /* Obviously memory size and forms array size are the same, so we can
     ignore this only way of failure from this function. */
  (void) tinywot_thing_init_dynamic_from_static(
    thing,
    tinywot_test_mallocd(sizeof(tinywot_test_forms)),
    sizeof(tinywot_test_forms),
    tinywot_test_forms,
    sizeof(tinywot_test_forms)
  );

  return thing;
}
