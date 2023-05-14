'use strict';

const addon = require ('../build/Debug/pstore-native');

function Database (path) {
    const instance = new addon.Database (path);
    this.head_revision = -1;

    this.size = () => instance.size ();
    this.id = () => instance.id ();
    this.path = () => instance.path ();
    this.revision = () => instance.revision ();
    this.sync = (rev) => instance.sync(rev);
}

module.exports = Database;
