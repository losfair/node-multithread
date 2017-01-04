const multithread = require("./core.js");

multithread.createThread("testThread1", "./testWorker.js");
multithread.createThread("testThread2", "./testWorker.js");

setTimeout(() => {
    console.log(multithread.getSharedVariableValue("test"));
    process.exit(0);
}, 1000);
