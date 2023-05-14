
#include "database.hpp"

#include <cfloat>
#include <cmath>
#include <optional>
#include <iostream>

namespace {

  template <typename Function, typename... Args>
  decltype (auto) error_wrap (Napi::Env env, Function function, Args &&... args) {
    try {
      return function (std::forward<Args> (args)...);
    } catch (Napi::Error const & err) {
      throw;
    } catch (std::exception const & ex) {
      throw Napi::Error::New (env, ex.what ());
    } catch (...) {
      throw Napi::Error::New (env, "unknown error");
    }
  }

} // end anonymous namespace

database::database (Napi::CallbackInfo const & info)
        : ObjectWrap (info) {
  Napi::Env env = info.Env ();
  error_wrap (env, [&] () {
    auto const path = info[0].As<Napi::String> ().Utf8Value ();
    db_ = std::make_shared<pstore::database> (path, pstore::database::access_mode::read_only);

    {
      Napi::HandleScope scope (env);

      Napi::Function ctor = myc::get_class (env);
      constructor = Napi::Persistent (ctor);
      constructor.SuppressDestruct ();
      target.Set ("Canvas", ctor);
    }
  });
}

Napi::Value database::size (Napi::CallbackInfo const & info) {
    Napi::Env env = info.Env ();
    if (info.Length () != 0) {
        throw Napi::TypeError::New (env, "wrong number of arguments");
    }
    return error_wrap (env, [&] () { return Napi::Number::New (env, db_->size ()); });
}

Napi::Value database::id (Napi::CallbackInfo const & info) {
    Napi::Env env = info.Env ();
    if (info.Length () != 0) {
        throw Napi::TypeError::New (env, "wrong number of arguments");
    }
    return error_wrap (env,
                       [&] () { return Napi::String::New (env, db_->get_header ().id ().str ()); });
}

Napi::Value database::path (Napi::CallbackInfo const & info) {
    Napi::Env env = info.Env ();
    if (info.Length () != 0) {
        throw Napi::TypeError::New (env, "wrong number of arguments");
    }
    return error_wrap (env, [&] () { return Napi::String::New (env, db_->path ()); });
}

Napi::Value database::revision (Napi::CallbackInfo const & info) {
    Napi::Env env = info.Env ();
    if (info.Length () != 0) {
        throw Napi::TypeError::New (env, "wrong number of arguments");
    }
    return error_wrap (env,
                       [&] () { return Napi::Number::New (env, db_->get_current_revision ()); });
}

namespace {

    std::int64_t as_int64 (Napi::Env env, Napi::Value const & value) {
        auto const d = value.As<Napi::Number> ().DoubleValue ();
        if (std::isnan (d) || std::isinf (d) || d < std::numeric_limits<std::int64_t>::min () ||
            d > std::numeric_limits<std::int64_t>::max ()) {
          throw Napi::TypeError::New (env, "An integer was expected");
        }
        return value.As<Napi::Number> ().Int64Value ();
    }

    void check_number_of_arguments (Napi::CallbackInfo const & info, unsigned const expected) {
        if (info.Length () != expected) {
          throw Napi::TypeError::New (info.Env (), "wrong number of arguments");
        }
    }

} // end anonymous namespace

Napi::Value database::sync (Napi::CallbackInfo const & info) {
    Napi::Env env = info.Env ();


    return error_wrap (env, [&] () {
      check_number_of_arguments (info, 1U);
      auto const v = as_int64 (env, info[0]);
      auto revision = 0U;
      if (v == head_revision) {
        revision = pstore::head_revision;
      } else if (v < std::numeric_limits<decltype (revision)>::min () ||
                 v > std::numeric_limits<decltype (revision)>::max ()) {
        throw Napi::TypeError::New (env, "Revision number out of range");
      } else {
        revision = static_cast<unsigned> (v);
      }

      db_->sync (revision);
      return Napi::Number::New (env, db_->get_current_revision ());
    });
}

Napi::Value database::get_index (Napi::CallbackInfo const & info) {
    Napi::Env env = info.Env ();
    check_number_of_arguments (info, 1U);
    std::string const name = info[0].As<Napi::String> ().Utf8Value ();
    if (name == "compilation") {
    }
    throw Napi::TypeError::New (env, "Unknown index");
}

Napi::Function database::get_class (Napi::Env env) {
    return DefineClass (env, "Database",
                        {
                          database::InstanceMethod ("get_index", &database::get_index),
                          database::InstanceMethod ("id", &database::id),
                          database::InstanceMethod ("path", &database::path),
                          database::InstanceMethod ("revision", &database::revision),
                          database::InstanceMethod ("size", &database::size),
                          database::InstanceMethod ("sync", &database::sync),
                        });
}


Napi::Object init (Napi::Env env, Napi::Object exports) {
    exports.Set (Napi::String::New (env, "Database"), database::get_class (env));
    return exports;
}

NODE_API_MODULE (addon, init)
