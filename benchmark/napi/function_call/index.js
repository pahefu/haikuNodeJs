// show the difference between calling a short js function
// relative to a comparable C++ function.
// Reports n of calls per second.
// Note that JS speed goes up, while cxx speed stays about the same.
'use strict';

const assert = require('assert');
const common = require('../../common.js');

// this fails when we try to open with a different version of node,
// which is quite common for benchmarks.  so in that case, just
// abort quietly.

try {
  var binding = require('./build/Release/binding');
} catch (er) {
  console.error('misc/function_call.js Binding failed to load');
  process.exit(0);
}
const cxx = binding.hello;

let napi_binding;
try {
  napi_binding = require('./build/Release/napi_binding');
} catch (er) {
  console.error('misc/function_call/index.js NAPI-Binding failed to load');
  process.exit(0);
}
const napi = napi_binding.hello;

var c = 0;
function js() {
  return c++;
}

assert(js() === cxx());

const bench = common.createBenchmark(main, {
  type: ['js', 'cxx', 'napi'],
  n: [1e6, 1e7, 5e7]
});

function main({ n, type }) {
  const fn = type === 'cxx' ? cxx : type === 'napi' ? napi : js;
  bench.start();
  for (var i = 0; i < n; i++) {
    fn();
  }
  bench.end(n);
}
