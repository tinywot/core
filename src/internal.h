/*
  SPDX-FileCopyrightText: 2022 Junde Yhi <junde@yhi.moe>
  SPDX-License-Identifier: MIT
*/

/*!
  \internal \file
  \brief TinyWoT internal API definitions.
*/

#ifndef TINYWOT_COMMON_H
#define TINYWOT_COMMON_H

#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

__attribute__((__noreturn__)) void
tinywot_fatal_error_handler(char const *file, size_t line, unsigned int code);

#ifdef TINYWOT_ENABLE_CONTRACTS
#define TINYWOT_FATAL_ERROR_CODE_UNREACHABLE_STATEMENT_REACHED 0u
#define TINYWOT_FATAL_ERROR_CODE_ASSERTION_FAILURE 1u
#define TINYWOT_FATAL_ERROR_CODE_PRECONDITION_FAILURE 2u
#define TINYWOT_FATAL_ERROR_CODE_POSTCONDITION_FAILURE 3u

#define TINYWOT_UNREACHABLE() \
  tinywot_fatal_error_handler( \
    __FILE__, \
    __LINE__, \
    TINYWOT_FATAL_ERROR_CODE_UNREACHABLE_STATEMENT_REACHED \
  )

#define TINYWOT_ASSERT(expr) \
  do { \
    if (!(expr)) { \
      tinywot_fatal_error_handler( \
        __FILE__, \
        __LINE__, \
        TINYWOT_FATAL_ERROR_CODE_ASSERTION_FAILURE \
      ); \
    } \
  } while (0)

#define TINYWOT_REQUIRE(expr) \
  do { \
    if (!(expr)) { \
      tinywot_fatal_error_handler( \
        __FILE__, \
        __LINE__, \
        TINYWOT_FATAL_ERROR_CODE_PRECONDITION_FAILURE \
      ); \
    } \
  } while (0)

#define TINYWOT_ENSURE(expr) \
  do { \
    if (!(expr)) { \
      tinywot_fatal_error_handler( \
        __FILE__, \
        __LINE__, \
        TINYWOT_FATAL_ERROR_CODE_POSTCONDITION_FAILURE \
      ); \
    } \
  } while (0)

#else
#define TINYWOT_UNREACHABLE(expr)
#define TINYWOT_ASSERT(expr)
#define TINYWOT_REQUIRE(expr)
#define TINYWOT_ENSURE(expr)
#endif /* ifdef TINYWOT_ENABLE_CONTRACTS */

#ifdef TINYWOT_ENABLE_DYNAMIC_MEMORY
void *tinywot_malloc(size_t size);
void tinywot_free(void *ptr);
#endif /* ifdef TINYWOT_ENABLE_DYNAMIC_MEMORY */

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
int tinywot_strcmp(char const *lhs, char const *rhs);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* ifndef TINYWOT_COMMON_H */
