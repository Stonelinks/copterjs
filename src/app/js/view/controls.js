var Marionette 	= require('backbone.marionette');
	Backbone 	= require('backbone'),
	JoystickView	= require('./joystick');

var ControlsView = Marionette.ItemView.extend({
	template: require('../../tmpl/controls.hbs'),

	onRender: function() {
		var joystickModelLeft = new Backbone.Model({x: 0, y: 0});
		var joystickModelRight = new Backbone.Model({x: 0, y: 0});

		var joystickLeft = new JoystickView({
			el: this.$el.find('#joystick-left'),
			model: joystickModelLeft
		}).render();
		var joystickRight = new JoystickView({
			el: this.$el.find('#joystick-right'),
			model: joystickModelRight
		}).render();

		var sendControls = function() {
			window.app.socket.emit('controls', {
				yaw: joystickModelLeft.get('x'),
				throttle: joystickModelLeft.get('y'),
				pitch: joystickModelRight.get('y'),
				roll: joystickModelRight.get('x'),
			});
		}

		joystickModelLeft.on("change", sendControls);
		joystickModelRight.on("change", sendControls);

	}
});

module.exports = ControlsView;