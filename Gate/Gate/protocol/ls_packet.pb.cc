// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: ls_packet.proto

#define INTERNAL_SUPPRESS_PROTOBUF_FIELD_DEPRECATION
#include "ls_packet.pb.h"

#include <algorithm>

#include <google/protobuf/stubs/common.h>
#include <google/protobuf/stubs/port.h>
#include <google/protobuf/stubs/once.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/wire_format_lite_inl.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/reflection_ops.h>
#include <google/protobuf/wire_format.h>
// @@protoc_insertion_point(includes)

namespace ls_packet {

namespace {

const ::google::protobuf::Descriptor* Ping_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  Ping_reflection_ = NULL;
const ::google::protobuf::Descriptor* ServerAddress_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  ServerAddress_reflection_ = NULL;
const ::google::protobuf::Descriptor* GameStart_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  GameStart_reflection_ = NULL;

}  // namespace


void protobuf_AssignDesc_ls_5fpacket_2eproto() GOOGLE_ATTRIBUTE_COLD;
void protobuf_AssignDesc_ls_5fpacket_2eproto() {
  protobuf_AddDesc_ls_5fpacket_2eproto();
  const ::google::protobuf::FileDescriptor* file =
    ::google::protobuf::DescriptorPool::generated_pool()->FindFileByName(
      "ls_packet.proto");
  GOOGLE_CHECK(file != NULL);
  Ping_descriptor_ = file->message_type(0);
  static const int Ping_offsets_[1] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Ping, ping_),
  };
  Ping_reflection_ =
    ::google::protobuf::internal::GeneratedMessageReflection::NewGeneratedMessageReflection(
      Ping_descriptor_,
      Ping::default_instance_,
      Ping_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Ping, _has_bits_[0]),
      -1,
      -1,
      sizeof(Ping),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Ping, _internal_metadata_),
      -1);
  ServerAddress_descriptor_ = file->message_type(1);
  static const int ServerAddress_offsets_[2] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(ServerAddress, host_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(ServerAddress, port_),
  };
  ServerAddress_reflection_ =
    ::google::protobuf::internal::GeneratedMessageReflection::NewGeneratedMessageReflection(
      ServerAddress_descriptor_,
      ServerAddress::default_instance_,
      ServerAddress_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(ServerAddress, _has_bits_[0]),
      -1,
      -1,
      sizeof(ServerAddress),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(ServerAddress, _internal_metadata_),
      -1);
  GameStart_descriptor_ = file->message_type(2);
  static const int GameStart_offsets_[1] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(GameStart, type_),
  };
  GameStart_reflection_ =
    ::google::protobuf::internal::GeneratedMessageReflection::NewGeneratedMessageReflection(
      GameStart_descriptor_,
      GameStart::default_instance_,
      GameStart_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(GameStart, _has_bits_[0]),
      -1,
      -1,
      sizeof(GameStart),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(GameStart, _internal_metadata_),
      -1);
}

namespace {

GOOGLE_PROTOBUF_DECLARE_ONCE(protobuf_AssignDescriptors_once_);
inline void protobuf_AssignDescriptorsOnce() {
  ::google::protobuf::GoogleOnceInit(&protobuf_AssignDescriptors_once_,
                 &protobuf_AssignDesc_ls_5fpacket_2eproto);
}

void protobuf_RegisterTypes(const ::std::string&) GOOGLE_ATTRIBUTE_COLD;
void protobuf_RegisterTypes(const ::std::string&) {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
      Ping_descriptor_, &Ping::default_instance());
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
      ServerAddress_descriptor_, &ServerAddress::default_instance());
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
      GameStart_descriptor_, &GameStart::default_instance());
}

}  // namespace

void protobuf_ShutdownFile_ls_5fpacket_2eproto() {
  delete Ping::default_instance_;
  delete Ping_reflection_;
  delete ServerAddress::default_instance_;
  delete ServerAddress_reflection_;
  delete GameStart::default_instance_;
  delete GameStart_reflection_;
}

