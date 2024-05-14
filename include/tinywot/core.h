/*
  SPDX-FileCopyrightText: 2021-2023 Junde Yhi <junde@yhi.moe>
  SPDX-License-Identifier: MIT
*/

/*!
  \file
  \brief TinyWoT Core C API definition.

  The Core part of TinyWoT is only a small portion of data structure
  definitions and helper functions. It is designed to be the platform-
  agnostic basis of the two other components:

  - Protocol Binding
  - Servient

  They must also be provided to complete a runtime platform. Because
  they are usually platform-spesific, the TinyWoT Core is in place to
  enable any party to implement them for not-yet-supported platforms.

  Other components of TinyWoT can be found under the
  [TinyWoT GitHub organization][org].

  [org]: https://github.com/tinywot
*/

#ifndef TINYWOT_CORE_H
#define TINYWOT_CORE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*!
  \brief Status codes.

  These are return values used and recognized across the library.
*/
enum tinywot_status {
  /*!
    \brief There is insufficient memory to complete an action.
  */
  TINYWOT_STATUS_ERROR_NOT_ENOUGH_MEMORY    = -5,

  /*!
    \brief The function is not implemented.
  */
  TINYWOT_STATUS_ERROR_NOT_IMPLEMENTED      = -4,

  /*!
    \brief Something is missing.

    This can happen when a requested target and operation type cannot
    match any handler registered in a Thing.
  */
  TINYWOT_STATUS_ERROR_NOT_FOUND            = -3,

  /*!
    \brief The requested operation is not allowed.

    This can happen when a target can be matched, but does not accept
    the operation type requested.
  */
  TINYWOT_STATUS_ERROR_NOT_ALLOWED          = -2,

  /*!
    \brief A generic error.

    This can be used when no other error codes can express the error.
    Applications and libraries should try to use other error codes when
    possible.
  */
  TINYWOT_STATUS_ERROR_GENERIC              = -1,

  /*!
    \brief An unknown status.
  */
  TINYWOT_STATUS_UNKNOWN                    =  0,

  /*!
    \brief The operation has been successful and completed.
  */
  TINYWOT_STATUS_SUCCESS                    =  1,

  /*!
    \brief The operation has been successful, but not all work has been
    done; it hasn't been fully completed yet.

    Depending on the context, the caller is given the chance to perform
    some action on the already returned data, then call the same
    function again, with the right arguments to carry on, until
    `TINYWOT_STATUS_SUCCESS` is received, when there is no more data to
    process.
  */
  TINYWOT_STATUS_PARTIAL                    =  2,
};

/*!
  \brief Operation types.

  Web of Things well-known operation types are defined in [Table 27],
  WoT Thing Description specification. This is an abstracted way to
  express request intentions.

  In addition, TinyWoT defines an extra `UNKNOWN` type, which has the
  raw value of `0`. This is used to identify an invalid operation type,
  usually assigned during variable initialization. It should be regarded
  as an invalid operation type and should cause an error.

  [Table 27]: https://www.w3.org/TR/wot-thing-description11/#table-well-known-operation-types
*/
enum tinywot_op {                                    /* 0bqf'ty'yxwr */
  /*!
    \brief A default, invalid operation type reserved for variable
    initialization.
  */
  TINYWOT_OP_UNKNOWN                        = 0x00u, /* 0b00'00'0000 */

  /*!
    \brief Identifies the read operation on Property Affordances to
    retrieve the corresponding data.
  */
  TINYWOT_OP_READPROPERTY                   = 0x51u, /* 0b01'01'0001 */

  /*!
    \brief Identifies the write operation on Property Affordances to
    update the corresponding data.
  */
  TINYWOT_OP_WRITEPROPERTY                  = 0x52u, /* 0b01'01'0010 */

  /*!
    \brief Identifies the observe operation on Property Affordances to
    be notified with the new data when the Property is updated.
  */
  TINYWOT_OP_OBSERVEPROPERTY                = 0x54u, /* 0b01'01'0100 */

  /*!
    \brief Identifies the unobserve operation on Property Affordances to
    stop the corresponding notifications.
  */
  TINYWOT_OP_UNOBSERVEPROPERTY              = 0x58u, /* 0b01'01'1000 */

  /*!
    \brief Identifies the invoke operation on Action Affordances to
    perform the corresponding action.
  */
  TINYWOT_OP_INVOKEACTION                   = 0x64u, /* 0b01'10'0100 */

  /*!
    \brief Identifies the querying operation on Action Affordances to
    get the status of the corresponding action.
  */
  TINYWOT_OP_QUERYACTION                    = 0x61u, /* 0b01'10'0001 */

