/* Generated by the protocol buffer compiler.  DO NOT EDIT! */

#ifndef PROTOBUF_C_WW_2enodemsg_5flist_2eproto__INCLUDED
#define PROTOBUF_C_WW_2enodemsg_5flist_2eproto__INCLUDED

#include <google/protobuf-c/protobuf-c.h>

PROTOBUF_C_BEGIN_DECLS


typedef struct _Ww__NodemsgList Ww__NodemsgList;


/* --- enums --- */


/* --- messages --- */

struct  _Ww__NodemsgList
{
  ProtobufCMessage base;
  int32_t s_id;
  int32_t d_id;
  int32_t superstep;
  int32_t num_msgs;
  int32_t msg_size;
  ProtobufCBinaryData msgs;
};
#define WW__NODEMSG_LIST__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&ww__nodemsg_list__descriptor) \
    , 0, 0, 0, 0, 0, {0,NULL} }


/* Ww__NodemsgList methods */
void   ww__nodemsg_list__init
                     (Ww__NodemsgList         *message);
size_t ww__nodemsg_list__get_packed_size
                     (const Ww__NodemsgList   *message);
size_t ww__nodemsg_list__pack
                     (const Ww__NodemsgList   *message,
                      uint8_t             *out);
size_t ww__nodemsg_list__pack_to_buffer
                     (const Ww__NodemsgList   *message,
                      ProtobufCBuffer     *buffer);
Ww__NodemsgList *
       ww__nodemsg_list__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   ww__nodemsg_list__free_unpacked
                     (Ww__NodemsgList *message,
                      ProtobufCAllocator *allocator);
/* --- per-message closures --- */

typedef void (*Ww__NodemsgList_Closure)
                 (const Ww__NodemsgList *message,
                  void *closure_data);

/* --- services --- */


/* --- descriptors --- */

extern const ProtobufCMessageDescriptor ww__nodemsg_list__descriptor;

PROTOBUF_C_END_DECLS


#endif  /* PROTOBUF_WW_2enodemsg_5flist_2eproto__INCLUDED */