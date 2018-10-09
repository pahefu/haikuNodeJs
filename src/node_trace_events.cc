#include "node.h"
#include "node_internals.h"
#include "tracing/agent.h"
#include "env.h"
#include "base_object-inl.h"

#include <set>
#include <string>

namespace node {

using v8::Array;
using v8::Context;
using v8::FunctionCallbackInfo;
using v8::FunctionTemplate;
using v8::Local;
using v8::Object;
using v8::String;
using v8::Value;

class NodeCategorySet : public BaseObject {
 public:
  static void New(const FunctionCallbackInfo<Value>& args);
  static void Enable(const FunctionCallbackInfo<Value>& args);
  static void Disable(const FunctionCallbackInfo<Value>& args);

  const std::set<std::string>& GetCategories() const { return categories_; }

  void MemoryInfo(MemoryTracker* tracker) const override {
    tracker->TrackThis(this);
    tracker->TrackField("categories", categories_);
  }

  ADD_MEMORY_INFO_NAME(NodeCategorySet)

 private:
  NodeCategorySet(Environment* env,
                  Local<Object> wrap,
                  std::set<std::string>&& categories) :
        BaseObject(env, wrap), categories_(std::move(categories)) {
    MakeWeak();
  }

  bool enabled_ = false;
  const std::set<std::string> categories_;
};

void NodeCategorySet::New(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);
  std::set<std::string> categories;
  CHECK(args[0]->IsArray());
  Local<Array> cats = args[0].As<Array>();
  for (size_t n = 0; n < cats->Length(); n++) {
    Local<Value> category;
    if (!cats->Get(env->context(), n).ToLocal(&category)) return;
    Utf8Value val(env->isolate(), category);
    if (!*val) return;
    categories.emplace(*val);
  }
  CHECK_NOT_NULL(env->tracing_agent_writer());
  new NodeCategorySet(env, args.This(), std::move(categories));
}

void NodeCategorySet::Enable(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);
  NodeCategorySet* category_set;
  ASSIGN_OR_RETURN_UNWRAP(&category_set, args.Holder());
  CHECK_NOT_NULL(category_set);
  const auto& categories = category_set->GetCategories();
  if (!category_set->enabled_ && !categories.empty()) {
    env->tracing_agent_writer()->Enable(categories);
    category_set->enabled_ = true;
  }
}

void NodeCategorySet::Disable(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);
  NodeCategorySet* category_set;
  ASSIGN_OR_RETURN_UNWRAP(&category_set, args.Holder());
  CHECK_NOT_NULL(category_set);
  const auto& categories = category_set->GetCategories();
  if (category_set->enabled_ && !categories.empty()) {
    env->tracing_agent_writer()->Disable(categories);
    category_set->enabled_ = false;
  }
}

void GetEnabledCategories(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);
  std::string categories =
      env->tracing_agent_writer()->agent()->GetEnabledCategories();
  if (!categories.empty()) {
    args.GetReturnValue().Set(
      String::NewFromUtf8(env->isolate(),
                          categories.c_str(),
                          v8::NewStringType::kNormal,
                          categories.size()).ToLocalChecked());
  }
}

void Initialize(Local<Object> target,
                Local<Value> unused,
                Local<Context> context,
                void* priv) {
  Environment* env = Environment::GetCurrent(context);

  env->SetMethod(target, "getEnabledCategories", GetEnabledCategories);

  Local<FunctionTemplate> category_set =
      env->NewFunctionTemplate(NodeCategorySet::New);
  category_set->InstanceTemplate()->SetInternalFieldCount(1);
  env->SetProtoMethod(category_set, "enable", NodeCategorySet::Enable);
  env->SetProtoMethod(category_set, "disable", NodeCategorySet::Disable);

  target->Set(FIXED_ONE_BYTE_STRING(env->isolate(), "CategorySet"),
              category_set->GetFunction(env->context()).ToLocalChecked());

  Local<String> isTraceCategoryEnabled =
      FIXED_ONE_BYTE_STRING(env->isolate(), "isTraceCategoryEnabled");
  Local<String> trace = FIXED_ONE_BYTE_STRING(env->isolate(), "trace");

  // Grab the trace and isTraceCategoryEnabled intrinsics from the binding
  // object and expose those to our binding layer.
  Local<Object> binding = context->GetExtrasBindingObject();
  target->Set(context, isTraceCategoryEnabled,
              binding->Get(context, isTraceCategoryEnabled).ToLocalChecked())
                  .FromJust();
  target->Set(context, trace,
              binding->Get(context, trace).ToLocalChecked()).FromJust();

  target->Set(context,
              FIXED_ONE_BYTE_STRING(env->isolate(), "traceCategoryState"),
              env->trace_category_state().GetJSArray()).FromJust();
}

}  // namespace node

NODE_MODULE_CONTEXT_AWARE_INTERNAL(trace_events, node::Initialize)
