#ifndef DATABASE_HPP
#define DATABASE_HPP

#include <cstdint>

#include <napi.h>
#include <pstore/core/database.hpp>
#include <pstore/core/index_types.hpp>

class myc final : public Napi::ObjectWrap<myc> {
public:
  explicit myc (Napi::CallbackInfo const & info)
          : ObjectWrap (info) {}
  Napi::Value get (Napi::CallbackInfo const & info) {
    Napi::Env env = info.Env ();
    if (info.Length () != 0) {
      throw Napi::TypeError::New (env, "wrong number of arguments");
    }
    return Napi::Number::New (env, 1234);
  }

  static Napi::Function get_class (Napi::Env env) {
    return DefineClass (env, "myc",
                        {
                          myc::InstanceMethod ("get", &myc::get),
                        });
  }
};

class database final : public Napi::ObjectWrap<database> {
public:
  explicit database (Napi::CallbackInfo const & info);

  Napi::Value id (Napi::CallbackInfo const & info);
  Napi::Value path (Napi::CallbackInfo const & info);
  Napi::Value size (Napi::CallbackInfo const & info);

  // Returns the revision to which the database is currently synched.
  Napi::Value revision (Napi::CallbackInfo const & info);

  static constexpr auto head_revision = std::int64_t{-1};

  Napi::Value sync (Napi::CallbackInfo const & info);

  Napi::Value get_index (Napi::CallbackInfo const & info);


  static Napi::Function get_class (Napi::Env env);

private:
  static Napi::FunctionReference constructor;
  std::shared_ptr<pstore::database> db_;
};

class compilation_index : public Napi::ObjectWrap<compilation_index> {
public:
private:
  std::shared_ptr<pstore::index::compilation_index> index_;
};

#endif // DATABASE_HPP
