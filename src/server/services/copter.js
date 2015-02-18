var io = require('socket.io-client');

function CopterService() {

};

CopterService.prototype.start = function() {
	console.log("copter server in start");

	this.socket = io.connect('http://localhost:4545');

	setInterval(function() {
		this.socket.emit('log', 'some message');
		console.log('logging');
	}.bind(this), 500);
};


 module.exports = CopterService;