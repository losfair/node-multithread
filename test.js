const multithread = require("./core.js");

multithread.createThread("testThread1", "./testWorker.js", (msg, status) => {
    console.log("[" + status + "]" + "[" + msg + "] " + multithread.getSharedVariableValue("output"));
});
multithread.createThread("testThread2", "./testWorker.js", (msg, status) => {
    console.log("[" + status + "]" + "[" + msg + "] " + multithread.getSharedVariableValue("output"));
});

console.log("Threads created");
