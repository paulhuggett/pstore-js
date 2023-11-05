#!/usr/bin/env node
'use strict'

function show (d) {
  console.log(util.format (`size=${d.size()}, id=${d.id()}, path=${d.path()}, revision=${d.revision()}`))
}

const util = require ('util')
const Database = require("./binding.js")

const d = new Database ("./db.db")
show (d)
d.sync(0)
show (d)