  /*!
    \brief Identifies the cancel operation on Action Affordances to
    cancel the ongoing corresponding action.
  */
  TINYWOT_OP_CANCELACTION                   = 0x68u, /* 0b01'10'1000 */

  /*!
    \brief Identifies the subscribe operation on Event Affordances to be
    notified by the Thing when the event occurs.
  */
  TINYWOT_OP_SUBSCRIBEEVENT                 = 0x74u, /* 0b01'11'0100 */

  /*!
    \brief Identifies the unsubscribe operation on Event Affordances to
    stop the corresponding notifications.
  */
  TINYWOT_OP_UNSUBSCRIBEEVENT               = 0x78u, /* 0b01'11'1000 */

  /*!
    \brief Identifies the readallproperties operation on a Thing to
    retrieve the data of all Properties in a single interaction.
  */
  TINYWOT_OP_READALLPROPERTIES              = 0x91u, /* 0b10'01'0001 */

  /*!
    \brief Identifies the writeallproperties operation on a Thing to
    update the data of all writable Properties in a single interaction.
  */
  TINYWOT_OP_WRITEALLPROPERTIES             = 0x92u, /* 0b10'01'0010 */

  /*!
    \brief Identifies the readmultipleproperties operation on a Thing to
    retrieve the data of selected Properties in a single interaction.
  */
  TINYWOT_OP_READMULTIPLEPROPERTIES         = 0xd1u, /* 0b11'01'0001 */

  /*!
    \brief Identifies the writemultipleproperties operation on a Thing
    to update the data of selected writable Properties in a single
    interaction.
  */
  TINYWOT_OP_WRITEMULTIPLEPROPERTIES        = 0xd2u, /* 0b11'01'0010 */

  /*!
    \brief Identifies the observeallproperties operation on Properties
    to be notified with new data when any Property is updated.
  */
  TINYWOT_OP_OBSERVEALLPROPERTIES           = 0xd4u, /* 0b11'01'0100 */

  /*!
    \brief Identifies the unobserveallproperties operation on Properties
    to stop notifications from all Properties in a single interaction.
  */
  TINYWOT_OP_UNOBSERVEALLPROPERTIES         = 0xd8u, /* 0b11'01'1000 */

  /*!
    \brief Identifies the queryallactions operation on a Thing to get
    the status of all Actions in a single interaction.
  */
  TINYWOT_OP_QUERYALLACTIONS                = 0xe1u, /* 0b11'10'0001 */

  /*!
    \brief Identifies the subscribeallevents operation on Events to
    subscribe to notifications from all Events in a single interaction.
  */
  TINYWOT_OP_SUBSCRIBEALLEVENTS             = 0xf4u, /* 0b11'11'0100 */

  /*!
    \brief Identifies the unsubscribeallevents operation on Events to
    unsubscribe from notifications from all Events in a single
    interaction.
  */
  TINYWOT_OP_UNSUBSCRIBEALLEVENTS           = 0xf8u, /* 0b11'11'1000 */
};

/*!
  \brief A chunk of data and the metadata about it.
*/
struct tinywot_payload {
  /*!
    \brief A pointer to the buffer holding the actual data.
  */
  unsigned char *content;

  /*!
    \brief The size of buffer pointed by `content` in byte.
  */
  size_t content_buffer_size_byte;

  /*!
    \brief The valid data length of `content` in byte.
  */
  size_t content_length_byte;

  /*!
    \brief An indicator of the type of `content`.

    This field is at least 16 bits long. While the semantics of this
    field remains undefined, it is designed to fit a
    [CoAP Content-Format][coap-cf] ID, which is an unsigned 16-bit
    number. It is recommended to use the registry for this field.

    [coap-cf]: https://www.iana.org/assignments/core-parameters/core-parameters.xhtml#content-formats
  */
  uint_fast16_t content_type;
};

/*!
  \brief The size of buffer reserved for `tinywot_request::target`.
*/
#ifndef TINYWOT_REQUEST_TARGET_BUFFER_SIZE_BYTE
#define TINYWOT_REQUEST_TARGET_BUFFER_SIZE_BYTE (64U)
#endif

/*!
  \brief An incoming request.
  \extends tinywot_payload

  Usually generated by a protocol binding, a request is a
  `tinywot_payload` with an associated target and an operation type. The
  two criteria is then used to find a handler in a `tinywot_thing` to
  handle this request.
*/
struct tinywot_request {
  /*!
    \brief The content carried with the request.
  */
  struct tinywot_payload payload;

