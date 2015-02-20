var Marionette 	= require('backbone.marionette');
var	LogView		= require('./log');
var ThreeDSliderView = require('./3dSlider');
var LiveChart 	= require('./liveChart');

var ConsoleView	= Marionette.ItemView.extend({
	template: require('../../tmpl/console.hbs'),

	onRender: function() {
		this.logView = new LogView();
		this.logView.render();
		this.$el.find('#log').html(this.logView.$el);

		var gyroSlider = new ThreeDSliderView({
			model: gyroModel
		});
		gyroSlider.render();
		this.$el.find("#gyro").html(gyroSlider.$el);

		// gyrocharts
		var gyroModel = new Backbone.Model({
			value: {
				x: 0,
				y: 0,
				z: 0
			},
			step: 0.001,
			max: Math.PI,
			min: -Math.PI
		});

		var gyroCharts = {
			x: new LiveChart(),
			y: new LiveChart(),
			z: new LiveChart()
		}
		for (var axis in gyroCharts) {
			gyroCharts[axis].render();
			this.$el.find("#gyro-strips").append(gyroCharts[axis].$el);
		}

		// accel charts
		var accelCharts = {
			x: new LiveChart(),
			y: new LiveChart(),
			z: new LiveChart()
		}
		for (var axis in accelCharts) {
			accelCharts[axis].render();
			this.$el.find("#accel-strips").append(accelCharts[axis].$el);
		}

		// attitude charts
		var attitudeCharts = {
			roll: new LiveChart(),
			pitch: new LiveChart()
		}
		for (var axis in attitudeCharts) {
			attitudeCharts[axis].render();
			this.$el.find("#attitude-strips").append(attitudeCharts[axis].$el);
		}

		var client = require('mqtt').connect();
		client.subscribe('vehicle/sensor/+');
		client.subscribe('vehicle/attitude');
		client.on('message', function(topic, payload) {
			if (topic === "vehicle/sensor/gyro") {
				var data = JSON.parse(payload.toString());
				gyroCharts.x.addPoint(data.x);
				gyroCharts.y.addPoint(data.y);
				gyroCharts.z.addPoint(data.z);
			}
			if (topic === "vehicle/sensor/accel") {
				var data = JSON.parse(payload.toString());
				accelCharts.x.addPoint(data.x);
				accelCharts.y.addPoint(data.y);
				accelCharts.z.addPoint(data.z);
			}
			if (topic === "vehicle/attitude") {
				var data = JSON.parse(payload.toString());
				attitudeCharts.roll.addPoint(data.roll);
				attitudeCharts.pitch.addPoint(data.pitch);
			}

		});
	}

});

module.exports = ConsoleView;

