#ifndef DATABASE_HPP
#define DATABASE_HPP

#include <cstdint>

//#define NODE_API_NO_EXTERNAL_BUFFERS_ALLOWED
#include <napi.h>

#include <pstore/core/database.hpp>
#include <pstore/core/index_types.hpp>
#include <pstore/core/hamt_map.hpp>

class database final : public Napi::ObjectWrap<database> {
public:
  explicit database (Napi::CallbackInfo const & info);
  static Napi::Object init (Napi::Env env, Napi::Object exports);

  // get(addr,size)/get(extent)
  // Returns a buffer containing the data at the extent given by addr,size.
  Napi::Value get (Napi::CallbackInfo const & info);
  // id()
  // Returns the database UUID.
  Napi::Value id (Napi::CallbackInfo const & info);
  // path()
  // Returns the path of the database store file.
  Napi::Value path (Napi::CallbackInfo const & info);
  // size()
  // Returns the logical size in bytes of the database store file. Note that,
  // on some platforms, this may be less than the physical size of the file.
  Napi::Value size (Napi::CallbackInfo const & info);
  // revision()
  // Returns the revision to which the database is currently synched.
  Napi::Value revision (Napi::CallbackInfo const & info);

  static constexpr auto head_revision = std::int64_t{-1};
  Napi::Value sync (Napi::CallbackInfo const & info);

  // index(name[,create])
  // Returns the named index. If the index does not yet exist and 'create' is
  // true or missing then it create; if 'create' is false, null is returned.
  Napi::Value index (Napi::CallbackInfo const & info);

private:
  Napi::Value get (Napi::Env env, int64_t addr, int64_t size);
  Napi::Value get (Napi::Env env, Napi::Value addr, Napi::Value size);
  Napi::Value get (Napi::Env env, Napi::Object extent);

  std::shared_ptr<pstore::database> db_;
};

class write_index : public Napi::ObjectWrap<write_index> {
public:
  explicit write_index (Napi::CallbackInfo const & info)
          : ObjectWrap (info) {}
  static Napi::Value new_instance (Napi::CallbackInfo const & info,
                                   std::shared_ptr<pstore::database> const & db, bool create);
  static void init (Napi::Env env);

  // Returns the number of entries in the index.
  Napi::Value size (Napi::CallbackInfo const & info);
  Napi::Value iterator (Napi::CallbackInfo const & info);

private:
  void set (std::shared_ptr<pstore::database> const & db,
            std::shared_ptr<pstore::index::write_index> const & index) {
    db_ = db;
    index_ = index;
  }
  static Napi::FunctionReference constructor_;
  std::shared_ptr<pstore::database> db_;
  std::shared_ptr<pstore::index::write_index> index_;
};

#endif // DATABASE_HPP
