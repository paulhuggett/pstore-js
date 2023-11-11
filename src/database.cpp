
#include "database.hpp"

#include <cfloat>
#include <cmath>
#include <optional>
#include <iostream>

#include <pstore/core/index_types.hpp>

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

Napi::Object database::init (Napi::Env env, Napi::Object exports) {
  Napi::Function constructor = DefineClass (env, "Database", {
    database::InstanceMethod ("index", &database::index),
    database::InstanceMethod ("id", &database::id),
    database::InstanceMethod ("path", &database::path),
    database::InstanceMethod ("revision", &database::revision),
    database::InstanceMethod ("size", &database::size),
    database::InstanceMethod ("sync", &database::sync),
  });
  exports.Set (Napi::String::New (env, "Database"), constructor);
  return exports;
}

database::database (Napi::CallbackInfo const & info)
        : ObjectWrap (info) {
  assert (info.IsConstructCall ());
  Napi::Env env = info.Env ();
  error_wrap (env, [this, &env, &info] () {
    auto const path = info[0].As<Napi::String> ().Utf8Value ();
    db_ = std::make_shared<pstore::database> (path, pstore::database::access_mode::read_only);
  });
}

Napi::Value database::size (Napi::CallbackInfo const & info) {
  Napi::Env env = info.Env ();
  if (info.Length () != 0) {
    throw Napi::TypeError::New (env, "wrong number of arguments");
  }
  return error_wrap (env, [this, &env] () { return Napi::Number::New (env, db_->size ()); });
}

Napi::Value database::id (Napi::CallbackInfo const & info) {
  Napi::Env env = info.Env ();
  if (info.Length () != 0) {
    throw Napi::TypeError::New (env, "wrong number of arguments");
  }
  return error_wrap (
    env, [this, &env] () { return Napi::String::New (env, db_->get_header ().id ().str ()); });
}

Napi::Value database::path (Napi::CallbackInfo const & info) {
  Napi::Env env = info.Env ();
  if (info.Length () != 0) {
    throw Napi::TypeError::New (env, "wrong number of arguments");
  }
  return error_wrap (env, [this, &env] () { return Napi::String::New (env, db_->path ()); });
}

Napi::Value database::revision (Napi::CallbackInfo const & info) {
  Napi::Env env = info.Env ();
  if (info.Length () != 0) {
    throw Napi::TypeError::New (env, "wrong number of arguments");
  }
  return error_wrap (
    env, [this, &env] () { return Napi::Number::New (env, db_->get_current_revision ()); });
}

Napi::Value database::sync (Napi::CallbackInfo const & info) {
  Napi::Env env = info.Env ();
  return error_wrap (env, [this, &info, &env] () {
    check_number_of_arguments (info, 1U);
    auto const v = as_int64 (env, info[0]);
    auto revision = pstore::revision_number{0U};
    using limits = std::numeric_limits<pstore::revision_number>;
    if (v == head_revision) {
      assert (head_revision == pstore::head_revision);
      revision = pstore::head_revision;
    } else if (v < limits::min () || v > limits::max ()) {
      throw Napi::TypeError::New (env, "Revision number out of range");
    } else {
      revision = static_cast<pstore::revision_number> (v);
    }
    db_->sync (revision);
    return Napi::Number::New (env, db_->get_current_revision ());
  });
}

Napi::Value database::index (Napi::CallbackInfo const & info) {
  Napi::Env env = info.Env ();
  auto const num_arguments = info.Length ();
  if (num_arguments < 1 || num_arguments > 2) {
    throw Napi::TypeError::New (env, "wrong number of arguments");
  }
  std::string const name = info[0].As<Napi::String> ().Utf8Value ();
  bool const create = num_arguments == 2 ? info[1].As<Napi::Boolean> ().Value () : true;
  //  if (name == "write") {
  return write_index::new_instance (info, db_, create);
  // else
  //   throw Napi::TypeError::New (env, "Unknown index");
}


// ====-----------------------------------------------====

class index_iterator final : public Napi::ObjectWrap<index_iterator> {
public:
  explicit index_iterator (Napi::CallbackInfo const & info)
          : ObjectWrap (info) {}
  static Napi::Value new_instance (Napi::CallbackInfo const & info,
                                   std::shared_ptr<pstore::database> const & db,
                                   std::shared_ptr<pstore::index::write_index> const & index);
  static void init (Napi::Env env);
  Napi::Value next (Napi::CallbackInfo const & info);

private:
  static Napi::FunctionReference constructor_;
  std::shared_ptr<pstore::database> db_;
  std::shared_ptr<pstore::index::write_index> index_;
  std::unique_ptr<pstore::index::write_index::iterator> it_;

