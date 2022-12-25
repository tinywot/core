/*
  SPDX-FileCopyrightText: 2022 Junde Yhi <junde@yhi.moe>
  SPDX-License-Identifier: MIT
*/

/*!
  \internal \file
*/

#ifndef TINYWOT_COMMON_H
#define TINYWOT_COMMON_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/*!
  \brief The `strcmp` invoked by TinyWoT.

  The default implementation is `strcmp` in `string.h`. Define this function
  again to override it with your implementation. For example, if you want to use
  the AVR program memory, then implement this function to call `strcmp_P`
  instead. The signature of this function is identical to the `strcmp` function
  in `string.h` / the standard library.

  \param[in] lhs Left-hand side string.
  \param[in] rhs Right-hand side string.
  \return
    - Negative value if `lhs` appears before `rhs` in lexicographical order.
    - Zero if `lhs` and `rhs` compare equal.
    - Positive value if `lhs` appears after `rhs` in lexicographical order.
  \sa https://en.cppreference.com/w/c/string/byte/strcmp
*/
int tinywot_strcmp(const char *lhs, const char *rhs);

void *tinywot_malloc(size_t size);

void tinywot_free(void *ptr);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* ifndef TINYWOT_COMMON_H */
