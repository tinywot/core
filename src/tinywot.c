/**
 * \internal \file tinywot.c
 * \brief TinyWoT public API implementations.
 *
 * \copyright
 * SPDX-FileCopyrightText: 2021 Junde Yhi <junde@yhi.moe>
 * SPDX-License-Identifier: MIT
 */

#include "tinywot.h"

TinyWoTResponse tinywot_process_request(const TinyWoTConfig * const restrict config, const TinyWoTThing * const restrict thing, const TinyWoTRequest * const restrict request)
{
  TinyWoTResponse ret = {0};

  switch (request->op) {
    case TINYWOT_OPERATION_TYPE_READ_PROPERTY:
      for (size_t i = 0; i < thing->property_read_handlers_size; i++) {
        const TinyWoTHandler * const restrict handler = &(thing->property_read_handlers[i]);

        if (config->strcmp(request->path, handler->path) == 0) {
          ret = handler->func(request);
          break;
        }
      }
      break;

    case TINYWOT_OPERATION_TYPE_WRITE_PROPERTY:
      for (size_t i = 0; i < thing->property_write_handlers_size; i++) {
        const TinyWoTHandler * const restrict handler = &(thing->property_write_handlers[i]);

        if (config->strcmp(request->path, handler->path) == 0) {
          ret = handler->func(request);
          break;
        }
      }
      break;

    case TINYWOT_OPERATION_TYPE_INVOKE_ACTION:
      for (size_t i = 0; i < thing->action_handlers_size; i++) {
        const TinyWoTHandler * const restrict handler = &(thing->action_handlers[i]);

        if (config->strcmp(request->path, handler->path) == 0) {
          ret = handler->func(request);
          break;
        }
      }
      break;

    default:
      break;
  }

  if (ret.status == TINYWOT_RESPONSE_STATUS_UNKNOWN)
    ret.status = TINYWOT_RESPONSE_STATUS_UNSUPPORTED;

  return ret;
}
