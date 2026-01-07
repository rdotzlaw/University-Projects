"use strict";
class Hashable { // abstract class
    constructor() {
        if(this.constructor === Hashable)
            console.error(new Error("Abstract class, use subclasses"));
    }

    hashVal(){
        if(this.constructor === Hashable)
            console.error(new Error("Abstract method, use subclasses"));
    }
    equals(x){
        if(this.constructor === Hashable)
            console.error(new Error("Abstract method, use subclasses"));
    }
}

module.exports = Hashable;