  static Napi::Value key_value_pair (Napi::Env env,
                                     pstore::index::write_index::iterator const & pos);
};

Napi::FunctionReference index_iterator::constructor_;

Napi::Value
index_iterator::new_instance (Napi::CallbackInfo const & info,
                              std::shared_ptr<pstore::database> const & db,
                              std::shared_ptr<pstore::index::write_index> const & index) {

  Napi::Object obj = constructor_.New ({});
  index_iterator * const result = Unwrap (obj);
  result->db_ = db;
  result->index_ = index;
  result->it_ = std::make_unique<pstore::index::write_index::iterator> (index->begin (*db));
  return obj;
}

void index_iterator::init (Napi::Env env) {
  Napi::HandleScope scope{env};
  Napi::Function func = DefineClass (env, "IndexIterator",
                                     {
                                       InstanceMethod ("next", &index_iterator::next),
                                     });
  constructor_ = Napi::Persistent (func);
  constructor_.SuppressDestruct ();
}

Napi::Value index_iterator::key_value_pair (Napi::Env env,
                                            pstore::index::write_index::iterator const & pos) {
  auto entry = Napi::Array::New (env, std::size_t{2});
  entry.Set (uint32_t{0}, pos->first);
  entry.Set (uint32_t{1}, "value");
  return entry;
}

// Produces an "IteratorResult". That is,
//     return { value: *it, done: it==end }
Napi::Value index_iterator::next (Napi::CallbackInfo const & info) {
  auto env = info.Env ();

  Napi::Object iterator_result = Napi::Object::New (env);
  auto const done = *it_ == index_->end (*db_);
  iterator_result.Set ("done", done);
  iterator_result.Set ("value", done ? env.Null () : key_value_pair (env, (*it_)++));
  return iterator_result;
}



// ====-----------------------------------------------====

Napi::FunctionReference write_index::constructor_;

void write_index::init (Napi::Env env) {
  Napi::HandleScope scope{env};
  Napi::Function func = DefineClass (env, "WriteIndex", {
    InstanceMethod ("size", &write_index::size),
    InstanceMethod (Napi::Symbol::WellKnown(env, "iterator"), &write_index::iterator)
  });
  constructor_ = Napi::Persistent (func);
  constructor_.SuppressDestruct ();
}

Napi::Value write_index::new_instance (Napi::CallbackInfo const & info,
                                       std::shared_ptr<pstore::database> const & db, bool create) {
  auto index = pstore::index::get_index<pstore::trailer::indices::write> (*db, create);
  // The index was not loaded so return null.
  if (!index) {
    return info.Env ().Null ();
  }
  Napi::Object obj = constructor_.New ({});
  write_index * const wi = Unwrap (obj);
  wi->set (db, index);
  return obj;
}

Napi::Value write_index::size (Napi::CallbackInfo const & info) {
  Napi::Env env = info.Env ();
  if (info.Length () != 0) {
    throw Napi::TypeError::New (env, "wrong number of arguments");
  }
  return error_wrap (env, [this, &env] () { return Napi::Number::New (env, index_->size ()); });
}

// Making a JavaScript object iterable...
//
//   const myIterable = {
//     [Symbol.iterator]: function () {
//       let count = 1
//       // An object is an iterator when it implements a next() method which takes
//       // no arguments and returns an object conforming to the IteratorResult interface.
//       return {
//         next: function () {
//           const result = count++
//           // Implements the IteratorResult interface
//           return { value: result, done: result > 3 }
//         }
//       }
//     }
//   }
//   for (const value of myIterable) {
//     console.log(value)
//   }
//   console.log([...myIterable])
//
// Produces:
//
//   1
//   2
//   3
//   [ 1, 2, 3 ]
//
// That's what we need to implement here...

Napi::Value write_index::iterator (Napi::CallbackInfo const & info) {
  return index_iterator::new_instance (info, db_, index_);
}


// ====-----------------------------------------------====

Napi::Object init (Napi::Env env, Napi::Object exports) {
  exports = database::init (env, exports);
  write_index::init (env);
  index_iterator::init (env);
  return exports;
}
NODE_API_MODULE (addon, init)
