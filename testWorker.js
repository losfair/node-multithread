console.log("Thread started");

let startTime = Date.now();

let result = 0;

for(let i = 0; i < 10000000; i++) result += Math.random();

let endTime = Date.now();

let output = JSON.stringify({
    "result": result,
    "time": endTime - startTime
});

sharedVariables.set("output", output);
