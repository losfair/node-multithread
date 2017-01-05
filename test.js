"use strict";

//console.log(Date.now());

const multithread = require("./core.js");

let doneCount = 0;
const totalCount = 100;

let startTime = Date.now();

console.log(startTime);

for(let i = 0; i < totalCount; i++) {
    try {
        multithread.createThread("testThread" + i, "./testWorker.js", {
            "testValue": i
        }, (msg, status) => {
            console.log("[" + status + "]" + "[" + msg + "] " + multithread.getSharedVariableValue("output"));
            doneCount++;
            if(doneCount == totalCount) {
                let endTime = Date.now();
                console.log(endTime - startTime);
            }
        });
    } catch(e) {
        console.log("Exception: " + e.toString());
    }
}

console.log("Threads created");
