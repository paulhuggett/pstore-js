#!/usr/bin/env node
import { Database } from './database.js'

function show (d) {
  const index = d.index('write')
  console.log(`revision=${d.revision()}, index size=${index.size()}`)
  for (const element of index) {
    console.log(element)
    // console.log(d.get (element[1].addr, element[1].size))
    console.log('---\n' + d.get(element[1]).toString('utf8'))
  }
}

const d = new Database('./db.db')
console.log(`size=${d.size()}, id=${d.id()}, path=${d.path()}`)
show(d)

d.sync(0)
show(d)