void protobuf_AddDesc_ls_5fpacket_2eproto() GOOGLE_ATTRIBUTE_COLD;
void protobuf_AddDesc_ls_5fpacket_2eproto() {
  static bool already_here = false;
  if (already_here) return;
  already_here = true;
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  ::google::protobuf::DescriptorPool::InternalAddGeneratedFile(
    "\n\017ls_packet.proto\022\tls_packet\"\024\n\004Ping\022\014\n\004"
    "ping\030\001 \002(\005\"+\n\rServerAddress\022\014\n\004host\030\001 \002("
    "\014\022\014\n\004port\030\002 \002(\005\"\031\n\tGameStart\022\014\n\004type\030\001 \002"
    "(\005", 122);
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedFile(
    "ls_packet.proto", &protobuf_RegisterTypes);
  Ping::default_instance_ = new Ping();
  ServerAddress::default_instance_ = new ServerAddress();
  GameStart::default_instance_ = new GameStart();
  Ping::default_instance_->InitAsDefaultInstance();
  ServerAddress::default_instance_->InitAsDefaultInstance();
  GameStart::default_instance_->InitAsDefaultInstance();
  ::google::protobuf::internal::OnShutdown(&protobuf_ShutdownFile_ls_5fpacket_2eproto);
}

// Force AddDescriptors() to be called at static initialization time.
struct StaticDescriptorInitializer_ls_5fpacket_2eproto {
  StaticDescriptorInitializer_ls_5fpacket_2eproto() {
    protobuf_AddDesc_ls_5fpacket_2eproto();
  }
} static_descriptor_initializer_ls_5fpacket_2eproto_;

// ===================================================================

#if !defined(_MSC_VER) || _MSC_VER >= 1900
const int Ping::kPingFieldNumber;
#endif  // !defined(_MSC_VER) || _MSC_VER >= 1900

Ping::Ping()
  : ::google::protobuf::Message(), _internal_metadata_(NULL) {
  SharedCtor();
  // @@protoc_insertion_point(constructor:ls_packet.Ping)
}

void Ping::InitAsDefaultInstance() {
}

Ping::Ping(const Ping& from)
  : ::google::protobuf::Message(),
    _internal_metadata_(NULL) {
  SharedCtor();
  MergeFrom(from);
  // @@protoc_insertion_point(copy_constructor:ls_packet.Ping)
}

void Ping::SharedCtor() {
  _cached_size_ = 0;
  ping_ = 0;
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

Ping::~Ping() {
  // @@protoc_insertion_point(destructor:ls_packet.Ping)
  SharedDtor();
}

void Ping::SharedDtor() {
  if (this != default_instance_) {
  }
}

void Ping::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* Ping::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return Ping_descriptor_;
}

const Ping& Ping::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_ls_5fpacket_2eproto();
  return *default_instance_;
}

Ping* Ping::default_instance_ = NULL;

Ping* Ping::New(::google::protobuf::Arena* arena) const {
  Ping* n = new Ping;
  if (arena != NULL) {
    arena->Own(n);
  }
  return n;
}

void Ping::Clear() {
// @@protoc_insertion_point(message_clear_start:ls_packet.Ping)
  ping_ = 0;
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  if (_internal_metadata_.have_unknown_fields()) {
    mutable_unknown_fields()->Clear();
  }
}

bool Ping::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!GOOGLE_PREDICT_TRUE(EXPRESSION)) goto failure
  ::google::protobuf::uint32 tag;
  // @@protoc_insertion_point(parse_start:ls_packet.Ping)
  for (;;) {
    ::std::pair< ::google::protobuf::uint32, bool> p = input->ReadTagWithCutoff(127);
    tag = p.first;
    if (!p.second) goto handle_unusual;
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // required int32 ping = 1;
      case 1: {
        if (tag == 8) {
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::int32, ::google::protobuf::internal::WireFormatLite::TYPE_INT32>(
                 input, &ping_)));
          set_has_ping();
        } else {
          goto handle_unusual;
        }
        if (input->ExpectAtEnd()) goto success;
        break;
      }

      default: {
      handle_unusual:
        if (tag == 0 ||
            ::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_END_GROUP) {
          goto success;
        }
        DO_(::google::protobuf::internal::WireFormat::SkipField(
              input, tag, mutable_unknown_fields()));
        break;
      }
    }
  }
