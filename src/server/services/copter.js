var io = require('socket.io-client');
var Launchpad = require('./launchpad');
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
	}, 5000).bind(this)
  
  
	var updateSocket = function(data) {
		this.client.publish('vehicle/log/trace', data.raw);
		this.client.publish('vehicle/sensor/gyro', JSON.stringify(data.gyro));
		this.client.publish('vehicle/sensor/accel', JSON.stringify(data.accel));
	}.bind(this)

	var launchpad = new Launchpad()
	if (launchpad.serial) {
		launchpad.serial.on('data', function(data) {
			consoleLog(data)
			updateSocket(data)
		}.bind(this));
	} else {
		setInterval(function() {
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
