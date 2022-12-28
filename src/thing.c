/*
  SPDX-FileCopyrightText: 2021-2022 Junde Yhi <junde@yhi.moe>
  SPDX-License-Identifier: MIT
*/

/*!
  \internal \file
  \brief TinyWoT Thing method implementations.
*/

#include "internal.h"
#include <stddef.h>

#include <tinywot.h>

/******************** Public Functions ********************/

int tinywot_thing_get_handler_function(
  struct tinywot_thing const *self,
  char const *name,
  enum tinywot_operation_type op,
  tinywot_handler_function_t **func
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

    /* A user can choose to not retrieve the pointer because they only want to
       know if the handler exists. */
    if (func) {
      *func = self->handlers[i].func;
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
  int status = 0;

  TINYWOT_REQUIRE(self);
  TINYWOT_REQUIRE(name);

  status = tinywot_thing_get_handler_function(self, name, op, &func);

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
  return func(input, output);
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
  struct tinywot_thing *thing,
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