success:
  // @@protoc_insertion_point(parse_success:ls_packet.Ping)
  return true;
failure:
  // @@protoc_insertion_point(parse_failure:ls_packet.Ping)
  return false;
#undef DO_
}

void Ping::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // @@protoc_insertion_point(serialize_start:ls_packet.Ping)
  // required int32 ping = 1;
  if (has_ping()) {
    ::google::protobuf::internal::WireFormatLite::WriteInt32(1, this->ping(), output);
  }

  if (_internal_metadata_.have_unknown_fields()) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        unknown_fields(), output);
  }
  // @@protoc_insertion_point(serialize_end:ls_packet.Ping)
}

::google::protobuf::uint8* Ping::InternalSerializeWithCachedSizesToArray(
    bool deterministic, ::google::protobuf::uint8* target) const {
  // @@protoc_insertion_point(serialize_to_array_start:ls_packet.Ping)
  // required int32 ping = 1;
  if (has_ping()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteInt32ToArray(1, this->ping(), target);
  }

  if (_internal_metadata_.have_unknown_fields()) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        unknown_fields(), target);
  }
  // @@protoc_insertion_point(serialize_to_array_end:ls_packet.Ping)
  return target;
}

int Ping::ByteSize() const {
// @@protoc_insertion_point(message_byte_size_start:ls_packet.Ping)
  int total_size = 0;

  // required int32 ping = 1;
  if (has_ping()) {
    total_size += 1 +
      ::google::protobuf::internal::WireFormatLite::Int32Size(
        this->ping());
  }
  if (_internal_metadata_.have_unknown_fields()) {
    total_size +=
      ::google::protobuf::internal::WireFormat::ComputeUnknownFieldsSize(
        unknown_fields());
  }
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = total_size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
  return total_size;
}

void Ping::MergeFrom(const ::google::protobuf::Message& from) {
// @@protoc_insertion_point(generalized_merge_from_start:ls_packet.Ping)
  if (GOOGLE_PREDICT_FALSE(&from == this)) {
    ::google::protobuf::internal::MergeFromFail(__FILE__, __LINE__);
  }
  const Ping* source = 
      ::google::protobuf::internal::DynamicCastToGenerated<const Ping>(
          &from);
  if (source == NULL) {
  // @@protoc_insertion_point(generalized_merge_from_cast_fail:ls_packet.Ping)
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
  // @@protoc_insertion_point(generalized_merge_from_cast_success:ls_packet.Ping)
    MergeFrom(*source);
  }
}

void Ping::MergeFrom(const Ping& from) {
// @@protoc_insertion_point(class_specific_merge_from_start:ls_packet.Ping)
  if (GOOGLE_PREDICT_FALSE(&from == this)) {
    ::google::protobuf::internal::MergeFromFail(__FILE__, __LINE__);
  }
  if (from._has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (from.has_ping()) {
      set_ping(from.ping());
    }
  }
  if (from._internal_metadata_.have_unknown_fields()) {
    mutable_unknown_fields()->MergeFrom(from.unknown_fields());
  }
}

void Ping::CopyFrom(const ::google::protobuf::Message& from) {
// @@protoc_insertion_point(generalized_copy_from_start:ls_packet.Ping)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void Ping::CopyFrom(const Ping& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:ls_packet.Ping)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool Ping::IsInitialized() const {
  if ((_has_bits_[0] & 0x00000001) != 0x00000001) return false;

  return true;
}

void Ping::Swap(Ping* other) {
  if (other == this) return;
  InternalSwap(other);
}
void Ping::InternalSwap(Ping* other) {
  std::swap(ping_, other->ping_);
  std::swap(_has_bits_[0], other->_has_bits_[0]);
  _internal_metadata_.Swap(&other->_internal_metadata_);
  std::swap(_cached_size_, other->_cached_size_);
}

::google::protobuf::Metadata Ping::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = Ping_descriptor_;
  metadata.reflection = Ping_reflection_;
  return metadata;
}

