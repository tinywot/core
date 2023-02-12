/*
  SPDX-FileCopyrightText: 2022-2023 Junde Yhi <junde@yhi.moe>
  SPDX-License-Identifier: MIT
*/

/*!
  \internal \file
  \brief Underlying interface declarations.

  This is an abstraction layer of some basic functions and internal interfaces
  used across TinyWoT. The application code can override these functions by
  defining the same function again. This is done via [weak symbol] defined in
  `base-default.c` as default implementations for these functions.

  [weak symbol]: https://en.wikipedia.org/wiki/Weak_symbol
*/

#ifndef TINYWOT_BASE_H
#define TINYWOT_BASE_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef TINYWOT_ENABLE_DYNAMIC_MEMORY
void tinywot_free(void *ptr);
void *tinywot_malloc(size_t size) __attribute__((__malloc__(tinywot_free)));
#endif /* ifdef TINYWOT_ENABLE_DYNAMIC_MEMORY */

/*!
  \brief The `strcmp` invoked by TinyWoT.

  The default implementation is `strcmp` in `string.h`. Define this function
  again to override it with your implementation. For example, if you want to
  use the AVR program memory, then implement this function to call `strcmp_P`
  instead. The signature of this function is identical to the `strcmp`
  function in `string.h` / the standard library.

  \param[in] lhs Left-hand side string.
  \param[in] rhs Right-hand side string.
  \return
    - Negative value if `lhs` appears before `rhs` in lexicographical order.
    - Zero if `lhs` and `rhs` compare equal.
    - Positive value if `lhs` appears after `rhs` in lexicographical order.
  \sa https://en.cppreference.com/w/c/string/byte/strcmp
*/
int tinywot_strcmp(char const *lhs, char const *rhs);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* ifndef TINYWOT_BASE_H */
