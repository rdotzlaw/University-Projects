"use strict";
let Node = require('./Node');
class LinkedList {
    #_head;
    #_tail;
    #_length;
    constructor() {
        this.#_length = 0;
    }

    get(k){
        // takes a hashable k and returns the associated v in the tuple contained in a node, or returns undefined
        // do it recursively
        if( this.#_length > 0){
            return this.#_head.get(k);
        } else {
            return undefined;
        }
    }

    contains(k){
        // recursively search the linked list for a node containing a tuple that has k in pos 0
        if( this.#_length > 0){
            return this.#_head.contains(k);
        }
        else
            return false;
    }

    add(v){
        let N = new Node(v);
        // empty list
        if( this.#_length === 0){
            this.#_head = N;
            this.#_tail = N;
        } else { // non empty list
            if(this.contains(v[0])){
                // start from head, find Node that has node.val[0].equals(v[0])
                let curr = this.#_head;
                while(curr != null && !curr.value[0].equals(v[0])){
                    curr = curr.next;
                }
                // now have node to update
                curr.value[1] = v[1];
            } else {
                // add as the next node of the tail node
                this.#_tail.next = N;
                // update tail node
                this.#_tail = N;
            }
        }
        // increase linkedList len
        this.#_length++;
    }
    isEmpty(){
        return this.#_length === 0;
    }
}
module.exports = LinkedList;