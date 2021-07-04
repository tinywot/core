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
 * \defgroup WoTOperationType WoT Operation Types
 * \brief Well-known operation types for the Web of Things.
 *
 * These are fixed operation types specified in [Table 1, WoT Architecture 1.1]
 * (https://www.w3.org/TR/wot-architecture11/#table-operation-types), except
 * WOT_OPERATION_TYPE_UNKNOWN, which is used in TinyWoT to denote an
 * uninitialized TinyWoTRequest.
 *
 * \sa TinyWoTRequest
 *
 * @{
 */

/**
 * \brief Unknown operation type.
 *
 * This is used to denote an uninitialized TinyWoTRequest.
 */
#define WOT_OPERATION_TYPE_UNKNOWN (UINT32_C(0x0000))
/**
 * \brief Identifies the read operation on Property Affordances to retrieve
 * the corresponding data.
 */
#define WOT_OPERATION_TYPE_READ_PROPERTY (UINT32_C(0x0001))
/**
 * \brief Identifies the write operation on Property Affordances to update the
 * corresponding data.
 */
#define WOT_OPERATION_TYPE_WRITE_PROPERTY (UINT32_C(0x0002))
/**
 * \brief Identifies the observe operation on Property Affordances to be
 * notified with the new data when the Property was updated.
 */
#define WOT_OPERATION_TYPE_OBSERVE_PROPERTY (UINT32_C(0x0004))
/**
 * \brief Identifies the unobserve operation on Property Affordances to stop
 * the corresponding notifications.
 */
#define WOT_OPERATION_TYPE_UNOBSERVE_PROPERTY (UINT32_C(0x0008))
/**
 * \brief Identifies the invoke operation on Action Affordances to perform the
 * corresponding action.
 */
#define WOT_OPERATION_TYPE_INVOKE_ACTION (UINT32_C(0x0010))
/**
 * \brief Identifies the subscribe operation on Event Affordances to be
 * notified by the Thing when the event occurs.
 */
#define WOT_OPERATION_TYPE_SUBSCRIBE_EVENT (UINT32_C(0x0020))
/**
 * \brief Identifies the unsubscribe operation on Event Affordances to stop
 * the corresponding notifications.
 */
#define WOT_OPERATION_TYPE_UNSUBSCRIBE_EVENT (UINT32_C(0x0040))
/**
 * \brief Identifies the readallproperties operation on Things to retrieve the
 * data of all Properties in a single interaction.
 */
#define WOT_OPERATION_TYPE_READ_ALL_PROPERTIES (UINT32_C(0x0100))
/**
 * \brief Identifies the writeallproperties operation on Things to update the
 * data of all writable Properties in a single interaction.
 */
#define WOT_OPERATION_TYPE_WRITE_ALL_PROPERTIES (UINT32_C(0x0200))
/**
 * \brief Identifies the readmultipleproperties operation on Things to
 * retrieve the data of selected Properties in a single interaction.
 */
#define WOT_OPERATION_TYPE_READ_MULTIPLE_PROPERTIES (UINT32_C(0x1100))
/**
 * \brief Identifies the writemultipleproperties operation on Things to update
 * the data of selected writable Properties in a single interaction.
 */
#define WOT_OPERATION_TYPE_WRITE_MULTIPLE_PROPERTIES (UINT32_C(0x1200))
/**
 * \brief Special operation type that is used to indicate an incoming HTTP
 * OPTIONS request.
 *
 * In this case, the handler will not be called and relevant metadata will be
 * returned directly.
 */
#define TINYWOT_OPERATION_TYPE_OPTIONS (UINT32_C(0x8000))

/** @} */

/**
 * \brief Content types / formats that TinyWoT expects.
 *
 * These values, except TINYWOT_CONTENT_TYPE_UNKNOWN, are picked from the
 * [CoAP Content-Formats Registry](https://datatracker.ietf.org/doc/html/rfc7252
 * #section-12.3). IoT devices more or less want to use one of them. The
 * additional one, TINYWOT_CONTENT_TYPE_TD_JSON, is proposed in WoT Thing
 * Description [here](https://www.w3.org/TR/wot-thing-description11/
 * #content-format-section) and have registered in the [IANA registry](https://
 * www.iana.org/assignments/core-parameters/core-parameters.xhtml#content-
 * formats).
 */
typedef enum {
  TINYWOT_CONTENT_TYPE_UNKNOWN = -1,
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
   * \sa WoTOperationType
   */
  uint32_t op;
  /**
   * \brief The path to the resource that the request is querying.
   *
   * Unlike the [`Form`](https://www.w3.org/TR/wot-thing-description11/#form)
   * type in WoT Thing Description, where a `href` of type `anyURI` is used,
   * here only the path component is used. The other components (e.g. scheme,
   * authority) should be handled by the protocol binding implementations.
   */
  char *path;
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
   */
  void *content;
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
   * This is used to indicate an uninitialized TinyWoTResponse.
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
   * This can be compared to a `HTTP 404 Not Found`. This will also be returned
   * by TinyWoT when no handler can be found.
   */
  TINYWOT_RESPONSE_STATUS_UNSUPPORTED,
  /**
   * \brief The handler cannot accept the incoming method.
   *
   * This can be compared to a `HTTP 405 Method Not Allowed`. Note that handler
   * implementors do not usually return this; instead, since TinyWoTHandler::ops
   * have already declared the acceptable interaction affordances of the
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
   * \brief The allowed affordances on the handler.
   *
   * This is a copy of `TinyWoTHandler::ops`. Useful for some responses that
   * need this information, e.g. HTTP 405 Method Not Allowed.
   */
  int allow;
  /**
   * \brief The type of #content.
   */
  TinyWoTContentType content_type;
  /**
   * \brief The length of #content.
   */
  size_t content_length;
  /**
   * \brief The content (payload).
   */
  void *content;
} TinyWoTResponse;

/**
 * \brief A tuple identifying a handler to an incoming request.
 *
 * This is used in TinyWoTThing to describe what kinds of request it can accept.
 * An incoming TinyWoTRequest will be matched on #path and #ops. If #path
 * matches and #ops has the incoming WoTOperationType bit set, then the
 * corresponding #func is invoked with the request and the #ctx provided.
 */
typedef struct {
  /**
   * \brief The path to a resource that #func can handle.
   */
  const char *path;
  /**
   * \brief The operation types that #func can accept.
   *
   * Except "all" and "multiple" types, WoTOperationType value can be
   * `OR`-ed (`|`) together. For example, to indicate that #func can accept
   * property read and property write operations:
   *
   * ```
   * .ops = WOT_OPERATION_TYPE_READ_PROPERTY |
   * WOT_OPERATION_TYPE_WRITE_PROPERTY
   * ```
   *
   * \sa WoTOperationType
   */
  uint32_t ops;
  /**
   * \brief The function to handle #ops at #path.
   *
   * \param[in] request The incoming request.
   * \param[inout] ctx The user data passed in TinyWoTHandler::ctx.
   * \return A response to send back.
   */
  TinyWoTResponse (*func)(TinyWoTRequest *request, void *ctx);
  /**
   * \brief Arbitrary user data (context) to pass to the handler when invoked.
   */
  void *ctx;
} TinyWoTHandler;

/**
 * \brief A representation of a (Web) Thing.
 */
typedef struct {
  /**
   * \brief A list of handlers implementing the behaviors of the Thing.
   *
   * [WoT Thing Description](https://www.w3.org/TR/wot-thing-description11/)
   * is a descriptor of this Thing.
   *
   * Note that at present TinyWoT does not automtically implement the
   * [Well-Known URI](https://www.w3.org/TR/2021/WD-wot-discovery-20210602/
   * #introduction-well-known), so to comply with the standard, an implementor
   * will have to manually implement this handler.
   */
  TinyWoTHandler *handlers;
  /**
   * \brief The size of #handlers, in the number of handlers.
   */
  size_t handlers_size;
} TinyWoTThing;

/**
 * \brief Process a `request` to `thing` using `config`.
 *
 * \param[in] thing   The Thing to expose.
 * \param[in] request The request received from protocol binding libraries.
 * \return A response returned by either:
 * - the handler, when a handler can be found.
 * - this method, when no handler can be found.
 */
TinyWoTResponse tinywot_process(const TinyWoTThing *const thing,
                                const TinyWoTRequest *const request);

#ifdef __cplusplus
}
#endif
