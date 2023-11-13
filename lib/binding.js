import { createRequire } from 'node:module'
const require = createRequire(import.meta.url)
const pstore = require('../build/Debug/pstore-native')

export class Database {
  static head_revision = -1
  #db

  constructor(path) {
    this.#db = new pstore.Database(path)
  }
  get (...args) {
    return this.#db.get(...args)
  }
  index (name) {
    return this.#db.index(name)
  }
  id () {
    return this.#db.id()
  }
  path () {
    return this.#db.path()
  }
  revision () {
    return this.#db.revision()
  }
  size () {
    return this.#db.size()
  }
  sync (rev) {
    return this.#db.sync(rev)
  }
}
