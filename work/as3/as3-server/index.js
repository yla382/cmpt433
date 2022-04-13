const express = require('express');
const bodyParser = require('body-parser');
const app = express();
const http = require('http')
const server = http.createServer(app);
const { Server } = require("socket.io");
const io = new Server(server);
var dgram = require('dgram');

// on connection to node server
io.on('connection', (socket) => {
    console.log('connected');

	// on request received from another server
    socket.on('REQUEST', (data) => {
        console.log(data);

        var PORT = 12345;
		var HOST = '192.168.7.2';
		var buffer = new Buffer(data);
		
		// send dgram to bbg
		var client = dgram.createSocket('udp4');
		client.send(buffer, 0, buffer.length, PORT, HOST, function(err, bytes) {
			if (err) 
				throw err;
			console.log('UDP message sent to ' + HOST +':'+ PORT);
		});

		client.on('listening', function () {
			var address = client.address();
		});

		// Handle an incoming message over the UDP from the local application.
		client.on('message', function (message, remote) {
			var reply = message.toString('utf8')
			socket.emit('commandReply', reply);

			client.close();

		});
    });
});

var indexRouter = require('./routes/indexRoute');

app.use('/', indexRouter);

server.listen(8088, function () {
    console.log('Example app listening on port 8088!');
});
