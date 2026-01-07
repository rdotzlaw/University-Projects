"use strict";
let Hashable = require('./Hashable');
class IntHash extends Hashable {
    #_input;
    constructor(input) {
        super();
        this.#_input = input;
    }
    get input(){ return this.#_input; }
    hashVal() {
        return this.#_input;
    }
    equals(x) {
        if( x instanceof IntHash)
            return x.input === this.#_input;
        else
            return false;
    }
}

module.exports = IntHash;
