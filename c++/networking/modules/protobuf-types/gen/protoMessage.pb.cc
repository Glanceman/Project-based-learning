// Generated by the protocol buffer compiler.  DO NOT EDIT!
// NO CHECKED-IN PROTOBUF GENCODE
// source: protoMessage.proto
// Protobuf C++ Version: 5.27.3

#include "protoMessage.pb.h"

#include <algorithm>
#include <type_traits>
#include "google/protobuf/io/coded_stream.h"
#include "google/protobuf/generated_message_tctable_impl.h"
#include "google/protobuf/extension_set.h"
#include "google/protobuf/wire_format_lite.h"
#include "google/protobuf/descriptor.h"
#include "google/protobuf/generated_message_reflection.h"
#include "google/protobuf/reflection_ops.h"
#include "google/protobuf/wire_format.h"
// @@protoc_insertion_point(includes)

// Must be included last.
#include "google/protobuf/port_def.inc"
PROTOBUF_PRAGMA_INIT_SEG
namespace _pb = ::google::protobuf;
namespace _pbi = ::google::protobuf::internal;
namespace _fl = ::google::protobuf::internal::field_layout;
namespace ProtoMessage {

inline constexpr Message::Impl_::Impl_(
    ::_pbi::ConstantInitialized) noexcept
      : msg_(
            &::google::protobuf::internal::fixed_address_empty_string,
            ::_pbi::ConstantInitialized()),
        val_{0},
        _cached_size_{0} {}

template <typename>
PROTOBUF_CONSTEXPR Message::Message(::_pbi::ConstantInitialized)
    : _impl_(::_pbi::ConstantInitialized()) {}
struct MessageDefaultTypeInternal {
  PROTOBUF_CONSTEXPR MessageDefaultTypeInternal() : _instance(::_pbi::ConstantInitialized{}) {}
  ~MessageDefaultTypeInternal() {}
  union {
    Message _instance;
  };
};

PROTOBUF_ATTRIBUTE_NO_DESTROY PROTOBUF_CONSTINIT
    PROTOBUF_ATTRIBUTE_INIT_PRIORITY1 MessageDefaultTypeInternal _Message_default_instance_;
}  // namespace ProtoMessage
static constexpr const ::_pb::EnumDescriptor**
    file_level_enum_descriptors_protoMessage_2eproto = nullptr;
static constexpr const ::_pb::ServiceDescriptor**
    file_level_service_descriptors_protoMessage_2eproto = nullptr;
const ::uint32_t
    TableStruct_protoMessage_2eproto::offsets[] ABSL_ATTRIBUTE_SECTION_VARIABLE(
        protodesc_cold) = {
        ~0u,  // no _has_bits_
        PROTOBUF_FIELD_OFFSET(::ProtoMessage::Message, _internal_metadata_),
        ~0u,  // no _extensions_
        ~0u,  // no _oneof_case_
        ~0u,  // no _weak_field_map_
        ~0u,  // no _inlined_string_donated_
        ~0u,  // no _split_
        ~0u,  // no sizeof(Split)
        PROTOBUF_FIELD_OFFSET(::ProtoMessage::Message, _impl_.msg_),
        PROTOBUF_FIELD_OFFSET(::ProtoMessage::Message, _impl_.val_),
};

