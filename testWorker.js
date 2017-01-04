let startTime = Date.now();

let result = 0;

for(let i = 0; i < 100000; i++) result += Math.random();

let endTime = Date.now();

console.log(JSON.stringify({
    "result": result,
    "time": endTime - startTime
}));

sharedVariables.set("test", result);

console.log("Worker done");
