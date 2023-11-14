import { createRequire } from 'node:module'
const require = createRequire(import.meta.url)
const pstore = require('../build/Debug/pstore-native')

// A wrapper for the pstore database interface.
export class Database {
  #db

  constructor(path) {
    this.#db = new pstore.Database(path)
  }
  // get(addr,size)/get(extent)
  // Returns a buffer containing the data at the extent given by addr,size.
  get (a, ...args) {
    return this.#db.get(a, ...args)
  }
  // id()
  // Returns the database UUID.
  id () {
    return this.#db.id()
  }
  // index(name[,create])
  // Returns the named index. If the index does not yet exist and 'create' is
  // true or missing then it create; if 'create' is false, null is returned.
  index (name, ...args) {
    return this.#db.index(name, ...args)
  }
  // path()
  // Returns the path of the database store file.
  path () {
    return this.#db.path()
  }
  // revision()
  // Returns the revision to which the database is currently synched.
  revision () {
    return this.#db.revision()
  }
  // size()
  // Returns the logical size in bytes of the database store file. Note that,
  // on some platforms, this may be less than the physical size of the file.
  size () {
    return this.#db.size()
  }
  // sync(rev)
  // Switches the store to the revision number 'rev'. The special constant
  // head_revision can be used to move to the most recent revision.
  static head_revision = -1
  sync (rev) {
    return this.#db.sync(rev)
  }
}
