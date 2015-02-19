var serialPort = require('serialport');

var LaunchpadParser = function() {
  var newLinedelimiter = '\r\n';
  var lineDelimiter = ',';
  var encoding = 'utf8';

  var parseFloatFromSerial = function(x) {
    return parseFloat(x) * 0.001;
  };

  var data = '';
  var partArray = [];
  return function(emitter, buffer) {
    data += buffer.toString(encoding);
    var parts = data.split(newLinedelimiter);
    data = parts.pop();
    parts.forEach(function(part) {
      partArray = part.split(lineDelimiter);
      emitter.emit('data', {
        raw: part,
        gyro: {
          x: parseFloatFromSerial(partArray[1]),
          y: parseFloatFromSerial(partArray[2]),
          z: parseFloatFromSerial(partArray[3])
        },
        accel: {
          x: parseFloatFromSerial(partArray[4]),
          y: parseFloatFromSerial(partArray[5]),
          z: parseFloatFromSerial(partArray[6])
        }
      });
    });
  };
};

var Launchpad = function(port) {
  port = port || '/dev/ttyACM0';
  var self = this;

  var opened = false;

  var serial = this.serial = new serialPort.SerialPort(port, {
    baudrate: 115200,
    parser: LaunchpadParser()
  });

  this.open = function(cb) {
    cb = cb || function() {};
    if (!opened) {
      serial.on('open', function() {
        opened = true;
        setTimeout(cb, 300);
      });
    }
    else {
      cb.call(self);
    }
  };
  
  this.write = function(s, cb) {
    cb = cb || function() {};

    var _write = function() {
      serial.write(s, cb);
    };

    if (!opened) {
      self.open(_write);
    }
    else {
      _write();
    }
  };

  this.samplingFrequency = 0.0

  this.close = function(cb) {
    cb = cb || function() {};
    if (opened) {
      serial.close(cb);
      opened = false;
    }
    else {
      cb.call(self);
    }
  };
};

module.exports = Launchpad;
