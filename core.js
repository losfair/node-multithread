const coreLib = require("./build/Release/MultithreadCore");
const fs = require("fs");
const path = require("path");

const initializerCode = fs.readFileSync(path.join(__dirname, "api.js"), "utf-8");

function createThread(name, scriptPath) {
    let scriptCode = fs.readFileSync(scriptPath, "utf-8");
    coreLib.createThread(
        name,
        initializerCode + scriptCode
    );
}

module.exports.createThread = createThread;
module.exports.getSharedVariableValue = coreLib.getSharedVariableValue;
module.exports.setSharedVariableValue = coreLib.setSharedVariableValue;
