"use strict";

const coreLib = require("./build/Release/MultithreadCore");
const fs = require("fs");
const path = require("path");

const initializerCode = fs.readFileSync(path.join(__dirname, "api.js"), "utf-8");

const MAX_THREAD_COUNT = coreLib.getMaxThreadCount();
let currentThreadCount = 0;

let workerCodeCache = {};

let threadWaitQueue = [];

function createWaitingThreads() {
    while(currentThreadCount < MAX_THREAD_COUNT && threadWaitQueue.length) {
        let item = threadWaitQueue.shift();
        createThread(item.name, item.scriptPath, item.args, item.cb);
    }
}

function createThread(name, scriptPath, args, cb) {
    if(currentThreadCount >= MAX_THREAD_COUNT) {
        threadWaitQueue.push({
            "name": name,
            "scriptPath": scriptPath,
            "args": args,
            "cb": cb
        });
        return;
    }

    let scriptCode;

    if(workerCodeCache[scriptPath]) scriptCode = workerCodeCache[scriptPath];
    else {
        scriptCode = fs.readFileSync(scriptPath, "utf-8");
        workerCodeCache[scriptPath] = scriptCode;
    }

    let scriptArgs = {};

    let code = initializerCode;

    if(args) {
        if(typeof(args) == "object" || typeof(args) == "array") {
            scriptArgs = JSON.stringify(args);
            code += "var Arguments = " + scriptArgs + ";";
        } else {
            if(!cb) cb = args;
        }
    }

    if(!cb) cb = () => {};

    code += scriptCode;

    coreLib.createThread(
        name,
        code,
        (msg, status) => {
            if(status == 1)  { // Done
                currentThreadCount--;
                createWaitingThreads();
            }
            cb(msg, status);
        }
    );

    currentThreadCount++;
}

module.exports.createThread = createThread;
module.exports.getSharedVariableValue = coreLib.getSharedVariableValue;
module.exports.setSharedVariableValue = coreLib.setSharedVariableValue;