#if PROTOBUF_INLINE_NOT_IN_HEADERS
// Ping

// required int32 ping = 1;
bool Ping::has_ping() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
void Ping::set_has_ping() {
  _has_bits_[0] |= 0x00000001u;
}
void Ping::clear_has_ping() {
  _has_bits_[0] &= ~0x00000001u;
}
void Ping::clear_ping() {
  ping_ = 0;
  clear_has_ping();
}
 ::google::protobuf::int32 Ping::ping() const {
  // @@protoc_insertion_point(field_get:ls_packet.Ping.ping)
  return ping_;
}
 void Ping::set_ping(::google::protobuf::int32 value) {
  set_has_ping();
  ping_ = value;
  // @@protoc_insertion_point(field_set:ls_packet.Ping.ping)
}

#endif  // PROTOBUF_INLINE_NOT_IN_HEADERS

// ===================================================================

#if !defined(_MSC_VER) || _MSC_VER >= 1900
const int ServerAddress::kHostFieldNumber;
const int ServerAddress::kPortFieldNumber;
#endif  // !defined(_MSC_VER) || _MSC_VER >= 1900

ServerAddress::ServerAddress()
  : ::google::protobuf::Message(), _internal_metadata_(NULL) {
  SharedCtor();
  // @@protoc_insertion_point(constructor:ls_packet.ServerAddress)
}

void ServerAddress::InitAsDefaultInstance() {
}

ServerAddress::ServerAddress(const ServerAddress& from)
  : ::google::protobuf::Message(),
    _internal_metadata_(NULL) {
  SharedCtor();
  MergeFrom(from);
  // @@protoc_insertion_point(copy_constructor:ls_packet.ServerAddress)
}

void ServerAddress::SharedCtor() {
  ::google::protobuf::internal::GetEmptyString();
  _cached_size_ = 0;
  host_.UnsafeSetDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  port_ = 0;
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

ServerAddress::~ServerAddress() {
  // @@protoc_insertion_point(destructor:ls_packet.ServerAddress)
  SharedDtor();
}

void ServerAddress::SharedDtor() {
  host_.DestroyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  if (this != default_instance_) {
  }
}

void ServerAddress::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* ServerAddress::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return ServerAddress_descriptor_;
}

const ServerAddress& ServerAddress::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_ls_5fpacket_2eproto();
  return *default_instance_;
}

ServerAddress* ServerAddress::default_instance_ = NULL;

ServerAddress* ServerAddress::New(::google::protobuf::Arena* arena) const {
  ServerAddress* n = new ServerAddress;
  if (arena != NULL) {
    arena->Own(n);
  }
  return n;
}

void ServerAddress::Clear() {
// @@protoc_insertion_point(message_clear_start:ls_packet.ServerAddress)
  if (_has_bits_[0 / 32] & 3u) {
    if (has_host()) {
      host_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
    }
    port_ = 0;
  }
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  if (_internal_metadata_.have_unknown_fields()) {
    mutable_unknown_fields()->Clear();
  }
}

bool ServerAddress::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!GOOGLE_PREDICT_TRUE(EXPRESSION)) goto failure
  ::google::protobuf::uint32 tag;
  // @@protoc_insertion_point(parse_start:ls_packet.ServerAddress)
  for (;;) {
    ::std::pair< ::google::protobuf::uint32, bool> p = input->ReadTagWithCutoff(127);
    tag = p.first;
    if (!p.second) goto handle_unusual;
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // required bytes host = 1;
      case 1: {
        if (tag == 10) {
          DO_(::google::protobuf::internal::WireFormatLite::ReadBytes(
                input, this->mutable_host()));
        } else {
          goto handle_unusual;
        }
        if (input->ExpectTag(16)) goto parse_port;
        break;
      }

      // required int32 port = 2;
      case 2: {
        if (tag == 16) {
         parse_port:
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::int32, ::google::protobuf::internal::WireFormatLite::TYPE_INT32>(
                 input, &port_)));
          set_has_port();
        } else {
          goto handle_unusual;
        }
        if (input->ExpectAtEnd()) goto success;
        break;
      }

      default: {
      handle_unusual:
        if (tag == 0 ||
            ::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_END_GROUP) {
          goto success;
        }
        DO_(::google::protobuf::internal::WireFormat::SkipField(
              input, tag, mutable_unknown_fields()));
        break;
      }
    }
  }
