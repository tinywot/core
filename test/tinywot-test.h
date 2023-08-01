/*
  SPDX-FileCopyrightText: 2023 Junde Yhi <junde@yhi.moe>
  SPDX-License-Identifier: MIT
*/

/*
  Definition of the supporting library for testing TinyWoT Core.
*/

#ifndef TINYWOT_TEST_H
#define TINYWOT_TEST_H

#include <stddef.h>
#include <stdint.h>

#define TINYWOT_TEST_CONTENT_TYPE_TEXT_PLAIN 0

#define TINYWOT_TEST_SMALL_MEMORY_SIZE_BYTE  16
#define TINYWOT_TEST_MEMORY_SIZE_BYTE        4096

void *tinywot_test_malloc(size_t size);
void tinywot_test_free(void *mem);

struct tinywot_payload *tinywot_test_payload_new(void);
struct tinywot_payload *tinywot_test_payload_with_memory(
  void *data,
  size_t data_size_byte
);
struct tinywot_payload *tinywot_test_payload_with_data(
  void *data,
  size_t data_size_byte,
  size_t content_length_byte,
  uint_fast16_t content_type
);
void tinywot_test_payload_free(struct tinywot_payload *pl);

struct tinywot_form const *tinywot_test_form_example_foo(void);
struct tinywot_form const *tinywot_test_form_example_bar(void);
struct tinywot_form const *tinywot_test_form_example_baz(void);

struct tinywot_thing_static const *tinywot_test_thing_static_example_new(void);

void tinywot_test_thing_dynamic_example_free(
  struct tinywot_thing_dynamic *thing
);
struct tinywot_thing_dynamic *tinywot_test_thing_dynamic_example_new(void)
  __attribute__((malloc));

#endif /* TINYWOT_TEST_H */