  /*!
    \brief The name of affordance.
  */
  char const *name;

  /*!
    \brief The intended operation type extracted from the request.
  */
  enum tinywot_op op;
};

/*!
  \brief An outgoing response.
  \extends tinywot_payload

  A response is a `tinywot_payload` with a status code. This is usually
  generated by a handler in `tinywot_thing` and is accepted by a
  protocol binding library to serialize a response message.
*/
struct tinywot_response {
  /*!
    \brief The content to send with the response.
  */
  struct tinywot_payload payload;

  /*!
    \brief A status code for the response.
  */
  enum tinywot_status status;
};

/*!
  \brief The signature of a handler function implementing a form (the
  behavior of a submission target).

  `response_payload` and `request_payload` are the `payload` fields of
  `tinywot_response` and `tinywot_request` respectively. The handler can
  assume the two to be always non-NULL. Handlers should read contents
  from the request payload (if applicable) and write contents to the
  response payload.

  \param[out] response_payload A pointer to a new `tinywot_payload` in a
  `tinywot_response`.
  \param[in] request_payload A pointer to a `tinywot_payload` in a
  `tinywot_response`.
  \param[inout] context  The context information set in the
  corresponding `tinywot_form`.
  \return `tinywot_status`
*/
typedef enum tinywot_status tinywot_form_handler_t(
  struct tinywot_payload *response_payload,
  struct tinywot_payload *request_payload,
  void *context
);

/*!
  \brief An operation endpoint.
*/
struct tinywot_form {
  /*!
    \brief The name of affordance that this form is attached to.

    For a top-level form, this field should be set to `NULL`.

    This field is so far informative only -- no procedure relies on its
    value. In the future, however, this field may be useful.
  */
  char const *name;

  /*!
    \brief The submission target of the form.

    A `tinywot_thing` will compare this field with the `target` field in
    a `tinywot_request` to figure out a registered form.

    The exact way to specify a submission target is undefined. This
    largely depends on the protocol binding implementation, who is
    responsible for extracting this information from the request
    message. Often, for an IRI specified in a Thing Description
    document, this should be the path name of it. For example, for the
    following IRI:

        http://thing.example.com/action/update

    Its path name is `/action/update`, so this field should be
    `/action/update`.
  */
  char const *target;

  /*!
    \brief The allowed operation type on this form.
  */
  enum tinywot_op op;

  /*!
    \brief A function pointer to the actual implementation of the form.

    This can be `NULL`, in which case this form is identified as not
    implemented.
  */
  tinywot_form_handler_t *handler;

  /*!
    \brief Arbitrary data to pass to `handler` when it is called.
  */
  void *context;
};

/*!
  \brief A Web Thing.

  A Thing is currently just a collection of `tinywot_form`s.
*/
struct tinywot_thing {
  /*!
    \brief A list of forms describing the behavior of this Thing.
  */
  struct tinywot_form *forms;

  /*!
    \brief The number of registered forms in `forms`.
  */
  size_t forms_count_n;

  /*!
    \brief The maximum number of `tinywot_form`s that the supporting
    memory can contain.

    This field mainly supports the dynamic changing of `tinywot_thing`.
    If `forms` points to a read-only memory, then this field should
    either be equal to `forms_count_n` or `0` to prevent any method from
    rewriting a non-modifyable memory.
  */
  size_t forms_max_n;
};

/*!
  \brief Initialize a `tinywot_thing` with a static list of forms.
  \memberof tinywot_thing

  If you have a list of `tinywot_form`s declared in a `static const`
  array, then this helps correctly setting up a `tinywot_thing`. The
  `tinywot_thing` initialized in this way should prevent any later
  modification from method calls, e.g. `tinywot_thing_change_form()`.

  Note that:

  - `forms` is only shallow copied, so it must point to `'static`
    memory. Otherwise, a dangling pointer would be used.
  - `forms_size_byte` is the size of `forms` in byte, not the number of
    `tinywot_forms` in the array. Most often, this should just be
    `sizeof(forms)` (when `forms` is still `struct tinywot_form []`).

  \param[inout] self An instance of `tinywot_thing`.
  \param[in] forms An array of `tinywot_form` in ROM.
  \param[in] forms_size_byte The size of `forms` in byte.
*/
void tinywot_thing_init_static(
  struct tinywot_thing *self,
  struct tinywot_form const *forms,
  size_t forms_size_byte
);

