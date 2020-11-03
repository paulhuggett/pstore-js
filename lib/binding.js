'use strict';

const addon = require('../build/Release/object-wrap-demo-native');

function Database (path) {
    const instance = new addon.Database (path);
    this.size = () => instance.size ();
    this.id = () => instance.id ();
    this.path = () => instance.path ();
    this.revision = () => instance.revision ();
}

module.exports = Database;
