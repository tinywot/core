/*
  SPDX-FileCopyrightText: 2021-2023 Junde Yhi <junde@yhi.moe>
  SPDX-License-Identifier: MIT
*/

/*!
  \file
  \brief TinyWoT public API implementations.
*/

#include "base.h"
#include <string.h>

#include <tinywot.h>

struct tinywot_scratchpad tinywot_scratchpad_new(void) {
  return TINYWOT_SCRATCHPAD_EMPTY;
}

struct tinywot_scratchpad tinywot_scratchpad_new_with_empty_memory(
  void *ptr,
  size_t size
) {
  struct tinywot_scratchpad ret = TINYWOT_SCRATCHPAD_EMPTY;
  ret.data = ptr;
  ret.size = size;
  return ret;
}

struct tinywot_scratchpad tinywot_scratchpad_new_with_type_hint(
  void *ptr,
  size_t size,
  unsigned int type_hint
) {
  struct tinywot_scratchpad ret = TINYWOT_SCRATCHPAD_EMPTY;
  ret.data = ptr;
  ret.size = size;
  ret.type_hint = type_hint;
  return ret;
}

struct tinywot_scratchpad tinywot_scratchpad_new_with_used_memory(
  void *ptr,
  size_t size,
  size_t valid_size,
  unsigned int type_hint
) {
  struct tinywot_scratchpad ret = TINYWOT_SCRATCHPAD_EMPTY;
  ret.data = ptr;
  ret.size = size;
  ret.valid_size = valid_size;
  ret.type_hint = type_hint;
  return ret;
}

int tinywot_thing_get_handler_function(
  struct tinywot_thing const *self,
  char const *name,
  enum tinywot_operation_type op,
  tinywot_handler_function_t **func,
  void **user_data
) {
  int status = TINYWOT_NOT_FOUND;

  TINYWOT_REQUIRE(self);
  TINYWOT_REQUIRE(name);

  for (size_t i = 0; i < self->handlers_size; i += 1) {
    if (tinywot_strcmp(name, self->handlers[i].name) != 0) {
      continue;
    }

    /* We do not allow an unknown / uninitialized operation type in a thing. */
    if (op != self->handlers[i].op || op == TINYWOT_OPERATION_TYPE_UNKNOWN) {
      status = TINYWOT_NOT_ALLOWED;
      continue;
    }

    /* A user can choose to not retrieve the pointers if they only want to know
       whether the handler exists (by checking the return code). */
    if (func) {
      *func = self->handlers[i].func;
    }

    if (user_data) {
      *user_data = self->handlers[i].user_data;
    }

    status = TINYWOT_SUCCESS;

    /* No security consideration for this naive implementation. */
    break;
  }

  return status;
}

int tinywot_thing_do(
  struct tinywot_thing const *self,
  char const *name,
  enum tinywot_operation_type op,
  struct tinywot_scratchpad const *input,
  struct tinywot_scratchpad *output
) {
  tinywot_handler_function_t *func = NULL;
  void *user_data = NULL;
  int status = 0;

  TINYWOT_REQUIRE(self);
  TINYWOT_REQUIRE(name);

  status =
    tinywot_thing_get_handler_function(self, name, op, &func, &user_data);

  if (status != TINYWOT_SUCCESS) {
    return status;
  }

  TINYWOT_ASSERT(status == TINYWOT_SUCCESS);

  /* We allow null pointer here to allow an entry behave like a stub. */
  if (!func) {
    return TINYWOT_NOT_IMPLEMENTED;
  }

  /* `input` or `output` may be null, which the handler function should be aware
     of. */
  return func(input, output, user_data);
}

int tinywot_thing_read_property(
  struct tinywot_thing const *self,
  char const *name,
  struct tinywot_scratchpad *output
) {
  TINYWOT_REQUIRE(self);
  TINYWOT_REQUIRE(name);
  TINYWOT_REQUIRE(output);

  return tinywot_thing_do(
    self, name, TINYWOT_OPERATION_TYPE_READPROPERTY, NULL, output
  );
}

int tinywot_thing_write_property(
  struct tinywot_thing const *self,
  char const *name,
  struct tinywot_scratchpad const *input
) {
  TINYWOT_REQUIRE(self);
  TINYWOT_REQUIRE(name);
  TINYWOT_REQUIRE(input);

  return tinywot_thing_do(
    self, name, TINYWOT_OPERATION_TYPE_WRITEPROPERTY, input, NULL
  );
}

int tinywot_thing_invoke_action(
  struct tinywot_thing const *self,
  char const *name,
  struct tinywot_scratchpad const *input
) {
  TINYWOT_REQUIRE(self);
  TINYWOT_REQUIRE(name);
  TINYWOT_REQUIRE(input);

  return tinywot_thing_do(
    self, name, TINYWOT_OPERATION_TYPE_INVOKEACTION, input, NULL
  );
}

int tinywot_thing_process_request(
  struct tinywot_thing const *thing,
  struct tinywot_request const *request,
  struct tinywot_response *response
) {
  int status = 0;

  TINYWOT_REQUIRE(thing);
  TINYWOT_REQUIRE(request);
  TINYWOT_REQUIRE(response);

  status = tinywot_thing_do(
    thing, request->name, request->op, request->content, response->content
  );

  /* Map TinyWoT status codes to response status codes. */
  switch (status) {
    case TINYWOT_SUCCESS:
      response->status = TINYWOT_RESPONSE_STATUS_OK;
      break;

    case TINYWOT_NOT_FOUND:
      response->status = TINYWOT_RESPONSE_STATUS_NOT_FOUND;
      break;

    case TINYWOT_NOT_IMPLEMENTED:
      response->status = TINYWOT_RESPONSE_STATUS_NOT_SUPPORTED;
      break;

    case TINYWOT_NOT_ALLOWED:
      response->status = TINYWOT_RESPONSE_STATUS_NOT_ALLOWED;
      break;

    case TINYWOT_GENERAL_ERROR:
      /* fall through */

    default:
      response->status = TINYWOT_RESPONSE_STATUS_ERROR;
      break;
  }

  return status;
}