/*!
  \brief Initialize a `tinywot_thing` with Randomly Accessible Memory
  (RAM).
  \memberof tinywot_thing

  If you want to add, change and remove `tinywot_form`s interactively,
  then this helps correctly setting up a `tinywot_thing`. The
  `tinywot_thing` initialized in this way allows all methods that change
  the list of forms of a `tinywot_thing`.

  `memory` is seen as blank memory with `0` forms.

  \param[inout] self An instance of `tinywot_thing`.
  \param[in] memory A pointer to any segment of RAM.
  \param[in] memory_size_byte The size of `memory` in byte.
*/
void tinywot_thing_init_dynamic(
  struct tinywot_thing *self, void *memory, size_t memory_size_byte
);

/*!
  \brief Initialize a `tinywot_thing` with Randomly Accessible Memory
  (RAM), then copy a static list of forms into the RAM.
  \memberof tinywot_thing

  If you want to initialize a dynamic (changable) `tinywot_thing` out of
  a pre-defined list of `tinywot_form`s, then this helps correctly
  setting up a `tinywot_thing`.

  Memory pointed by `forms` will be copied to memory pointed by
  `memory`. Note that in this way `tinywot_form::name` and
  `tinywot_form::target` are only shallow copied, but this is by design
  as they are required to be `const char`. `forms` does not have to
  point to ROM either, as it is copied to `memory`.

  \param[inout] self An instance of `tinywot_thing`.
  \param[in] memory A pointer to any segment of RAM.
  \param[in] memory_size_byte The size of `memory` in byte.
  \param[in] forms An array of `tinywot_form` in ROM or RAM.
  \param[in] forms_size_byte The size of `forms` in byte.
  \return
    - `::TINYWOT_STATUS_ERROR_NOT_ENOUGH_MEMORY` if `memory_size_byte`
      is smaller than `forms_size_byte`.
    - `::TINYWOT_STATUS_SUCCESS` if the initialization succeeds.
*/
enum tinywot_status tinywot_thing_init_dynamic_from_static(
  struct tinywot_thing *self,
  void *memory,
  size_t memory_size_byte,
  struct tinywot_form const *forms,
  size_t forms_size_byte
);

/*!
  \brief Find a `tinywot_form` registered in the `tinywot_thing`.
  \memberof tinywot_thing

  This function finds a matching `tinywot_form` according to `target`
  and `op` from the end of the list of registered `tinywot_form` in the
  `tinywot_thing`. The first matching `tinywot_form` is retured to the
  caller via the `form` parameter.

  \param[in] self An instance of `tinywot_thing`.
  \param[out] form A copy of pointer to the matching `form`.
  \param[in] target `tinywot_form::target`.
  \param[in] op `tinywot_form::op`.
  \return
    - `::TINYWOT_STATUS_ERROR_NOT_FOUND` if no matching `tinywot_form`
      can be found in the `tinywot_thing`.
    - `::TINYWOT_STATUS_ERROR_NOT_ALLOWED` if there is at least one
      `tinywot_form::target` matches, but no `tinywot_form::op` matches.
      This usually means that the `tinywot_form::target` cannot accept
      the requested interaction.
    - `::TINYWOT_STATUS_SUCCESS` if a matching `tinywot_form` has been
      returned via the `form` parameter.
*/
enum tinywot_status tinywot_thing_find_form(
  struct tinywot_thing const *self,
  struct tinywot_form **form,
  char const *target,
  enum tinywot_op op
);

/*!
  \brief Add (register) a `tinywot_form` to the `tinywot_thing`.
  \memberof tinywot_thing

  This function merely appends `form` to the end of list of
  `tinywot_form` in the supplied `tinywot_thing`. Because
  `tinywot_thing_find_form()` looks up the list from the end, adding a
  `tinywot_form` with the same `tinywot_form::target` and
  `tinywot_form::op` effectively overrides a formerly added (registered)
  `tinywot_form`. Removing it (using `tinywot_thing_remove_form()`) can
  re-enable the formerly added (registered) `tinywot_form`.

  To change an already added (registered) `tinywot_form`, use
  `tinywot_thing_change_form()`.

  \param[inout] self An instance of `tinywot_thing`.
  \param[in] form A pointer to a `tinywot_form` to be added (registered)
  to the supplied `tinywot_thing`.
  \return
    - `::TINYWOT_STATUS_ERROR_NOT_ENOUGH_MEMORY` if there is not enough
      space in the backing memory of the `tinywot_thing` to insert a new
      `tinywot_form`.
    - `::TINYWOT_STATUS_SUCCESS` if the operation has been successful.
*/
enum tinywot_status tinywot_thing_add_form(
  struct tinywot_thing *self, struct tinywot_form const *form
);

