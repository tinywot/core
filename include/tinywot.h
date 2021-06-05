/**
 * \file tinywot.h
 * \brief TinyWoT public APIs.
 *
 * \copyright
 * SPDX-FileCopyrightText: 2021 Junde Yhi <junde@yhi.moe>
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Well-known operation types for the Web of Things.
 *
 * These are fixed operation types specified in [Table 1, WoT Architecture 1.1]
 * (https://www.w3.org/TR/wot-architecture11/#table-operation-types), except
 * #TINYWOT_OPERATION_TYPE_UNKNOWN, which is usually used in TinyWoT to denote
 * an uninitialized #TinyWoTRequest.
 *
 * \sa TinyWoTRequest
 */
typedef enum {
  /**
   * \brief Unknown operation type.
   *
   * This is usually used to denote an uninitialized #TinyWoTRequest.
   */
  TINYWOT_OPERATION_TYPE_UNKNOWN = 0x00,

  /**
   * \brief Identifies the read operation on Property Affordances to retrieve
   * the corresponding data.
   */
  TINYWOT_OPERATION_TYPE_READ_PROPERTY = 0x01,

  /**
   * \brief Identifies the write operation on Property Affordances to update the
   * corresponding data.
   */
  TINYWOT_OPERATION_TYPE_WRITE_PROPERTY = 0x02,

  /**
   * \brief Identifies the observe operation on Property Affordances to be
   * notified with the new data when the Property was updated.
   */
  TINYWOT_OPERATION_TYPE_OBSERVE_PROPERTY = 0x04,

  /**
   * \brief Identifies the unobserve operation on Property Affordances to stop
   * the corresponding notifications.
   */
  TINYWOT_OPERATION_TYPE_UNOBSERVE_PROPERTY = 0x08,

  /**
   * \brief Identifies the invoke operation on Action Affordances to perform the
   * corresponding action.
   */
  TINYWOT_OPERATION_TYPE_INVOKE_ACTION = 0x10,

  /**
   * \brief Identifies the subscribe operation on Event Affordances to be
   * notified by the Thing when the event occurs.
   */
  TINYWOT_OPERATION_TYPE_SUBSCRIBE_EVENT = 0x20,

  /**
   * \brief Identifies the unsubscribe operation on Event Affordances to stop
   * the corresponding notifications.
   */
  TINYWOT_OPERATION_TYPE_UNSUBSCRIBE_EVENT = 0x40,

  /**
   * \brief Identifies the readallproperties operation on Things to retrieve the
   * data of all Properties in a single interaction.
   */
  TINYWOT_OPERATION_TYPE_READ_ALL_PROPERTIES = 0xf1,

  /**
   * \brief Identifies the writeallproperties operation on Things to update the
   * data of all writable Properties in a single interaction.
   */
  TINYWOT_OPERATION_TYPE_WRITE_ALL_PROPERTIES = 0xf2,

  /**
   * \brief Identifies the readmultipleproperties operation on Things to
   * retrieve the data of selected Properties in a single interaction.
   */
  TINYWOT_OPERATION_TYPE_READ_MULTIPLE_PROPERTIES = 0xe1,

  /**
   * \brief Identifies the writemultipleproperties operation on Things to update
   * the data of selected writable Properties in a single interaction.
   */
  TINYWOT_OPERATION_TYPE_WRITE_MULTIPLE_PROPERTIES = 0xe2,
} TinyWoTOperationType;

/**
 * \brief Content types / formats that TinyWoT expects.
 *
 * These values are picked from the [CoAP Content-Formats Registry](https://
 * datatracker.ietf.org/doc/html/rfc7252#section-12.3). IoT devices more or
 * less want to use one of them. The additional one,
 * #TINYWOT_CONTENT_TYPE_TD_JSON, is proposed in WoT Thing Description [here](
 * https://www.w3.org/TR/wot-thing-description11/#content-format-section) and
 * have registered in the [IANA registry](https://www.iana.org/assignments/
 * core-parameters/core-parameters.xhtml#content-formats).
 */
