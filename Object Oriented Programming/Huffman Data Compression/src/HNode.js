"use strict";
class HNode {
    #_left;
    #_right;
    #_val;
    constructor(c) {
        this.#_val = c;
        this.#_left = null;
        this.#_right = null;
    }
    set right(r){ this.#_right = r; }
    get right() { return this.#_right; }
    set left(l){ this.#_left = l; }
    get left() { return this.#_left; }
    get val(){ return this.#_val; }

    /*
    search(c) {
        // base case
        if(this.#_val === c) {
            return "Y";
        } else {
            // check both subtrees for c

            let temp = this.#_right.search(c);
            if (temp !== "X") {
                // if the recursive calls find c
                return temp + "0";
            }
            temp = this.#_left.search(c);
            if (temp !== "X") {
                return temp + "1";
            }

            // if reach this point, left and right subtrees don't contain c
            return "X";
        }
    }*/

    search(c, A){
        // c is the value to look for in a leaf node
        // A is the array containing the 0's and 1's representing the path taken
        // A is an output variable passed in from the top of the recursion
        // if a leaf node is reached and it doesn't contain c, then the last value of A is popped off

        // check if leaf node
        if(this.#_left === null && this.#_right === null){
            // is leaf node
            return this.#_val === c;
        } else {
            // this is not a leaf node, we need to check both right and left nodes if they exist
            let res = false;
            // if left node exists, check it
            if(this.#_left !== null){
                A.push(0);
                res = this.#_left.search(c, A);
            }
            // if val hasn't been found
            if(!res){
                // remove the 0 from A
                A.pop();
                // now check right node if it exists
                if(this.#_right !== null){
                    A.push(1);
                    res = this.#_right.search(c, A);
                }
            }
            if(!res){
                // haven't found val in right either, remove 1 from A
                A.pop();
                return false;
            } else {
                return true;
            }
        }
    }

}
module.exports = HNode;