static const ::_pbi::MigrationSchema
    schemas[] ABSL_ATTRIBUTE_SECTION_VARIABLE(protodesc_cold) = {
        {0, -1, -1, sizeof(::ProtoMessage::Message)},
};
static const ::_pb::Message* const file_default_instances[] = {
    &::ProtoMessage::_Message_default_instance_._instance,
};
const char descriptor_table_protodef_protoMessage_2eproto[] ABSL_ATTRIBUTE_SECTION_VARIABLE(
    protodesc_cold) = {
    "\n\022protoMessage.proto\022\014ProtoMessage\"#\n\007Me"
    "ssage\022\013\n\003msg\030\001 \001(\t\022\013\n\003val\030\002 \001(\005b\006proto3"
};
static ::absl::once_flag descriptor_table_protoMessage_2eproto_once;
PROTOBUF_CONSTINIT const ::_pbi::DescriptorTable descriptor_table_protoMessage_2eproto = {
    false,
    false,
    79,
    descriptor_table_protodef_protoMessage_2eproto,
    "protoMessage.proto",
    &descriptor_table_protoMessage_2eproto_once,
    nullptr,
    0,
    1,
    schemas,
    file_default_instances,
    TableStruct_protoMessage_2eproto::offsets,
    file_level_enum_descriptors_protoMessage_2eproto,
    file_level_service_descriptors_protoMessage_2eproto,
};
namespace ProtoMessage {
// ===================================================================

class Message::_Internal {
 public:
};

Message::Message(::google::protobuf::Arena* arena)
    : ::google::protobuf::Message(arena) {
  SharedCtor(arena);
  // @@protoc_insertion_point(arena_constructor:ProtoMessage.Message)
}
inline PROTOBUF_NDEBUG_INLINE Message::Impl_::Impl_(
    ::google::protobuf::internal::InternalVisibility visibility, ::google::protobuf::Arena* arena,
    const Impl_& from, const ::ProtoMessage::Message& from_msg)
      : msg_(arena, from.msg_),
        _cached_size_{0} {}

Message::Message(
    ::google::protobuf::Arena* arena,
    const Message& from)
    : ::google::protobuf::Message(arena) {
  Message* const _this = this;
  (void)_this;
  _internal_metadata_.MergeFrom<::google::protobuf::UnknownFieldSet>(
      from._internal_metadata_);
  new (&_impl_) Impl_(internal_visibility(), arena, from._impl_, from);
  _impl_.val_ = from._impl_.val_;

  // @@protoc_insertion_point(copy_constructor:ProtoMessage.Message)
}
inline PROTOBUF_NDEBUG_INLINE Message::Impl_::Impl_(
    ::google::protobuf::internal::InternalVisibility visibility,
    ::google::protobuf::Arena* arena)
      : msg_(arena),
        _cached_size_{0} {}

inline void Message::SharedCtor(::_pb::Arena* arena) {
  new (&_impl_) Impl_(internal_visibility(), arena);
  _impl_.val_ = {};
}
Message::~Message() {
  // @@protoc_insertion_point(destructor:ProtoMessage.Message)
  _internal_metadata_.Delete<::google::protobuf::UnknownFieldSet>();
  SharedDtor();
}
inline void Message::SharedDtor() {
  ABSL_DCHECK(GetArena() == nullptr);
  _impl_.msg_.Destroy();
  _impl_.~Impl_();
}

const ::google::protobuf::MessageLite::ClassData*
Message::GetClassData() const {
  PROTOBUF_CONSTINIT static const ::google::protobuf::MessageLite::
      ClassDataFull _data_ = {
          {
              &_table_.header,
              nullptr,  // OnDemandRegisterArenaDtor
              nullptr,  // IsInitialized
              PROTOBUF_FIELD_OFFSET(Message, _impl_._cached_size_),
              false,
          },
          &Message::MergeImpl,
          &Message::kDescriptorMethods,
          &descriptor_table_protoMessage_2eproto,
          nullptr,  // tracker
      };
  ::google::protobuf::internal::PrefetchToLocalCache(&_data_);
  ::google::protobuf::internal::PrefetchToLocalCache(_data_.tc_table);
  return _data_.base();
}
PROTOBUF_CONSTINIT PROTOBUF_ATTRIBUTE_INIT_PRIORITY1
const ::_pbi::TcParseTable<1, 2, 0, 32, 2> Message::_table_ = {
  {
    0,  // no _has_bits_
    0, // no _extensions_
    2, 8,  // max_field_number, fast_idx_mask
    offsetof(decltype(_table_), field_lookup_table),
    4294967292,  // skipmap
    offsetof(decltype(_table_), field_entries),
    2,  // num_field_entries
    0,  // num_aux_entries
    offsetof(decltype(_table_), field_names),  // no aux_entries
    &_Message_default_instance_._instance,
    nullptr,  // post_loop_handler
    ::_pbi::TcParser::GenericFallback,  // fallback
    #ifdef PROTOBUF_PREFETCH_PARSE_TABLE
    ::_pbi::TcParser::GetTable<::ProtoMessage::Message>(),  // to_prefetch
    #endif  // PROTOBUF_PREFETCH_PARSE_TABLE
  }, {{
    // int32 val = 2;
    {::_pbi::TcParser::SingularVarintNoZag1<::uint32_t, offsetof(Message, _impl_.val_), 63>(),
     {16, 63, 0, PROTOBUF_FIELD_OFFSET(Message, _impl_.val_)}},
    // string msg = 1;
    {::_pbi::TcParser::FastUS1,
     {10, 63, 0, PROTOBUF_FIELD_OFFSET(Message, _impl_.msg_)}},
  }}, {{
    65535, 65535
  }}, {{
    // string msg = 1;
    {PROTOBUF_FIELD_OFFSET(Message, _impl_.msg_), 0, 0,
    (0 | ::_fl::kFcSingular | ::_fl::kUtf8String | ::_fl::kRepAString)},
    // int32 val = 2;
    {PROTOBUF_FIELD_OFFSET(Message, _impl_.val_), 0, 0,
    (0 | ::_fl::kFcSingular | ::_fl::kInt32)},
  }},
  // no aux_entries
  {{
    "\24\3\0\0\0\0\0\0"
    "ProtoMessage.Message"
    "msg"
  }},
};

PROTOBUF_NOINLINE void Message::Clear() {
// @@protoc_insertion_point(message_clear_start:ProtoMessage.Message)
  ::google::protobuf::internal::TSanWrite(&_impl_);
  ::uint32_t cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  _impl_.msg_.ClearToEmpty();
  _impl_.val_ = 0;
  _internal_metadata_.Clear<::google::protobuf::UnknownFieldSet>();
}

::uint8_t* Message::_InternalSerialize(
    ::uint8_t* target,
    ::google::protobuf::io::EpsCopyOutputStream* stream) const {
  // @@protoc_insertion_point(serialize_to_array_start:ProtoMessage.Message)
  ::uint32_t cached_has_bits = 0;
  (void)cached_has_bits;

  // string msg = 1;
  if (!this->_internal_msg().empty()) {
    const std::string& _s = this->_internal_msg();
    ::google::protobuf::internal::WireFormatLite::VerifyUtf8String(
        _s.data(), static_cast<int>(_s.length()), ::google::protobuf::internal::WireFormatLite::SERIALIZE, "ProtoMessage.Message.msg");
    target = stream->WriteStringMaybeAliased(1, _s, target);
  }

  // int32 val = 2;
  if (this->_internal_val() != 0) {
    target = ::google::protobuf::internal::WireFormatLite::
        WriteInt32ToArrayWithField<2>(
            stream, this->_internal_val(), target);
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    target =
        ::_pbi::WireFormat::InternalSerializeUnknownFieldsToArray(
            _internal_metadata_.unknown_fields<::google::protobuf::UnknownFieldSet>(::google::protobuf::UnknownFieldSet::default_instance), target, stream);
  }
  // @@protoc_insertion_point(serialize_to_array_end:ProtoMessage.Message)
  return target;
}

::size_t Message::ByteSizeLong() const {
// @@protoc_insertion_point(message_byte_size_start:ProtoMessage.Message)
  ::size_t total_size = 0;

  ::uint32_t cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  ::_pbi::Prefetch5LinesFrom7Lines(reinterpret_cast<const void*>(this));
  // string msg = 1;
  if (!this->_internal_msg().empty()) {
    total_size += 1 + ::google::protobuf::internal::WireFormatLite::StringSize(
                                    this->_internal_msg());
  }

  // int32 val = 2;
  if (this->_internal_val() != 0) {
    total_size += ::_pbi::WireFormatLite::Int32SizePlusOne(
        this->_internal_val());
  }

  return MaybeComputeUnknownFieldsSize(total_size, &_impl_._cached_size_);
}


void Message::MergeImpl(::google::protobuf::MessageLite& to_msg, const ::google::protobuf::MessageLite& from_msg) {
  auto* const _this = static_cast<Message*>(&to_msg);
  auto& from = static_cast<const Message&>(from_msg);
  // @@protoc_insertion_point(class_specific_merge_from_start:ProtoMessage.Message)
  ABSL_DCHECK_NE(&from, _this);
  ::uint32_t cached_has_bits = 0;
  (void) cached_has_bits;

  if (!from._internal_msg().empty()) {
    _this->_internal_set_msg(from._internal_msg());
  }
  if (from._internal_val() != 0) {
    _this->_impl_.val_ = from._impl_.val_;
  }
  _this->_internal_metadata_.MergeFrom<::google::protobuf::UnknownFieldSet>(from._internal_metadata_);
}

void Message::CopyFrom(const Message& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:ProtoMessage.Message)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}


void Message::InternalSwap(Message* PROTOBUF_RESTRICT other) {
  using std::swap;
  auto* arena = GetArena();
  ABSL_DCHECK_EQ(arena, other->GetArena());
  _internal_metadata_.InternalSwap(&other->_internal_metadata_);
  ::_pbi::ArenaStringPtr::InternalSwap(&_impl_.msg_, &other->_impl_.msg_, arena);
        swap(_impl_.val_, other->_impl_.val_);
}

::google::protobuf::Metadata Message::GetMetadata() const {
  return ::google::protobuf::Message::GetMetadataImpl(GetClassData()->full());
}
// @@protoc_insertion_point(namespace_scope)
}  // namespace ProtoMessage
namespace google {
namespace protobuf {
}  // namespace protobuf
}  // namespace google
// @@protoc_insertion_point(global_scope)
PROTOBUF_ATTRIBUTE_INIT_PRIORITY2 static ::std::false_type
    _static_init2_ PROTOBUF_UNUSED =
        (::_pbi::AddDescriptors(&descriptor_table_protoMessage_2eproto),
         ::std::false_type{});
#include "google/protobuf/port_undef.inc"
