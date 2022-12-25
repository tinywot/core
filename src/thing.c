/*
  SPDX-FileCopyrightText: 2021-2022 Junde Yhi <junde@yhi.moe>
  SPDX-License-Identifier: MIT
*/

/*!
  \internal \file
*/

#include "common.h"
#include <stddef.h>

#include <tinywot.h>

static int find_handler(
  const struct tinywot_thing *self,
  const char *name,
  enum tinywot_operation_type op,
  struct tinywot_handler **handler
) {
  int status = TINYWOT_NOT_FOUND;
  size_t i = 0;

  *handler = NULL;

  for (i = 0; i < self->handlers_size; i += 1) {
    if (tinywot_strcmp(name, self->handlers[i].name) != 0) {
      continue;
    }

    if (op != self->handlers[i].op) {
      if (!*handler) {
        status = TINYWOT_NOT_ALLOWED;
      }

      continue;
    }

    *handler = &self->handlers[i];
    status = TINYWOT_SUCCESS;

    /* continue; */
  }

  return status;
}

static int tinywot_thing_do(
  const struct tinywot_thing *self,
  const char *name,
  enum tinywot_operation_type op,
  const struct tinywot_scratchpad *input,
  struct tinywot_scratchpad *output
) {
  struct tinywot_handler *handler = NULL;
  int status = 0;

  status = find_handler(self, name, op, &handler);

  if (status != TINYWOT_SUCCESS) {
    return status;
  }

  if (!handler) {
    return TINYWOT_NOT_FOUND;
  }

  if (!handler->func) {
    return TINYWOT_NOT_IMPLEMENTED;
  }

  return handler->func(input, output);
}

int tinywot_thing_read_property(
  const struct tinywot_thing *self,
  const char *name,
  struct tinywot_scratchpad *output
) {
  if (!self || !name || !output) {
    return TINYWOT_NULL_POINTER_ERROR;
  }

  return tinywot_thing_do(
    self, name, TINYWOT_OPERATION_TYPE_PROPERTY_READ, NULL, output
  );
}

int tinywot_thing_write_property(
  const struct tinywot_thing *self,
  const char *name,
  const struct tinywot_scratchpad *input
) {
  if (!self || !name || !input) {
    return TINYWOT_NULL_POINTER_ERROR;
  }

  return tinywot_thing_do(
    self, name, TINYWOT_OPERATION_TYPE_PROPERTY_WRITE, input, NULL
  );
}

int tinywot_thing_invoke_action(
  const struct tinywot_thing *self,
  const char *name,
  const struct tinywot_scratchpad *input
) {
  if (!self || !name || !input) {
    return TINYWOT_NULL_POINTER_ERROR;
  }

  return tinywot_thing_do(
    self, name, TINYWOT_OPERATION_TYPE_INVOKE_ACTION, input, NULL
  );
}

int tinywot_thing_process_request(
  struct tinywot_thing *thing,
  const struct tinywot_request *request,
  struct tinywot_response *response
) {
  int status = 0;

  if (!thing || !request || !response) {
    return TINYWOT_NULL_POINTER_ERROR;
  }

  status = tinywot_thing_do(
    thing, request->name, request->op, request->content, response->content
  );

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

    case TINYWOT_NULL_POINTER_ERROR: /* fall through */
    case TINYWOT_GENERAL_ERROR:      /* fall through */
    default:
      response->status = TINYWOT_RESPONSE_STATUS_ERROR;
      break;
  }

  return status;
}