success:
  // @@protoc_insertion_point(parse_success:ls_packet.ServerAddress)
  return true;
failure:
  // @@protoc_insertion_point(parse_failure:ls_packet.ServerAddress)
  return false;
#undef DO_
}

void ServerAddress::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // @@protoc_insertion_point(serialize_start:ls_packet.ServerAddress)
  // required bytes host = 1;
  if (has_host()) {
    ::google::protobuf::internal::WireFormatLite::WriteBytesMaybeAliased(
      1, this->host(), output);
  }

  // required int32 port = 2;
  if (has_port()) {
    ::google::protobuf::internal::WireFormatLite::WriteInt32(2, this->port(), output);
  }

  if (_internal_metadata_.have_unknown_fields()) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        unknown_fields(), output);
  }
  // @@protoc_insertion_point(serialize_end:ls_packet.ServerAddress)
}

::google::protobuf::uint8* ServerAddress::InternalSerializeWithCachedSizesToArray(
    bool deterministic, ::google::protobuf::uint8* target) const {
  // @@protoc_insertion_point(serialize_to_array_start:ls_packet.ServerAddress)
  // required bytes host = 1;
  if (has_host()) {
    target =
      ::google::protobuf::internal::WireFormatLite::WriteBytesToArray(
        1, this->host(), target);
  }

  // required int32 port = 2;
  if (has_port()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteInt32ToArray(2, this->port(), target);
  }

  if (_internal_metadata_.have_unknown_fields()) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        unknown_fields(), target);
  }
  // @@protoc_insertion_point(serialize_to_array_end:ls_packet.ServerAddress)
  return target;
}

int ServerAddress::RequiredFieldsByteSizeFallback() const {
// @@protoc_insertion_point(required_fields_byte_size_fallback_start:ls_packet.ServerAddress)
  int total_size = 0;

  if (has_host()) {
    // required bytes host = 1;
    total_size += 1 +
      ::google::protobuf::internal::WireFormatLite::BytesSize(
        this->host());
  }

  if (has_port()) {
    // required int32 port = 2;
    total_size += 1 +
      ::google::protobuf::internal::WireFormatLite::Int32Size(
        this->port());
  }

  return total_size;
}
int ServerAddress::ByteSize() const {
// @@protoc_insertion_point(message_byte_size_start:ls_packet.ServerAddress)
  int total_size = 0;

  if (((_has_bits_[0] & 0x00000003) ^ 0x00000003) == 0) {  // All required fields are present.
    // required bytes host = 1;
    total_size += 1 +
      ::google::protobuf::internal::WireFormatLite::BytesSize(
        this->host());

    // required int32 port = 2;
    total_size += 1 +
      ::google::protobuf::internal::WireFormatLite::Int32Size(
        this->port());

  } else {
    total_size += RequiredFieldsByteSizeFallback();
  }
  if (_internal_metadata_.have_unknown_fields()) {
    total_size +=
      ::google::protobuf::internal::WireFormat::ComputeUnknownFieldsSize(
        unknown_fields());
  }
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = total_size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
  return total_size;
}

void ServerAddress::MergeFrom(const ::google::protobuf::Message& from) {
// @@protoc_insertion_point(generalized_merge_from_start:ls_packet.ServerAddress)
  if (GOOGLE_PREDICT_FALSE(&from == this)) {
    ::google::protobuf::internal::MergeFromFail(__FILE__, __LINE__);
  }
  const ServerAddress* source = 
      ::google::protobuf::internal::DynamicCastToGenerated<const ServerAddress>(
          &from);
  if (source == NULL) {
  // @@protoc_insertion_point(generalized_merge_from_cast_fail:ls_packet.ServerAddress)
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
  // @@protoc_insertion_point(generalized_merge_from_cast_success:ls_packet.ServerAddress)
    MergeFrom(*source);
  }
}

