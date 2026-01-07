'use strict';
let LinkedList = require('./LinkedList');
let Dictionary = require('./Dictionary');
let IntHash = require('./IntHash');

let assert = require('assert');

function LL_isEmpty(){
    let LL = new LinkedList();
    console.assert(LL.isEmpty());
}
function DICT_isEmpty(){
    let DICT = new Dictionary(10);
    console.assert(DICT.isEmpty());
}
function DICT_contains(){
    let DICT = new Dictionary(10);
    let hash = new IntHash(5);
    let val = 10;
    // directly add [hash,val] to linked list in proper place
    DICT.A[5].add([hash, val]);
    console.assert(DICT.contains(hash));
}
function DICT_get(){
    let DICT = new Dictionary(10);
    let hash = new IntHash(5);
    let val = 10;
    // directly add [hash,val] to linked list in proper place
    DICT.A[5].add([hash, val]);
    console.assert(DICT.get(hash) === val);
}
function DICT_put(){
    let DICT = new Dictionary(10);
    let hash = new IntHash(5);
    let val = 10;
    DICT.put(hash, 10);
    console.assert(DICT.contains(hash) && DICT.get(hash) === val);
}
function main() {
    console.log("Testing Dictionary and associated classes.")
    console.log("   Testing LinkedList isEmpty");
    LL_isEmpty();
    console.log("   Testing Dictionary isEmpty");
    DICT_isEmpty();
    console.log("   Testing Dictionary contains");
    DICT_contains();
    console.log("   Testing Dictionary get");
    DICT_get();
    console.log("   Testing Dictionary put");
    DICT_put();
    console.log("All testing complete.");
}

main();
