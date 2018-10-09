'use strict';

const common = require('../common');
if (!common.hasCrypto)
  common.skip('missing crypto');
const http2 = require('http2');

const serverTimeout = common.platformTimeout(200);
const mustNotCall = common.mustNotCall();

const server = http2.createServer();
server.timeout = serverTimeout;

server.on('request', (req, res) => res.end());
server.on('timeout', mustNotCall);

server.listen(0, common.mustCall(() => {
  const port = server.address().port;

  const url = `http://localhost:${port}`;
  const client = http2.connect(url);
  const startTime = process.hrtime();
  makeReq();

  function makeReq() {
    const request = client.request({
      ':path': '/foobar',
      ':method': 'GET',
      ':scheme': 'http',
      ':authority': `localhost:${port}`,
    });
    request.resume();
    request.end();

    request.on('end', () => {
      const diff = process.hrtime(startTime);
      const milliseconds = (diff[0] * 1e3 + diff[1] / 1e6);
      if (milliseconds < serverTimeout * 2) {
        setImmediate(makeReq);
      } else {
        server.removeListener('timeout', mustNotCall);
        server.close();
        client.close();
      }
    });
  }
}));