void ServerAddress::MergeFrom(const ServerAddress& from) {
// @@protoc_insertion_point(class_specific_merge_from_start:ls_packet.ServerAddress)
  if (GOOGLE_PREDICT_FALSE(&from == this)) {
    ::google::protobuf::internal::MergeFromFail(__FILE__, __LINE__);
  }
  if (from._has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (from.has_host()) {
      set_has_host();
      host_.AssignWithDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), from.host_);
    }
    if (from.has_port()) {
      set_port(from.port());
    }
  }
  if (from._internal_metadata_.have_unknown_fields()) {
    mutable_unknown_fields()->MergeFrom(from.unknown_fields());
  }
}

void ServerAddress::CopyFrom(const ::google::protobuf::Message& from) {
// @@protoc_insertion_point(generalized_copy_from_start:ls_packet.ServerAddress)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void ServerAddress::CopyFrom(const ServerAddress& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:ls_packet.ServerAddress)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool ServerAddress::IsInitialized() const {
  if ((_has_bits_[0] & 0x00000003) != 0x00000003) return false;

  return true;
}

void ServerAddress::Swap(ServerAddress* other) {
  if (other == this) return;
  InternalSwap(other);
}
void ServerAddress::InternalSwap(ServerAddress* other) {
  host_.Swap(&other->host_);
  std::swap(port_, other->port_);
  std::swap(_has_bits_[0], other->_has_bits_[0]);
  _internal_metadata_.Swap(&other->_internal_metadata_);
  std::swap(_cached_size_, other->_cached_size_);
}

::google::protobuf::Metadata ServerAddress::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = ServerAddress_descriptor_;
  metadata.reflection = ServerAddress_reflection_;
  return metadata;
}

#if PROTOBUF_INLINE_NOT_IN_HEADERS
// ServerAddress

// required bytes host = 1;
bool ServerAddress::has_host() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
void ServerAddress::set_has_host() {
  _has_bits_[0] |= 0x00000001u;
}
void ServerAddress::clear_has_host() {
  _has_bits_[0] &= ~0x00000001u;
}
void ServerAddress::clear_host() {
  host_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  clear_has_host();
}
 const ::std::string& ServerAddress::host() const {
  // @@protoc_insertion_point(field_get:ls_packet.ServerAddress.host)
  return host_.GetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
 void ServerAddress::set_host(const ::std::string& value) {
  set_has_host();
  host_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), value);
  // @@protoc_insertion_point(field_set:ls_packet.ServerAddress.host)
}
 void ServerAddress::set_host(const char* value) {
  set_has_host();
  host_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::string(value));
  // @@protoc_insertion_point(field_set_char:ls_packet.ServerAddress.host)
}
 void ServerAddress::set_host(const void* value, size_t size) {
  set_has_host();
  host_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(),
      ::std::string(reinterpret_cast<const char*>(value), size));
  // @@protoc_insertion_point(field_set_pointer:ls_packet.ServerAddress.host)
}
 ::std::string* ServerAddress::mutable_host() {
  set_has_host();
  // @@protoc_insertion_point(field_mutable:ls_packet.ServerAddress.host)
  return host_.MutableNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
 ::std::string* ServerAddress::release_host() {
  // @@protoc_insertion_point(field_release:ls_packet.ServerAddress.host)
  clear_has_host();
  return host_.ReleaseNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
 void ServerAddress::set_allocated_host(::std::string* host) {
  if (host != NULL) {
    set_has_host();
  } else {
    clear_has_host();
  }
  host_.SetAllocatedNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), host);
  // @@protoc_insertion_point(field_set_allocated:ls_packet.ServerAddress.host)
}

// required int32 port = 2;
bool ServerAddress::has_port() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
void ServerAddress::set_has_port() {
  _has_bits_[0] |= 0x00000002u;
}
void ServerAddress::clear_has_port() {
  _has_bits_[0] &= ~0x00000002u;
}
void ServerAddress::clear_port() {
  port_ = 0;
  clear_has_port();
}
 ::google::protobuf::int32 ServerAddress::port() const {
  // @@protoc_insertion_point(field_get:ls_packet.ServerAddress.port)
  return port_;
}
 void ServerAddress::set_port(::google::protobuf::int32 value) {
  set_has_port();
  port_ = value;
  // @@protoc_insertion_point(field_set:ls_packet.ServerAddress.port)
}

