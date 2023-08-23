/*
  SPDX-FileCopyrightText: 2023 Junde Yhi <junde@yhi.moe>
  SPDX-License-Identifier: MIT
*/

/*!
  \file
  \brief Definitions for the unit test supporting library of TinyWoT
  Core.
*/

#ifndef TINYWOT_TEST_H
#define TINYWOT_TEST_H

#include <stddef.h>
#include <stdint.h>
#include <tinywot/core.h>

#define TINYWOT_TEST_CONTENT_TYPE_TEXT_PLAIN (0U)

/*! \brief Memory size for testing insufficient memory conditions. */
#define TINYWOT_TEST_SMALL_MEMORY_SIZE_BYTE  (16U)
/*! \brief Memory size for general heap allocation. */
#define TINYWOT_TEST_MEMORY_SIZE_BYTE        (4096U)

void tinywot_test_free(void *mem);
/*! \brief Allocate memory from the heap. */
void *tinywot_test_mallocx(size_t size)
  __attribute__((malloc, returns_nonnull));
/*! \brief Allocate memory from the heap and initialize it with 0xcd. */
void *tinywot_test_mallocd(size_t size)
  __attribute__((malloc, returns_nonnull));
/*! \brief Allocate memory from the heap and initialize it with 0x00. */
void *tinywot_test_malloc0(size_t size)
  __attribute__((malloc, returns_nonnull));

void tinywot_test_tinywot_payload_delete(struct tinywot_payload *pl);
struct tinywot_payload *tinywot_test_tinywot_payload_new(size_t size)
  __attribute__((malloc, returns_nonnull));

void tinywot_test_tinywot_thing_delete(struct tinywot_thing *thing);
struct tinywot_thing *tinywot_test_tinywot_thing_new(void)
  __attribute__((malloc, returns_nonnull));
struct tinywot_thing *tinywot_test_tinywot_thing_new_example(void)
  __attribute__((malloc, returns_nonnull));

#endif /* TINYWOT_TEST_H */
