var express = require('express'),
	path	= require('path')
	_ 		= require('lodash');

function WebService() {
	// create the express app
	this.app = express();

	// stash off where our static root is
	this.root = path.resolve(__dirname, '../../../build');

	// setup socketio
	this.server = require('http').Server(this.app);
	this.io = require('socket.io')(this.server);
	this.io.on('connection', this.onConnection.bind(this));

	this.messages = require('../messages.json');
};

WebService.prototype.start = function() {
	// serve our static content
	this.app.use(express.static(this.root));

	// route all request to the index.html so Marionette can handle routing
	this.app.use('*', function(req, res, next) {
		res.sendFile(this.root + "/index.html");
	}.bind(this));

	this.server.listen(4545, function() {
		console.log("Webserver started");
	}.bind(this));
};

WebService.prototype.onConnection = function(socket) {
	_.each(this.messages, function(name) {
		socket.on(name, function(msg) {
			socket.broadcast.emit(name, msg);
		});
	})
};

 module.exports = WebService;