#endif  // PROTOBUF_INLINE_NOT_IN_HEADERS

// ===================================================================

#if !defined(_MSC_VER) || _MSC_VER >= 1900
const int GameStart::kTypeFieldNumber;
#endif  // !defined(_MSC_VER) || _MSC_VER >= 1900

GameStart::GameStart()
  : ::google::protobuf::Message(), _internal_metadata_(NULL) {
  SharedCtor();
  // @@protoc_insertion_point(constructor:ls_packet.GameStart)
}

void GameStart::InitAsDefaultInstance() {
}

GameStart::GameStart(const GameStart& from)
  : ::google::protobuf::Message(),
    _internal_metadata_(NULL) {
  SharedCtor();
  MergeFrom(from);
  // @@protoc_insertion_point(copy_constructor:ls_packet.GameStart)
}

void GameStart::SharedCtor() {
  _cached_size_ = 0;
  type_ = 0;
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

GameStart::~GameStart() {
  // @@protoc_insertion_point(destructor:ls_packet.GameStart)
  SharedDtor();
}

void GameStart::SharedDtor() {
  if (this != default_instance_) {
  }
}

void GameStart::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* GameStart::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return GameStart_descriptor_;
}

const GameStart& GameStart::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_ls_5fpacket_2eproto();
  return *default_instance_;
}

GameStart* GameStart::default_instance_ = NULL;

GameStart* GameStart::New(::google::protobuf::Arena* arena) const {
  GameStart* n = new GameStart;
  if (arena != NULL) {
    arena->Own(n);
  }
  return n;
}

void GameStart::Clear() {
// @@protoc_insertion_point(message_clear_start:ls_packet.GameStart)
  type_ = 0;
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  if (_internal_metadata_.have_unknown_fields()) {
    mutable_unknown_fields()->Clear();
  }
}

bool GameStart::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!GOOGLE_PREDICT_TRUE(EXPRESSION)) goto failure
  ::google::protobuf::uint32 tag;
  // @@protoc_insertion_point(parse_start:ls_packet.GameStart)
  for (;;) {
    ::std::pair< ::google::protobuf::uint32, bool> p = input->ReadTagWithCutoff(127);
    tag = p.first;
    if (!p.second) goto handle_unusual;
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // required int32 type = 1;
      case 1: {
        if (tag == 8) {
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::int32, ::google::protobuf::internal::WireFormatLite::TYPE_INT32>(
                 input, &type_)));
          set_has_type();
        } else {
          goto handle_unusual;
        }
        if (input->ExpectAtEnd()) goto success;
        break;
      }

      default: {
      handle_unusual:
        if (tag == 0 ||
            ::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_END_GROUP) {
          goto success;
        }
        DO_(::google::protobuf::internal::WireFormat::SkipField(
              input, tag, mutable_unknown_fields()));
        break;
      }
    }
  }
success:
  // @@protoc_insertion_point(parse_success:ls_packet.GameStart)
  return true;
failure:
  // @@protoc_insertion_point(parse_failure:ls_packet.GameStart)
  return false;
#undef DO_
}

void GameStart::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // @@protoc_insertion_point(serialize_start:ls_packet.GameStart)
  // required int32 type = 1;
  if (has_type()) {
    ::google::protobuf::internal::WireFormatLite::WriteInt32(1, this->type(), output);
  }

  if (_internal_metadata_.have_unknown_fields()) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        unknown_fields(), output);
  }
  // @@protoc_insertion_point(serialize_end:ls_packet.GameStart)
}

