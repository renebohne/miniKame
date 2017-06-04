var net     = require('net');
console.log("connecting...");
var client = net.connect({ host: '192.168.4.1', port: 80 }, connected);

function connected() {
testBuff = new Buffer(['7','+','\n']);
client.write(testBuff);
    console.log("wrote to client.");
    // do other things
}