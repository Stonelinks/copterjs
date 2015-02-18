var Marionette 	= require('backbone.marionette');
var	LogView		= require('./log');
var ThreeDSliderView = require('./3dSlider');

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
			max: Math.PI,
			min: -Math.PI
		});

		var gyroSlider = new ThreeDSliderView({
			model: gyroModel
		});
		gyroSlider.render();
		this.$el.find("#gyro").html(gyroSlider.$el);

		window.app.socket.on('gyro', function(data) {
			gyroModel.set('value', data);
		})
	}
});

module.exports = ConsoleView;

