var Launchpad = require('./launchpad');
var mqtt = require('mqtt');
var _ = require('lodash');

useFakeData = true;

var timing = {
  sensorThrottle: 50,
  attitudeThrottle: 50,
  fakeDataInterval: 250,
  controlsLogInterval: 500,
  writerInterval: 20
};

function CopterService() {
  this.controls = {
    yaw: 127,
    pitch: 127,
    roll: 127,
    throttle: 127
  };
}

CopterService.prototype.start = function() {

  this.client = this.setupMqtt({
		'vehicle/controls': this.updateControls.bind(this)
  });

  this.startControlsLog();

	this.launchpad = new Launchpad();
	if (this.launchpad.serial && !useFakeData) {
    this.startLaunchpadListener();
    this.startLaunchpadWriter();
  }
 else {
   this.startGenerateFakeData();
	}

  console.log('started copter service');
};

CopterService.prototype.setupMqtt = function(topics) {
	console.log('Setting up mqtt');
	var client = mqtt.connect('mqtt://localhost:1883');

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
};

CopterService.prototype.updateControls = function(controls) {
  var mapControlToRange = function(input) {
    return Math.max(0, Math.min(255, Math.floor(127 + (input * 127))));
  };

  for (thing in controls) {
    this.controls[thing] = mapControlToRange(controls[thing]);
  }
};

CopterService.prototype.publish = function(data) {
  for (topic in data) {
    if (data[topic]) {
      this.client.publish(topic, JSON.stringify(data[topic]));
    }
  }
};

CopterService.prototype.publishSensors = _.throttle(function(data) {
  this.publish({
    'vehicle/sensor/gyro': data.gyro,
    'vehicle/sensor/accel': data.accel
  });
}, timing.sensorThrottle);

CopterService.prototype.publishAttitude = _.throttle(function(data) {
  this.publish({
    'vehicle/attitude': data.attitude
  });
}, timing.attitudeThrottle);

CopterService.prototype.startControlsLog = function() {
  setInterval(function() {
    console.log(this.controls);
    this.publish({
    	'vehicle/log/trace' : this.controls
    })
  }.bind(this), timing.controlsLogInterval);
};

CopterService.prototype.startLaunchpadListener = function() {
  this.launchpad.serial.on('data', function(data) {
    if (data.hasOwnProperty('gyro') && data.hasOwnProperty('accel')) {
      this.publishSensorData.call(this, data);
    }
    if (data.hasOwnProperty('attitude')) {
      this.publishAttitudeData.call(this, data);
    }
  }.bind(this));
};

CopterService.prototype.startLaunchpadWriter = function() {
  setInterval(function() {
    this.launchpad.serial.write(new Buffer([255, parseInt(this.controls.roll), parseInt(this.controls.pitch), parseInt(this.controls.yaw)]));
  }.bind(this), timing.writerInterval);
};

CopterService.prototype.startGenerateFakeData = function() {
  setInterval(function() {
    var fakeData = {
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
    };

    //console.log('sending fake data to browser');
    this.publishSensors.call(this, fakeData);
    this.publishAttitude.call(this, fakeData);
  }.bind(this), timing.fakeDataInterval);
};

module.exports = CopterService;
