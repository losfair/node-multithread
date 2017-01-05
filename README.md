# multithread

`multithread` is a Node.js module for running multiple threads in one `node` process.

While the main thread has access to all Node extensions,
worker threads can only access basic V8 APIs and some methods of the `console` object.
Interactions between the main thread and worker threads are done via parameters and
return values - see `test.js` and `testWorker.js` for example. Alternatively, the
`multithread` module provides a implementation for cross-isolate shared variables,
which can also be used.

This module is in active development and I will be happy to accept new pull requests.