typedef enum {
  TINYWOT_CONTENT_TYPE_TEXT_PLAIN = 0,
  TINYWOT_CONTENT_TYPE_OCTET_STREAM = 42,
  TINYWOT_CONTENT_TYPE_JSON = 50,
  TINYWOT_CONTENT_TYPE_TD_JSON = 432,
} TinyWoTContentType;

/**
 * \brief A received request.
 *
 * This can be compared to the [`Form`](https://www.w3.org/TR/wot-thing-
 * description11/#form) type in WoT Thing Description.
 */
typedef struct {
  /**
   * \brief The type ("semantic intention") of the request.
   */
  TinyWoTOperationType op;

  /**
   * \brief The path to the resource that the request is querying.
   *
   * Unlike the [`Form`](https://www.w3.org/TR/wot-thing-description11/#form)
   * type in WoT Thing Description, where a `href` of type `anyURI` is used,
   * here only the path component is used. The other components (e.g. scheme,
   * authority) should be handled by the protocol binding implementations.
   */
  const char *path;

  /**
   * \brief The type of #content in a MIME string.
   */
  TinyWoTContentType content_type;

  /**
   * \brief The size (in bytes) of #content.
   */
  size_t content_length;

  /**
   * \brief The content (payload).
   *
   * When a #content_reader is used instead, this must be set to `NULL`.
   *
   * \sa content_reader
   */
  const unsigned char *content;

  /**
   * \brief A callback function for consuming #content lazily.
   *
   * When #content is used instead, this must be set to `NULL`.
   *
   * \param[inout] buf Where the content should be placed.
   * \param[in] buf_size The maximum size of #buf.
   * \param[in] from From which byte should this call read from.
   * \param[out] remaining The remaining content length (in bytes).
   * \return Bytes read.
   * \sa content
   */
  size_t (*content_reader)(unsigned char *const buf, const size_t buf_size,
                           const size_t from, size_t *const remaining);
} TinyWoTRequest;

/**
 * \brief Status of operations returned by handlers.
 *
 * \sa TinyWoTResponse
 */
typedef enum {
  /**
   * \brief Unknown status.
   *
   * This is usually used to indicate an uninitialized #TinyWoTResponse.
   * Handlers should not return this.
   */
  TINYWOT_RESPONSE_STATUS_UNKNOWN = 0,

  /**
   * \brief The handler has successfully processed a request.
   *
   * This can be compared to a `HTTP 200 OK`.
   */
  TINYWOT_RESPONSE_STATUS_OK,

  /**
   * \brief The handler thinks the request is malformed.
   *
   * This can be compared to a `HTTP 400 Bad Request`.
   */
  TINYWOT_RESPONSE_STATUS_BAD_REQUEST,

  /**
   * \brief The handler doesn't know how to process a request.
   *
   * This will also be returned by TinyWoT when no handler can be found. This
   * can be compared to a `HTTP 404 Not Found`.
   */
  TINYWOT_RESPONSE_STATUS_UNSUPPORTED,

  /**
   * \brief The handler cannot accept the incoming method.
   *
   * This can be compared to a `HTTP 405 Method Not Allowed`. Note that handler
   * implementors do not usually return this; instead, since TinyWoTHandler::
   * ops have already declared the acceptable interaction affordances of the
   * corresponding handler, TinyWoT will directly return this instead of
   * invoking the handler function.
   */
  TINYWOT_RESPONSE_STATUS_METHOD_NOT_ALLOWED,

  /**
   * \brief The handler has failed to process a request.
   *
   * This can be compared to a `HTTP 500 Internal Server Error`.
   */
  TINYWOT_RESPONSE_STATUS_ERROR,

  /**
   * \brief No handler is implemented for the request at the matched path.
   *
   * This can be compared to a `HTTP 501 Not Implemented`. If there is an entry
   * in TinyWoTThing::handlers matches the incoming request, but with its
   * TinyWoTHandler::func being NULL, then this will be returned by TinyWoT too.
   */
  TINYWOT_RESPONSE_STATUS_NOT_IMPLEMENTED,
} TinyWoTResponseStatus;

/**
 * \brief A response to a request.
 *
 * \sa TinyWoTRequest
 */
