"use strict"
let HNode = require('./HNode');
class HTree {
    #_weight; // floating point between 0 and 1
    #_root; // root node
    #_chars;

    constructor(c, w) {
        if(c instanceof HTree && w instanceof HTree){
            // greater weight on the left
            this.#_weight = c.weight + w.weight;
            // merge and sort char arrays from both
            this.#_chars = [].concat(c.chars, w.chars);
            this.#_chars.sort();
            this.#_root = new HNode('');
            if( c.weight > w.weight) {
                this.#_root.left = c.root;
                this.#_root.right = w.root;
            } else {
                this.#_root.left = w.root;
                this.#_root.right = c.root;
            }
        } else {
            // create a leaf node
            this.#_weight = w;
            this.#_root = new HNode(c);
            this.#_chars = [c];
        }
    }
    get chars() { return this.#_chars; }
    get root() { return this.#_root; }
    get weight() { return this.#_weight; }
    set weight(i) { this.#_weight = this.#_weight + i;}

    compareTo(t){
        // compare the other HTree (t) to this
        // return 1, 0 , -1 depending on if t comes before(1) or after(-1) this
        // for any 2 trees the tree with the lowest weight comes first
        // if 2 trees contain the same weight, the tree that contains (in a leaf node), the smallest character comes first

        // For example, if two trees had the same weight,
        // and if one tree contained the characters “C”,”O”,”M”,”P”
        // and the other contained “E”,”N”,”G”,”L”, then the first tree would come first (it contains “C”).

        // there shouldn't be a case where they both contain the same smallest char, but in case you can return 0

        if(this.#_weight < t.weight){
            return -1;
        } else if(this.#_weight > t.weight){
            return 1;
        } else { // have the same weight, check smallest char in both
            if(this.#_chars[0] < t.chars[0]){ // t comes after this
                return -1;
            } else if(this.#_chars[0] > t.chars[0]) { // t comes before this
                return 1;
            } else { // somehow have the same char in both
                console.log("Compared two trees that contain the same chars, this shouldn't happen.")
                return 0;
            }
        }
    }

    search(c){
        // recursively search for the char c in the huffman tree
        // return the left/right moves required to reach the leaf node containing c from the root node
        // returns a string of 1's and 0's, 0 is left, 1 is right
        let A = [];
        this.#_root.search(c, A);
        // return A as a string of 1's and 0's
        return A.join('');
    }
}

module.exports = HTree;