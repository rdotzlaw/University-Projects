"use strict";
let HTree = require('./HTree');
let assert = require('assert');
let fs = require('fs');
function HTree_c1() {
    // test basic constructor, constructs a single leaf node in the tree
    let ht = new HTree('c', 0.1);
    console.assert(ht.weight === 0.1);
    console.assert(ht.root.val === 'c');
    console.assert(ht.root.left === null);
    console.assert(ht.root.right === null);

}
function HTree_c2() {
    let ht1 = new HTree('b', 0.1);
    let ht2 = new HTree('c', 0.9);
    let ht3 = new HTree(ht1, ht2);
    console.assert(ht3.weight === 1.0);
    console.assert(ht3.root.val === '');
    console.assert(ht3.root.left.val === 'c');
    console.assert(ht3.root.right.val === 'b');
}
function HTree_search(){
    let ht1 = new HTree('b', 0.1);
    let ht2 = new HTree('c', 0.9);
    let ht3 = new HTree(ht1, ht2);
    assert(ht3.search('b') === "1");
    assert(ht3.search('c') === "0");
}
function HTree_compareTo(){
    let ht1 = new HTree('b', 0.1);
    let ht2 = new HTree('c', 0.9);
    console.assert(ht1.compareTo(ht2) === -1);
    console.assert(ht2.compareTo(ht1) === 1);
}

function main(){

    console.log("Testing HTree and associated classes.");
    console.log("   Testing HTree constructor 1");
    HTree_c1();
    console.log("   Testing HTree constructor 2");
    HTree_c2();
    console.log("   Testing HTree search");
    HTree_search();
    console.log("   Testing HTree compareTo");
    HTree_compareTo();
    console.log("All testing complete.");
}

main();