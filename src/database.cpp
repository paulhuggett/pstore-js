#include "database.hpp"

database::database (Napi::CallbackInfo const & info)
        : ObjectWrap (info) {
    Napi::Env env = info.Env ();
    try {
        if (info.Length () != 1) {
            throw Napi::TypeError::New (env, "wrong number of arguments");
        }

        if (!info[0].IsString ()) {
            Napi::TypeError::New (env, "Argument must be string (path to the database)")
                .ThrowAsJavaScriptException ();
            return;
        }

        auto const path = info[0].As<Napi::String> ().Utf8Value ();
        db_ = std::make_shared<pstore::database> (path, pstore::database::access_mode::read_only);
    } catch (std::exception const & ex) {
        Napi::Error::New (env, ex.what ()).ThrowAsJavaScriptException ();
    } catch (...) {
        Napi::Error::New (env, "unknown error").ThrowAsJavaScriptException ();
    }
}

namespace {

    template <typename Function, typename... Args>
    Napi::Value error_wrap (Napi::Env env, Function function, Args &&... args) {
        try {
            return function (std::forward<Args> (args)...);
        } catch (Napi::Error const & err) {
            throw;
        } catch (std::exception const & ex) {
            Napi::Error::New (env, ex.what ()).ThrowAsJavaScriptException ();
        } catch (...) {
            Napi::Error::New (env, "unknown error").ThrowAsJavaScriptException ();
        }
        return env.Null ();
    }

} // end anonymous namespace

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

Napi::Function database::get_class (Napi::Env env) {
    return DefineClass (env, "Database",
                        {
                            database::InstanceMethod ("id", &database::id),
                            database::InstanceMethod ("path", &database::path),
                            database::InstanceMethod ("revision", &database::revision),
                            database::InstanceMethod ("size", &database::size),
                        });
}


Napi::Object init (Napi::Env env, Napi::Object exports) {
    Napi::String name = Napi::String::New (env, "Database");
    exports.Set (name, database::get_class (env));
    return exports;
}

NODE_API_MODULE (addon, init)
