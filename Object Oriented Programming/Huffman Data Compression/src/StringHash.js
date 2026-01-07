"use strict";
let Hashable = require('./Hashable');
class StringHash extends Hashable {
    #_input;
    #_p;
    constructor(input) {
        super();
        this.#_input = input;
        /*
        if( input instanceof String){
            this.#_input = input;
        } else if('toString' in input){
            this.#_input = input.toString();
        } else {
            this.#_input = "";
        }*/
        this.#_p = 7; // prime number of choice
    }
    get input() { return this.#_input; }

    // input is a string
    // hash val = s[0]*p^(n-1)^ + s[1]*p^(n-2)^ + ...+ s[n-3]*p^(2)^ + s[n-2]*p + s[n-1]
    // @returns {number}
    hashVal() {

        let result = 0;

        for(let x = 0; x < this.#_input.length; x++){
            result += this.#_input.charCodeAt(x) * (this.#_p ^ (this.#_input.length - x - 1));
            //         s[x] (in ascii)         *    p    ^     n-1 - x
        }
        return result;
    }
    equals(x) {
        if(x instanceof StringHash)
            return this.#_input === x.input;
        else
            return false;
    }
}

module.exports = StringHash;