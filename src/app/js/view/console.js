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

		var gyroSlider = new ThreeDSliderView({
			model: gyroModel
		});
		gyroSlider.render();
		this.$el.find("#gyro").html(gyroSlider.$el);


		var charts = {
			x: new LiveChart(),
			y: new LiveChart(),
			z: new LiveChart()
		}

		for (var axis in charts) {
			charts[axis].render();
			this.$el.find("#strips").append(charts[axis].$el);
		}

		var client = require('mqtt').connect();
		client.subscribe('vehicle/sensor/#');
		client.on('message', function(topic, payload) {
			if (topic === "vehicle/sensor/gyro") {
				var data = JSON.parse(payload.toString());
				charts.x.addPoint(data.x);
				charts.y.addPoint(data.y);
				charts.z.addPoint(data.z);
			}
		});
	}
});

module.exports = ConsoleView;

