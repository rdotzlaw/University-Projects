"use strict";
let HTree = require('./HTree');
let Dictionary = require('./Dictionary');
let fs = require('fs');
let StringHash = require('./StringHash');

/*
      fs.readFile('input.txt', function (err, data) {
      if (err) {
         return console.error(err);
      }
 */

// the huffman encoding algorithm
class HEncode {
    #_data;
    #_charCount;
    #_dict;
    // perform the huffman encoding
    // input is hardcoded to hamlet.txt for the official testing input

    // read the whole file
    // count all the chars, and count the occurrence of each char
    // want to measure the percentage of the file that is 'char' ( as a num between 0 and 1)
    // store this info in a dictionary, do this for all chars, including spaces and newlines
    constructor(s) {

        let fd = fs.openSync(s,'r');
        //console.log(fd + " " + s);
        this.#_data = fs.readFileSync(s, 'utf8');
        console.log("Starting Huffman Encoding on ", s);
        // #_data now contains whole file
        this.#_charCount = this.#_data.length;
        // now count occurrence of chars in data
        // and store in dictionary
        this.#_dict = new Dictionary(26);
        // go through all chars and put them into dictionary while counting the times they appear in data
        let inc = 1.0 / this.#_charCount; // a floating point num [0,1] that represents the percentage of the file that is taken up by any single char
        //console.log(inc * 6.0);
        //console.log(inc + inc + inc + inc + inc + inc);
        // ^^ maybe results in a rounding error
        // create array to store all chars in file
        let chars = [];
        for (let i = 0; i < this.#_charCount; i++) {
            let temp = this.#_data[i];

            // create StringHash of temp and see if its already in dictionary
            let sHash = new StringHash(temp);

            // the value in the dictionary is a single node huffman tree, with it's weight as a multiple of 'inc'
            // if it is then update value currently in dictionary by the current value + 1/data.length
            // searches dict[sHash.hashVal() % dict.length]'s linked list for any hashable's that are equal to sHash
            // will either be true or false
            if (!this.#_dict.isEmpty() && this.#_dict.contains(sHash)) {
                // dictionary isn't empty and sHash is in the dictionary already
                // update whatever value it has
                // gets value v from the (k,v) pair
                let x = this.#_dict.get(sHash);
                // adding 2 small decimals will sometimes create an inaccurate result
                x = x + inc;
                // weird math to prevent float inaccuracy errors,
                // dont ask how it works, it just does
                let i = x * this.#_charCount;
                i = Math.round(i);
                // create a new (k,v) pair of k = sHash, and v = updated weight
                this.#_dict.put(sHash, i*inc);

            } else {
                // if it isn't then add it to the dictionary with the value 1/data.length
                // just add sHash, either dict is empty, or isn't empty and doesnt contain sHash
                // create a new (k,v) pair of k = sHash, and v = the weight
                this.#_dict.put(sHash, inc);
                // if adding to the dictionary for the 1st time, add this char to the list of chars
                chars.push(temp);
            }
            // counts percentage of file that is any given char and stores in dict
        } // for loop end

        // now create a list of huffman trees for all the chars, using their percentage as the weight
        let trees = [];
        for (let i = 0; i < chars.length; i++) {
            trees.push(new HTree(chars[i], this.#_dict.get(new StringHash(chars[i]))));
            //console.log(trees[i].chars + " " + trees[i].weight);
        }

        // create final tree from HTrees in trees[], tree final weight should be exactly 1.0
        // find the smallest huffman tree using the compareTo method, then the second smallest, and join them
        // continue to find the next 2 smallest trees

        // sort trees using compareTo, the smallest 2 will be elements last 2 elements

        let remove = function(element, A) {
            // removes element from an array (A)
            let i = A.indexOf(element);
            A.splice(i, 1);
        }
        while(trees.length > 1){
            // loop through trees array
            let firSmall = trees[0];
            let secSmall = null;
            for(let i = 1; i < trees.length; i++){
                if(firSmall.compareTo(trees[i]) > 0){ // trees[i] is smaller than 1st smallest
                    secSmall = firSmall;
                    firSmall = trees[i];
                } else { //trees[i] is bigger
                    if(secSmall == null) {
                        secSmall = trees[i];
                    } else if(secSmall.compareTo(trees[i]) > 0){ // trees[i] is smaller than 2nd smallest
                        secSmall = trees[i];
                    }
                }
            }
            // now found the 2 smallest elements
            let t = new HTree(firSmall, secSmall);
            // remove firSmall and secSmall from array
            remove(firSmall, trees);
            remove(secSmall, trees);
            // add t to array
            trees.push(t);
        }


/*
        trees.sort(function (a, b) {
            return b.compareTo(a)
        });
        // loop until only 1 tree left, the huffman tree

        // ISSUE HERE
        while (trees.length > 1) {
            let tree = new HTree(trees[trees.length - 1], trees[trees.length - 2]);
            // remove the last 2 elements (that just got combined into a new tree
            trees.pop();
            trees.pop();
            // add the newly created tree to the array in the correct sorted position
            let i = trees.length - 1;
            let inserted = false;
            while (i >= 0) {
                // if tree comes before trees[i]
                if (tree.compareTo(trees[i]) === -1) {
                    i--;
                } else { // tree is bigger
                    // insert tree in front of trees[i]
                    trees.splice(i, 0, tree);
                    inserted = true;
                    break;
                }
            }
            if (!inserted)
                trees.splice(0, 0, tree); // throw it in at the front
        }
        // trees is now an array of size 1, with only the HTree inside it
*/

        let HT = trees[0];
        //console.log(HT.weight);

        // pre compute paths taken for each char
        // store paths in dict
        for (let i = 0; i < chars.length; i++) {
            // compute the path
            let path = "";
            path = HT.search(chars[i]);
            // store the computed path in the dictionary with (new StringHash(chars[i]), path) as the (k,v) pair
            this.#_dict.put(new StringHash(chars[i]), path); // updates value in dict to the path
        }
        // read the chars from the file again, then replace the char with 0's and 1's to represent the path taken in the HTree to reach the leaf node containing that char
        // separate the 0/1 strings with a space, then end the output string with a new line char
        let result = "";

        for (let i = 0; i < this.#_charCount; i++) {
            // for each char in data
            //console.log(this.#_dict.get(new StringHash(this.#_data[i])));
            if(i !== 0)
                result = result + " " + this.#_dict.get(new StringHash(this.#_data[i]));
            else
                result = this.#_dict.get(new StringHash(this.#_data[i]));

        }
        // finish with new line char
        result.concat('\n');

        //console.log(result);

        // then write result string to a file called output.txt
        console.log("Writing to output.txt");
        fs.writeFileSync('output.txt', result);

        console.log("Encoding finished.");
        fs.closeSync(fd);
    }
}

module.exports = HEncode;
