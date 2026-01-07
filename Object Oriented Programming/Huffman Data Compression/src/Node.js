"use strict";
class Node {
    #_next;
    #_value;
    constructor(v) {
        // value is a tuple of ( HASHABLE obj, val)
        this.#_value = v;
        this.#_next = null;
    }

    contains(k){
        if( this.#_value[0].equals(k)){
            return true;
        } else if( this.#_next != null ){
            return this.#_next.contains(k);
        } else {
            return false;
        }
    }

    get(k){
        if( this.#_value[0].equals(k)){
            return this.#_value[1];
        } else if( this.#_next != null ) {
            return this.#_next.get(k);
        } else {
            return undefined;
        }
    }

    set next(n) { this.#_next = n; }
    get next() { return this.#_next; }
    get value() { return this.#_value; }
    set value(v) { this.#_value = v; }
}
module.exports = Node;