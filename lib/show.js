#!/usr/bin/env node
'use strict'

function show (d) {
  console.log(util.format (`size=${d.size()}, id=${d.id()}, path=${d.path()}, revision=${d.revision()}`))
}

const util = require ('util')
const Database = require("./binding.js")

const d = new Database ("./db.db")
show (d)

const size = d.index("write").size()
console.log(`index size=${size}`)

d.sync(0)
show (d)
const size0 = d.index("write").size()
console.log(`index size=${size0}`)