::google::protobuf::uint8* GameStart::InternalSerializeWithCachedSizesToArray(
    bool deterministic, ::google::protobuf::uint8* target) const {
  // @@protoc_insertion_point(serialize_to_array_start:ls_packet.GameStart)
  // required int32 type = 1;
  if (has_type()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteInt32ToArray(1, this->type(), target);
  }

  if (_internal_metadata_.have_unknown_fields()) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        unknown_fields(), target);
  }
  // @@protoc_insertion_point(serialize_to_array_end:ls_packet.GameStart)
  return target;
}

int GameStart::ByteSize() const {
// @@protoc_insertion_point(message_byte_size_start:ls_packet.GameStart)
  int total_size = 0;

  // required int32 type = 1;
  if (has_type()) {
    total_size += 1 +
      ::google::protobuf::internal::WireFormatLite::Int32Size(
        this->type());
  }
  if (_internal_metadata_.have_unknown_fields()) {
    total_size +=
      ::google::protobuf::internal::WireFormat::ComputeUnknownFieldsSize(
        unknown_fields());
  }
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = total_size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
  return total_size;
}

void GameStart::MergeFrom(const ::google::protobuf::Message& from) {
// @@protoc_insertion_point(generalized_merge_from_start:ls_packet.GameStart)
  if (GOOGLE_PREDICT_FALSE(&from == this)) {
    ::google::protobuf::internal::MergeFromFail(__FILE__, __LINE__);
  }
  const GameStart* source = 
      ::google::protobuf::internal::DynamicCastToGenerated<const GameStart>(
          &from);
  if (source == NULL) {
  // @@protoc_insertion_point(generalized_merge_from_cast_fail:ls_packet.GameStart)
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
  // @@protoc_insertion_point(generalized_merge_from_cast_success:ls_packet.GameStart)
    MergeFrom(*source);
  }
}

void GameStart::MergeFrom(const GameStart& from) {
// @@protoc_insertion_point(class_specific_merge_from_start:ls_packet.GameStart)
  if (GOOGLE_PREDICT_FALSE(&from == this)) {
    ::google::protobuf::internal::MergeFromFail(__FILE__, __LINE__);
  }
  if (from._has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (from.has_type()) {
      set_type(from.type());
    }
  }
  if (from._internal_metadata_.have_unknown_fields()) {
    mutable_unknown_fields()->MergeFrom(from.unknown_fields());
  }
}

void GameStart::CopyFrom(const ::google::protobuf::Message& from) {
// @@protoc_insertion_point(generalized_copy_from_start:ls_packet.GameStart)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void GameStart::CopyFrom(const GameStart& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:ls_packet.GameStart)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool GameStart::IsInitialized() const {
  if ((_has_bits_[0] & 0x00000001) != 0x00000001) return false;

  return true;
}

void GameStart::Swap(GameStart* other) {
  if (other == this) return;
  InternalSwap(other);
}
void GameStart::InternalSwap(GameStart* other) {
  std::swap(type_, other->type_);
  std::swap(_has_bits_[0], other->_has_bits_[0]);
  _internal_metadata_.Swap(&other->_internal_metadata_);
  std::swap(_cached_size_, other->_cached_size_);
}

::google::protobuf::Metadata GameStart::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = GameStart_descriptor_;
  metadata.reflection = GameStart_reflection_;
  return metadata;
}

#if PROTOBUF_INLINE_NOT_IN_HEADERS
// GameStart

// required int32 type = 1;
bool GameStart::has_type() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
void GameStart::set_has_type() {
  _has_bits_[0] |= 0x00000001u;
}
void GameStart::clear_has_type() {
  _has_bits_[0] &= ~0x00000001u;
}
void GameStart::clear_type() {
  type_ = 0;
  clear_has_type();
}
 ::google::protobuf::int32 GameStart::type() const {
  // @@protoc_insertion_point(field_get:ls_packet.GameStart.type)
  return type_;
}
 void GameStart::set_type(::google::protobuf::int32 value) {
  set_has_type();
  type_ = value;
  // @@protoc_insertion_point(field_set:ls_packet.GameStart.type)
}

#endif  // PROTOBUF_INLINE_NOT_IN_HEADERS

// @@protoc_insertion_point(namespace_scope)

}  // namespace ls_packet

// @@protoc_insertion_point(global_scope)
