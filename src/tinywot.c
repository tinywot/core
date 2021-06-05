/**
 * \internal \file tinywot.c
 * \brief TinyWoT public API implementations.
 *
 * \copyright
 * SPDX-FileCopyrightText: 2021 Junde Yhi <junde@yhi.moe>
 * SPDX-License-Identifier: MIT
 */

#include "tinywot.h"

TinyWoTResponse tinywot_process_request(const TinyWoTConfig *const config,
                                        const TinyWoTThing *const thing,
                                        const TinyWoTRequest *const request) {
  TinyWoTResponse response = {0};

  /* Sorry, but all / multiple read / write are not supported yet. */
  if (request->op == TINYWOT_OPERATION_TYPE_READ_ALL_PROPERTIES ||
      request->op == TINYWOT_OPERATION_TYPE_WRITE_ALL_PROPERTIES ||
      request->op == TINYWOT_OPERATION_TYPE_READ_MULTIPLE_PROPERTIES ||
      request->op == TINYWOT_OPERATION_TYPE_WRITE_MULTIPLE_PROPERTIES) {
    response.status = TINYWOT_RESPONSE_STATUS_NOT_IMPLEMENTED;
    return response;
  }

  for (size_t i = 0; i < thing->handlers_size; i++) {
    const TinyWoTHandler *const handler = &(thing->handlers[i]);

    if (config->strcmp(request->path, handler->path) != 0) {
      continue;
    }

    if (!(request->op & handler->ops)) {
      response.status = TINYWOT_RESPONSE_STATUS_METHOD_NOT_ALLOWED;
      break;
    }

    if (!handler->func) {
      response.status = TINYWOT_RESPONSE_STATUS_NOT_IMPLEMENTED;
      break;
    }

    response = handler->func(request);

    break;
  }

  if (response.status == TINYWOT_RESPONSE_STATUS_UNKNOWN)
    response.status = TINYWOT_RESPONSE_STATUS_UNSUPPORTED;

  return response;
}
