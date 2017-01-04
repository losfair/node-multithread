const coreLib = require("./build/Release/MultithreadCore");
const fs = require("fs");
const path = require("path");

const initializerCode = fs.readFileSync(path.join(__dirname, "api.js"), "utf-8");

function createThread(name, scriptPath, cb) {
    let scriptCode = fs.readFileSync(scriptPath, "utf-8");

    if(!cb) cb = () => {};

    coreLib.createThread(
        name,
        initializerCode + scriptCode,
        cb
    );
}

module.exports.createThread = createThread;
module.exports.getSharedVariableValue = coreLib.getSharedVariableValue;
module.exports.setSharedVariableValue = coreLib.setSharedVariableValue;
