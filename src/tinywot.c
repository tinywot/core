/**
 * \internal \file tinywot.c
 * \brief TinyWoT public API implementations.
 *
 * \copyright
 * SPDX-FileCopyrightText: 2021 Junde Yhi <junde@yhi.moe>
 * SPDX-License-Identifier: MIT
 */

#include "tinywot.h"

/**
 * \internal \brief The `strcmp` to use.
 *
 * On AVR platforms where program space strings are available, utilizing this
 * can save a lot of room in RAM occupied by strings.
 */
#if defined(__AVR_ARCH__) && defined(TINYWOT_USE_PROGMEM)
#include <avr/pgmspace.h>
#define _strcmp strcmp_P
#else
#include <string.h>
#define _strcmp strcmp
#endif

TinyWoTResponse tinywot_process(const TinyWoTThing *const thing,
                                const TinyWoTRequest *const request) {
  TinyWoTResponse response = {0};

  for (size_t i = 0; i < thing->handlers_size; i++) {
    const TinyWoTHandler *const handler = &(thing->handlers[i]);

    if (!handler->path)
      continue;

    if (_strcmp(request->path, handler->path) != 0)
      continue;

    if (!(request->op & handler->ops)) {
      response.status = TINYWOT_RESPONSE_STATUS_METHOD_NOT_ALLOWED;
      break;
    }

    if (!handler->func) {
      response.status = TINYWOT_RESPONSE_STATUS_NOT_IMPLEMENTED;
      break;
    }

    response = handler->func((TinyWoTRequest *)request, handler->ctx);

    break;
  }

  if (response.status == TINYWOT_RESPONSE_STATUS_UNKNOWN)
    response.status = TINYWOT_RESPONSE_STATUS_UNSUPPORTED;

  return response;
}