/*!
  \brief Modify a `tinywot_form` registered in the `tinywot_thing`.
  \memberof tinywot_thing

  This function first finds a matching form in the supplied
  `tinywot_thing`, then overwrites it with the `tinywot_form` provided
  via the `form` parameter.

  \param[inout] self An instance of `tinywot_thing`.
  \param[in] target `tinywot_form::target`.
  \param[in] op `tinywot_form::op`.
  \param[in] form The new `tinywot_form` replacing the matching one.
  \return
    - `::TINYWOT_STATUS_ERROR_NOT_FOUND` if the specified `target` and
      `op` cannot match a registered `tinywot_form`.
    - `::TINYWOT_STATUS_SUCCESS` if the operation has been successful.
*/
enum tinywot_status tinywot_thing_change_form(
  struct tinywot_thing *self,
  char const *target,
  enum tinywot_op op,
  struct tinywot_form const *form
);

/*!
  \brief Delete a `tinywot_form` registered in the `tinywot_thing`.
  \memberof tinywot_thing

  \param[inout] self An instance of `tinywot_thing`.
  \param[in] target `tinywot_form::target`.
  \param[in] op `tinywot_form::op`.
  \return
    - `::TINYWOT_STATUS_ERROR_NOT_FOUND` if the specified `target` and
      `op` cannot match a registered `tinywot_form`.
    - `::TINYWOT_STATUS_SUCCESS` if the operation has been successful.
*/
enum tinywot_status tinywot_thing_remove_form(
  struct tinywot_thing *self,
  char const *target,
  enum tinywot_op op
);

/*!
  \brief Transforming a `tinywot_request` to a `tinywot_response` with
  the `tinywot_thing`.
  \memberof tinywot_thing

  \param[in] self An instance of `tinywot_thing`.
  \param[out] response An outgoing `tinywot_response`.
  \param[in] request An incoming `tinywot_request`.
  \return
    - `::TINYWOT_STATUS_ERROR_NOT_IMPLEMENTED` if a matching
      `tinywot_form` is found, but it has no registered handler
      function (`tinywot_form::handler` is `NULL`).
    - `::TINYWOT_STATUS_ERROR_NOT_FOUND` if the requested
      `tinywot_request::target` and `tinywot_request::op` cannot match a
      registered `tinywot_form`. An empty response with this status is
      prepared in the supplied `tinywot_response` in this case.
    - `::TINYWOT_STATUS_ERROR_NOT_ALLOWED` if at least one
      `tinywot_form` can be found with `tinywot_request::target`, but
      none matches `tinywot_request::op`.
    - `::TINYWOT_STATUS_SUCCESS` if the request has been successfully
      processed with the `tinywot_thing`.
    - Other codes are also possible if the registered handler function
      returns them.
*/
enum tinywot_status tinywot_thing_process_request(
  struct tinywot_thing const *self,
  struct tinywot_response *response,
  struct tinywot_request *request
);

/*!
  \brief Standard general type for TinyWoT input / output modules.
*/
struct tinywot_io {
  /*!
    \brief A pointer to an arbitrary data structure.
  */
  void *context;

  /*!
    \brief Function that reads data into a memory buffer.
  */
  enum tinywot_status (*read)(
    void *context,
    unsigned char *buffer,
    size_t buffer_size_byte,
    size_t *read_length_byte
  );

  /*!
    \brief Function that writes data from a memory buffer.
  */
  enum tinywot_status (*write)(
    void *context,
    unsigned char *buffer,
    size_t write_length_byte,
    size_t *written_length_byte
  );
};

/*!
  \brief Standard general type for TinyWoT protocol implementation
  modules.
*/
struct tinywot_protocol {
  /*!
    \brief A pointer to an arbitrary data structure.
  */
  void *context;

  /*!
    \brief Function that converts the data in a memory buffer to a
    `tinywot_request`.
  */
  enum tinywot_status (*receive)(
    void *context,
    struct tinywot_request *request,
    unsigned char *buffer,
    size_t receive_length_byte
  );

  /*!
    \brief Function that converts a `tinywot_response` to a series of
    bytes in a memory buffer.
  */
  enum tinywot_status (*send)(
    void *context,
    struct tinywot_response *response,
    unsigned char *buffer,
    size_t buffer_size_byte,
    size_t *sent_length_byte
  );
};

char const *tinywot_status_to_string(enum tinywot_status self);
enum tinywot_status tinywot_payload_append(
  struct tinywot_payload *self,
  void const *data,
  size_t data_size_byte
);
enum tinywot_status tinywot_payload_append_string(
  struct tinywot_payload *self,
  char const *str
);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* TINYWOT_CORE_H */
