"use strict";
let LinkedList = require('./LinkedList');

class Dictionary {
    #_A;
    #_len;
    // an array: A of size n, containing Linked Lists
    // a linked list contains nodes, which contain Hashable objects and a value
    // each Hashable object is added to the linked list at A[Hashable.hashVal() % n]
    constructor(n) {
        this.#_len = n;
        this.#_A = new Array(n);
        // fill array with linked lists
        for(let i = 0; i < n; i++){
            this.#_A[i] = new LinkedList();
        }
    }
    get A(){ return this.#_A; }

    put(k, v){
        // k is a hashable object
        // if a node with key k already exists, update it's value from [k, v1] to [k, v]
        this.#_A[k.hashVal() % this.#_len].add([k, v]);
    }
    get(k){
        // should return the v associated with the [k,v] pair in a node at the linked list in pos A[k.hashVal() % len]
        return this.#_A[k.hashVal() % this.#_len].get(k);
    }
    contains(k){
        // returns true if the hashable k is in the dictionary
        // check the linked list at A[k.hashVal() % len] for k
        return this.#_A[k.hashVal() % this.#_len].contains(k);
    }
    isEmpty(){
        // loop through all linked lists, if all linked lists are empty, then the dictionary is empty
        let i = 0;
        // if reach end of list
        while(i < this.#_len){
            // if a single linked list isn't empty, return false
            if( !this.#_A[i].isEmpty() ){
                return false;
            }
            i++;
        }
        // return true if reach end of list
        return true;
    }
}

module.exports = Dictionary;