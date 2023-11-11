#!/usr/bin/env node
'use strict'

const util = require ('util')
const Database = require("./binding.js")

const d = new Database ("./db.db")
console.log(`size=${d.size()}, id=${d.id()}, path=${d.path()}`)
console.log(`revision=${d.revision()}, index size=${d.index("write").size()}`)
d.sync(0)
console.log(`revision=${d.revision()}, index size=${d.index("write").size()}`)
