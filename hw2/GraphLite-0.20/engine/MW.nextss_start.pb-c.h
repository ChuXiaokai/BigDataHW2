/* Generated by the protocol buffer compiler.  DO NOT EDIT! */

#ifndef PROTOBUF_C_MW_2enextss_5fstart_2eproto__INCLUDED
#define PROTOBUF_C_MW_2enextss_5fstart_2eproto__INCLUDED

#include <google/protobuf-c/protobuf-c.h>

PROTOBUF_C_BEGIN_DECLS


typedef struct _Mw__NextssStart Mw__NextssStart;


/* --- enums --- */


/* --- messages --- */

struct  _Mw__NextssStart
{
  ProtobufCMessage base;
  int32_t s_id;
  int32_t d_id;
  int32_t superstep;
  int64_t node_msg;
  size_t n_aggr_global;
  ProtobufCBinaryData *aggr_global;
};
#define MW__NEXTSS_START__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&mw__nextss_start__descriptor) \
    , 0, 0, 0, 0, 0,NULL }


/* Mw__NextssStart methods */
void   mw__nextss_start__init
                     (Mw__NextssStart         *message);
size_t mw__nextss_start__get_packed_size
                     (const Mw__NextssStart   *message);
size_t mw__nextss_start__pack
                     (const Mw__NextssStart   *message,
                      uint8_t             *out);
size_t mw__nextss_start__pack_to_buffer
                     (const Mw__NextssStart   *message,
                      ProtobufCBuffer     *buffer);
Mw__NextssStart *
       mw__nextss_start__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   mw__nextss_start__free_unpacked
                     (Mw__NextssStart *message,
                      ProtobufCAllocator *allocator);
/* --- per-message closures --- */

typedef void (*Mw__NextssStart_Closure)
                 (const Mw__NextssStart *message,
                  void *closure_data);

/* --- services --- */


/* --- descriptors --- */

extern const ProtobufCMessageDescriptor mw__nextss_start__descriptor;

PROTOBUF_C_END_DECLS


#endif  /* PROTOBUF_MW_2enextss_5fstart_2eproto__INCLUDED */