typedef struct {
  /**
   * \brief The status of the request.
   */
  TinyWoTResponseStatus status;

  /**
   * \brief The type of #content.
   */
  TinyWoTContentType content_type;

  /**
   * The length of #content.
   */
  size_t content_length;

  /**
   * \brief The content (payload).
   *
   * When a #content_reader is used instead, this must be set to `NULL`.
   *
   * \sa content_reader
   */
  const unsigned char *content;

  /**
   * \brief A callback function for consuming #content lazily.
   *
   * When #content is used instead, this must be set to `NULL`.
   *
   * \param[inout] buf Where the content should be placed.
   * \param[in] buf_size The maximum size of #buf.
   * \param[in] from From which byte should this call read from.
   * \param[out] remaining The remaining content length (in bytes).
   * \return Bytes read.
   * \sa content
   */
  size_t (*content_reader)(unsigned char *const buf, const size_t buf_size,
                           const size_t from, size_t *const remaining);
} TinyWoTResponse;

/**
 * \brief A tuple identifying a handler to an incoming request.
 *
 * This is used in #TinyWoTThing to describe what kinds of request it can
 * accept. An incoming #TinyWoTRequest will be matched on #path and #ops. If
 * #path matches and #ops has the incoming #TinyWoTOperationType bit set, then
 * the corresponding #func is invoked with the request.
 */
typedef struct {
  /**
   * \brief The path to a resource that #func can handle.
   */
  const char *path;

  /**
   * \brief The operation types that #func can accept.
   *
   * Except "all" and "multiple" types, #TinyWoTOperationType value can be
   * `OR`-ed (`|`) together. For example, to indicate that #func can accept
   * property read and property write operations:
   *
   * ```
   * .ops = TINYWOT_OPERATION_TYPE_READ_PROPERTY |
   * TINYWOT_OPERATION_TYPE_WRITE_PROPERTY
   * ```
   */
  const TinyWoTOperationType ops;

  /**
   * \brief The function to handle #ops at #path.
   *
   * \param[in] request The incoming request.
   * \return A response to send back.
   */
  TinyWoTResponse (*func)(const TinyWoTRequest *const request);

  /**
   * \brief Arbitrary user data (context) to pass to the handler when invoked.
   */
  void *ctx;
} TinyWoTHandler;

/**
 * \brief A (Web) Thing.
 */
typedef struct {
  /**
   * \brief A list of handlers implementing the behavior (the Thing Description)
   * of the Thing.
   */
  const TinyWoTHandler *handlers;

  /**
   * \brief The size of #handlers.
   */
  size_t handlers_size;
} TinyWoTThing;

/**
 * \brief A configuration object consisting of several platform-specific
 * interfaces.
 *
 * TinyWoT is platform-agnostic. However, there are situations where external
 * functions need to be used. To allow flexibility, they need to be supplied to
 * TinyWoT dynamically.
 */
typedef struct {
  /**
   * \brief String comparison function.
   *
   * The signature is identical to the `strcmp` in standard C library. The
   * reason you need to supply this to TinyWoT is that you may want to utilize
   * the program space on some platforms (e.g. AVR8) to store strings to save
   * the precious RAM space. In this case, a special `strcmp` needs to be used.
   * `lhs` will be in the RAM and `rhs` will be in the program space. If there
   * is no need of using program-space-specific `strcmp`, just supply a standard
   * `strcmp` here.
   */
  int (*strcmp)(const char *lhs, const char *rhs);

  /**
   * \brief String length computation function.
   *
   * The reason for supplying this function is the same as #strcmp.
   */
  size_t (*strlen)(const char *str);

  /**
   * \brief Memory copying function.
   *
   * The reason for supplying this function is the same as #strcmp.
   */
  void *(*memcpy)(void *dest, const void *src, size_t count);
} TinyWoTConfig;

/**
 * \brief Process a `request` to `thing` using `config`.
 *
 * \param[in] config  Configuration for TinyWoT to work.
 * \param[in] thing   The Thing to expose.
 * \param[in] request The request received from protocol binding libraries.
 * \return A response returned by either:
 * - the handler, when a handler can be found.
 * - this method, when no handler can be found.
 */
TinyWoTResponse tinywot_process_request(const TinyWoTConfig *const config,
                                        const TinyWoTThing *const thing,
                                        const TinyWoTRequest *const request);

#ifdef __cplusplus
}
#endif
