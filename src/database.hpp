#ifndef DATABASE_HPP
#define DATABASE_HPP

#include <napi.h>
#include <pstore/core/database.hpp>

class database : public Napi::ObjectWrap<database> {
public:
    database (Napi::CallbackInfo const & info);

    Napi::Value id (Napi::CallbackInfo const & info);
    Napi::Value path (Napi::CallbackInfo const & info);
    Napi::Value size (Napi::CallbackInfo const & info);
    // Returns the revision to which the database is currently synched.
    Napi::Value revision (Napi::CallbackInfo const & info);

    static Napi::Function get_class (Napi::Env env);

private:
    std::shared_ptr<pstore::database> db_;
};

#endif // DATABASE_HPP
