var io = require('socket.io-client');
var Launchpad = require('./launchpad');
var _ = require('lodash');

function CopterService() {
  
};

CopterService.prototype.start = function() {
	console.log("copter server in start");
  var launchpad = new Launchpad()

	this.socket = io.connect('http://localhost:4545');
  
  var consoleLog = _.throttle(function(data) {
    console.log(data.raw);
  }, 5000).bind(this)
  
  var updateSocket = _.throttle(function(data) {
    this.socket.emit('log', data.raw);
    this.socket.emit('gyro', data.gyro);
    this.socket.emit('accel', data.accel);
  }, 50).bind(this)

  this.socket.emit('controls', function(data) {
  	console.log(data);
  });

	launchpad.serial.on('data', function(data) {
		consoleLog(data)
		updateSocket(data)
	}.bind(this));
};

module.exports = CopterService;
