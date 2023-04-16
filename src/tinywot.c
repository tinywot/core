/*
  SPDX-FileCopyrightText: 2021-2023 Junde Yhi <junde@yhi.moe>
  SPDX-License-Identifier: MIT
*/

/*!
  \internal \file
  \brief TinyWoT public API implementations.
*/

#include <stdbool.h>
#include <string.h>

#include <tinywot.h>

void tinywot_scratchpad_initialize(struct tinywot_scratchpad *self)
{
  memset(self, 0, sizeof(struct tinywot_scratchpad));
  self->type_hint = TINYWOT_CONTENT_TYPE_UNKNOWN;
}

int tinywot_scratchpad_split(
  struct tinywot_scratchpad *restrict left,
  struct tinywot_scratchpad *restrict right,
  size_t to_split_byte
)
{
  /* The memory region to subdivide must be an available space. */
  if (to_split_byte > left->size_byte || to_split_byte > (left->size_byte - left->valid_byte)) {
    return TINYWOT_ERROR_NOT_ENOUGH_MEMORY;
  }

  /* Check R/W before doing anything. */
  if (!left->read_write || !right->read_write) {
    return TINYWOT_ERROR_NOT_ALLOWED;
  }

  /* Clear the target memory. */
  memset(right, 0, sizeof(struct tinywot_scratchpad));

  /* We've checked this above, so right must be R/W. */
  right->read_write = true;

  /* Copy the type hint. */
  right->type_hint = left->type_hint;

  /* The original scratchpad has available space reduced by the size of the new
     scratchpad, which is specified in the parameter. */
  left->size_byte -= to_split_byte;
  right->size_byte = to_split_byte;

  /* The original valid byte count is unchanged, but is 0 for the new one. */
  right->valid_byte = 0;

  /* Set RHS data pointer; the offset is LHS size at this point. */
  right->data = left->data + left->size_byte;

  /* The update() pointer is not copied, as it might not be applicable to the
     other scratchpad. */

  return TINYWOT_SUCCESS;
}

void tinywot_thing_initialize_with_forms(
  struct tinywot_thing *self,
  struct tinywot_form const *forms,
  size_t forms_size_n
)
{
  memset(self, 0, sizeof(struct tinywot_thing));
  /* self->read_write = false; */ /* it already is */
  self->forms_count_n = forms_size_n;
  self->forms_max_n = forms_size_n;
  self->forms = forms;
}

void tinywot_thing_initialize_with_memory(
  struct tinywot_thing *self, void *memory, size_t memory_size_byte
)
{
  memset(self, 0, sizeof(struct tinywot_thing));
  self->read_write = true;
  /* self->forms_count_n = 0; */ /* it already is */
  self->forms_max_n = memory_size_byte / sizeof(struct tinywot_form);
  self->forms = memory;
}

int tinywot_thing_get_form(
  struct tinywot_thing const *self,
  char const *target,
  uint_least32_t allowed_ops,
  struct tinywot_form const **form
)
{
  return TINYWOT_ERROR_NOT_IMPLEMENTED;
}

int tinywot_thing_set_form(
  struct tinywot_thing const *self, struct tinywot_form const *form
)
{
  return TINYWOT_ERROR_NOT_IMPLEMENTED;
}

int tinywot_thing_do(
  struct tinywot_thing const *self,
  char const *target,
  uint_least32_t op,
  struct tinywot_scratchpad *inout
)
{
  return TINYWOT_ERROR_NOT_IMPLEMENTED;
}

int tinywot_thing_process_request(
  struct tinywot_thing const *self,
  struct tinywot_request *request,
  struct tinywot_response *response
)
{
  int status = TINYWOT_ERROR_GENERAL;

  status = tinywot_thing_do(
    self, (char const *)request->target.data, request->op, request->content
  );

  /* Map TinyWoT status codes to Response status codes. */
  switch (status) {
    case TINYWOT_SUCCESS:
      response->status = TINYWOT_RESPONSE_STATUS_OK;
      break;

    case TINYWOT_ERROR_NOT_FOUND:
      response->status = TINYWOT_RESPONSE_STATUS_NOT_FOUND;
      break;

    case TINYWOT_ERROR_NOT_IMPLEMENTED:
      response->status = TINYWOT_RESPONSE_STATUS_NOT_SUPPORTED;
      break;

    case TINYWOT_ERROR_NOT_ALLOWED:
      response->status = TINYWOT_RESPONSE_STATUS_NOT_ALLOWED;
      break;

    case TINYWOT_ERROR_GENERAL: /* fall through */
    case TINYWOT_ERROR_NOT_ENOUGH_MEMORY: /* fall through */
    default:
      response->status = TINYWOT_RESPONSE_STATUS_ERROR;
      break;
  }

  /* Shallow copy the scratchpad, as scratchpads are used as both inputs and
     outputs throughout the library. */
  response->content = request->content;

  return status;
}

int tinywot_servient_process(
  struct tinywot_servient const *self, struct tinywot_scratchpad *scratchpad
)
{
  struct tinywot_request request = { 0 };
  struct tinywot_response response = { 0 };
  int status = 0;

  /* Fit the supplied scratchpad in; this will be used thoughout the Servient
     processing procedure. */
  request.content = scratchpad;

  /* Ensure the scratchpad is clean before doing anything else. */
  tinywot_scratchpad_initialize(request.content);

  status = self->protocol.receive(&request, &self->io);
  if (tinywot_is_error(status)) {
    return status;
  }

  status = tinywot_thing_process_request(&self->thing, &request, &response);
  if (tinywot_is_error(status)) {
    return status;
  }

  status = self->protocol.send(&response, &self->io);
  if (tinywot_is_error(status)) {
    return status;
  }

  return TINYWOT_SUCCESS;
}
