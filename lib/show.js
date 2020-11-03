#!/usr/bin/env node
'use strict';

const util = require ('util');
const Database = require("./binding.js");

const d = new Database ("./db.db")
console.log (util.format ("size=%d, id=%s, path=%s, revision=%d", d.size (), d.id (), d.path (), d.revision ()));


