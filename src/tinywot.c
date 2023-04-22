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

void tinywot_scratchpad_initialize(struct tinywot_scratchpad *self) {
  memset(self, 0, sizeof(struct tinywot_scratchpad));
  self->type_hint = TINYWOT_CONTENT_TYPE_UNKNOWN;
}

int tinywot_scratchpad_split(
  struct tinywot_scratchpad *restrict left,
  struct tinywot_scratchpad *restrict right,
  size_t to_split_byte
) {
  /* The memory region to subdivide must be an available space. */
  if (to_split_byte > left->size_byte ||
      to_split_byte > (left->size_byte - left->valid_byte)) {
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
) {
  memset(self, 0, sizeof(struct tinywot_thing));
  /* self->read_write = false; */ /* it already is */
  self->forms_count_n = forms_size_n;
  self->forms_max_n = forms_size_n;
  self->forms = forms;
}

void tinywot_thing_initialize_with_memory(
  struct tinywot_thing *self,
  void *memory,
  size_t memory_size_byte
) {
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
) {
  for (size_t form_i = 0; form_i < self->forms_count_n; form_i += 1) {
    struct tinywot_form const *form_i_p = &self->forms[form_i];

    /* Test if the target string matches, and the requested operation type is
       included in the allowed types in the form. */
    if ((strcmp(target, form_i_p->target) == 0) &&
        !(allowed_ops & form_i_p->allowed_ops)) {
      /* Allow the user to only test the existence of form and opt-out the
         receipt of form pointer. */
      if (form) {
        *form = form_i_p;
      }

      return TINYWOT_SUCCESS;
    }
  }

  /* No result if the loop doesn't return early. */
  return TINYWOT_ERROR_NOT_FOUND;
}

int tinywot_thing_set_form(
  struct tinywot_thing *self,
  struct tinywot_form const *form
) {
  struct tinywot_form const *self_form_p = NULL;
  int status = 0;

  /* Check R/W before doing anything else. */
  if (!self->read_write) {
    return TINYWOT_ERROR_NOT_ALLOWED;
  }

  /* Check if there has been an existing form registered in the Thing. If so,
     then the content of the existing form should be replaced, instead of
     inserting a new form blindly. */
  status = tinywot_thing_get_form(
    self,
    form->target,
    form->allowed_ops,
    &self_form_p
  );
  if (tinywot_is_error(status) && status != TINYWOT_ERROR_NOT_FOUND) {
    return status;
  }

  /* If an existing one is not found, point `self_form_p` to the next available
     empty forms slot. Otherwise, `self_form_p` has been set to point to an
     existing one. */
  if (status == TINYWOT_ERROR_NOT_FOUND) {
    /* Check for available space. */
    if (self->forms_count_n >= self->forms_max_n) {
      return TINYWOT_ERROR_NOT_ENOUGH_MEMORY;
    } else {
      self_form_p = &self->forms[self->forms_count_n + 1];
      self->forms_count_n += 1;
    }
  }

  /* SEI CERT Exception EXP05-C-EX3: `tinywot_thing::read_write` is in place and
     has been checked, so the read-only qualification is still kept.

     XXX: Potentially this is a security issue, as the paddings of the struct
     are also copied and kept, in which there may be residual secrets. According
     to SEI CERT Rule DCL39-C, when the struct is later given to someone else,
     the paddings are also given, potentially leaking the secrets. If the user
     always supplies a cleared struct using `memset()`, then there isn't a
     problem to just `memcpy()`. To avoid too much complication, `memcpy()` is
     still used here. Users are noticed in documentation. */

  /* cppcheck-suppress cert-EXP05-C */
  memcpy((struct tinywot_form *)self_form_p, form, sizeof(struct tinywot_form));

  return TINYWOT_SUCCESS;
}

int tinywot_thing_do(
  struct tinywot_thing const *self,
  char const *target,
  uint_least32_t op,
  struct tinywot_scratchpad *inout
) {
  struct tinywot_form const *self_form_p = NULL;
  int status = 0;

  status = tinywot_thing_get_form(self, target, op, &self_form_p);
  if (tinywot_is_error(status)) {
    return status;
  }

  return self_form_p->handler(
    self_form_p->name,
    self_form_p->target,
    op,
    inout,
    self_form_p->user_data
  );
}

int tinywot_thing_process_request(
  struct tinywot_thing const *self,
  struct tinywot_request *request,
  struct tinywot_response *response
) {
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

    case TINYWOT_ERROR_GENERAL:           /* fall through */
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
  struct tinywot_servient const *self,
  struct tinywot_scratchpad *scratchpad
) {
  struct tinywot_request request = {0};
  struct tinywot_response response = {0};
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
