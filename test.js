const multithread = require("./core.js");

multithread.createThread("testThread1", "./testWorker.js", () => {
    console.log(multithread.getSharedVariableValue("output"));
});
multithread.createThread("testThread2", "./testWorker.js", () => {
    console.log(multithread.getSharedVariableValue("output"));
});
console.log("Threads created");
