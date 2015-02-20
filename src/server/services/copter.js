var io = require('socket.io-client');
var Launchpad = require('./launchpad');
var HoverController = require('./hovercontroller');
var mqtt    = require('mqtt');
var _ = require('lodash');

function CopterService() {
  
};

CopterService.prototype.start = function() {
	this.client = this.setupMqtt({
		'vehicle/controls' : function(controls) {
			console.log(controls)
		}
	});
	//setTimeout(this.setupMqtt.bind(this), 1000);
	console.log("copter server in start");

	var consoleLog = _.throttle(function(data) {
		console.log(data.raw);
	}, 1000).bind(this)

	var socketLog = _.throttle(function(data) {
		this.client.publish('vehicle/log/trace', data.raw);
	}, 1000).bind(this)
  
	var updateSocket = _.throttle(function(data) {
	    if (data.gyro) {
	      this.client.publish('vehicle/sensor/gyro', JSON.stringify(data.gyro));
	    }
	    if (data.accel) {
	      this.client.publish('vehicle/sensor/accel', JSON.stringify(data.accel));
	    }
	    if (data.attitude) {
	      this.client.publish('vehicle/attitude', JSON.stringify(data.attitude));
	    }
	}, 30).bind(this)

	var launchpad = new Launchpad();
    var hovercontroller = new HoverController(launchpad);
	if (launchpad.serial) {
		launchpad.serial.on('data', function(data) {
			consoleLog(data)
			socketLog(data)
			updateSocket(data)
		}.bind(this));
	} else {
		setInterval(function() {
			attitude.y += Math.PI/360;
			updateSocket({
				raw: 'raw data',
				gyro: {
					x: Math.random(),
					y: Math.random(),
					z: Math.random()
				},
				accel: {
					x: Math.random(),
					y: Math.random(),
					z: Math.random()
				},
				attitude: {
					roll: Math.random(),
					pitch: Math.random()
				}
			})
		}, 250);
	}
};

CopterService.prototype.setupMqtt = function(topics) {
	console.log('Setting up mqtt');
	var client  = mqtt.connect('mqtt://localhost:1883');

	for (var topic in topics) {
		client.subscribe(topic);
	}
	client.publish('vehicle/sensor/accel', {x: Math.random(), y: Math.random(), z: Math.random()});

  	client.on('message', function(topic, payload) {
  		if (topics[topic]) {
  			var data;
  			try {
  				data = JSON.parse(payload.toString());
  			} catch (e) {
  				data = payload.toString();
  			}
  			topics[topic].call(this, data);
  		}
	}.bind(this));

	return client;
}

module.exports = CopterService;
