#ifndef WT_TYPE_H
#define WT_TYPE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

enum wt_status
{
  WT_STATUS_ERROR_NOT_ENOUGH_MEMORY    = -5,
  WT_STATUS_ERROR_NOT_IMPLEMENTED      = -4,
  WT_STATUS_ERROR_NOT_FOUND            = -3,
  WT_STATUS_ERROR_NOT_ALLOWED          = -2,
  WT_STATUS_ERROR_GENERIC              = -1,
  WT_STATUS_UNKNOWN                    =  0,
  WT_STATUS_SUCCESS                    =  1,
  WT_STATUS_PARTIAL                    =  2,
};

enum wt_op
{                                               /* 0bnn'vv'yxwr */
  WT_OP_UNKNOWN                        = 0x00u, /* 0b00'00'0000 */

  WT_OP_READPROPERTY                   = 0x51u, /* 0b01'01'0001 */
  WT_OP_WRITEPROPERTY                  = 0x52u, /* 0b01'01'0010 */
  WT_OP_OBSERVEPROPERTY                = 0x54u, /* 0b01'01'0100 */
  WT_OP_UNOBSERVEPROPERTY              = 0x58u, /* 0b01'01'1000 */

  WT_OP_INVOKEACTION                   = 0x64u, /* 0b01'10'0100 */
  WT_OP_QUERYACTION                    = 0x61u, /* 0b01'10'0001 */
  WT_OP_CANCELACTION                   = 0x68u, /* 0b01'10'1000 */

  WT_OP_SUBSCRIBEEVENT                 = 0x74u, /* 0b01'11'0100 */
  WT_OP_UNSUBSCRIBEEVENT               = 0x78u, /* 0b01'11'1000 */

  WT_OP_READMULTIPLEPROPERTIES         = 0x91u, /* 0b10'01'0001 */
  WT_OP_WRITEMULTIPLEPROPERTIES        = 0x92u, /* 0b10'01'0010 */

  WT_OP_READALLPROPERTIES              = 0xd1u, /* 0b11'01'0001 */
  WT_OP_WRITEALLPROPERTIES             = 0xd2u, /* 0b11'01'0010 */
  WT_OP_OBSERVEALLPROPERTIES           = 0xd4u, /* 0b11'01'0100 */
  WT_OP_UNOBSERVEALLPROPERTIES         = 0xd8u, /* 0b11'01'1000 */

  WT_OP_QUERYALLACTIONS                = 0xe1u, /* 0b11'10'0001 */

  WT_OP_SUBSCRIBEALLEVENTS             = 0xf4u, /* 0b11'11'0100 */
  WT_OP_UNSUBSCRIBEALLEVENTS           = 0xf8u, /* 0b11'11'1000 */
};

struct wt_payload
{
  unsigned char *content;
  size_t buffer_size;
  size_t content_length;
  uint_fast16_t content_type;
};

struct wt_request
{
  struct wt_payload payload;
  char const *name;
  uint_fast16_t op;
};

struct wt_response
{
  struct wt_payload payload;
  uint_fast8_t status;
};

struct wt_form
{
  char const *name;
  char const *target;
  uint_fast16_t ops_allowed;
  int (*handler)(struct wt_payload * restrict payload_response,
                 struct wt_payload const * restrict payload_request,
                 intptr_t *context);
  intptr_t context;
};

enum wt_protocol_mode
{
  WT_PROTOCOL_READ_REQUEST_WRITE_RESPONSE = 1,
  WT_PROTOCOL_WRITE_REQUEST_READ_RESPONSE = 2,
};

struct wt_interface_protocol
{
  int (*set_mode)(void *self, enum wt_protocol_mode mode);

  int (*read)(void *self,
              unsigned char const *buffer,
              size_t data_size,
              size_t *read_size);
  int (*write)(void *self,
               unsigned char *buffer,
               size_t buffer_size,
               size_t *written_size);

  int (*get_target)(void *self, char *buffer, size_t buffer_size);
  int (*get_op)(void *self, uint_fast16_t *op, uint_fast16_t ops_allowed);
  int (*get_payload)(void *self, struct wt_payload *payload);

  int (*set_target)(void *self, char *buffer);
  int (*set_op)(void *self, uint_fast16_t op);
  int (*set_payload)(void *self, struct wt_payload const *payload);

  int (*get_response)(void *self, struct wt_response *response);
  int (*set_response)(void *self, struct wt_response const *response);
};

struct wt_interface_form_store
{
  int (*set_forms)(void *self,
                   struct wt_form const *forms,
                   size_t buffer_size,
                   size_t forms_number);

  int (*add_form)(void *self, struct wt_form const *form);
  int (*change_form)(void *self,
                     struct wt_form const * restrict form_input,
                     struct wt_form const * restrict form_criteria,
                     enum wt_form_select select);
  int (*remove_form)(void *self,
                     struct wt_form const *form_criteria,
                     enum wt_form_select select);

  int (*find_form_by_name)(void *self,
                           struct wt_form *form_result,
                           char const *name);
  int (*find_form_by_target)(void *self,
                             struct wt_form *form_result,
                             char const *target);
};

struct wt_interface_servient
{
  int (*set_protocol)(void *self,
                      struct wt_interface_protocol const *protocol);
  int (*set_form_store)(void *self,
                        struct wt_interface_form_store const *form_store);

  int (*run_once)(void *self);
};

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* WT_TYPE_H */
