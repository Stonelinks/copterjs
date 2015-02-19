var Marionette 	= require('backbone.marionette');
	Backbone 	= require('backbone');

var JoystickView = Marionette.ItemView.extend({
	template: false,
	tagName: "canvas",
	radius: 20,

	events: {
		"mousedown" :  "startControl",
		"mouseup" :  "stopControl",
		"mousemove" : "onControl",
		"touchstart" :  "startControl",
		"touchend" :  "stopControl",
		"touchmove" : "onControl"
	},

	modelEvents: {
		"change" : "draw"
	},

	onRender: function() {
//		this.measure();

		this.context = this.el.getContext('2d');

		this.dimensions = {
			width: 460,
			height: 230
		};
		this.center = {
			x: this.dimensions.width/2,
			y: this.dimensions.height/2
		};
		this.$el.width(this.dimensions.width);
		this.$el.height(this.dimensions.height);

		this.draw();
	},

	draw: function() {
		this.context.clearRect(0,0,this.dimensions.width, this.dimensions.height);
		this.context.beginPath();
		this.context.arc(this.center.x, this.center.y, this.radius, 0, 2* Math.PI, false);
		this.context.strokeStyle = "green";
		this.context.stroke();

		if (this.controlling) {	
			this.context.beginPath();
			this.context.arc(
				(this.model.get('x') + 1)*this.center.x, 
				(this.model.get('y') + 1)*this.center.y, 
				10, 0, 2* Math.PI, false);
			this.context.fillStyle = "green";
			this.context.fill();
		}
		return true;
	},

	measure: function() {
		this.dimensions = {
			width: this.el.width,
			height: this.el.height
		};
		this.center = {
			x: this.dimensions.width/2,
			y: this.dimensions.height/2
		};
	},

	startControl: function(e) {
		e.preventDefault();
		this.measure();
		var coords = this.getCoords(e);

		if (Math.abs(coords.x - this.center.x) < this.radius && Math.abs(coords.y - this.center.y) < this.radius) {
				this.controlling = true;
				this.model.set({
					x: (e.offsetX/this.dimensions.width - 0.5) * 2,
					y: (e.offsetY/this.dimensions.height - 0.5) * 2
				});
				this.draw();
		}
	},

	getCoords: function(e) {
		if (e.originalEvent instanceof TouchEvent) {
			var local = {
				x: this.$el.offset().left,
				y: this.$el.offset().top
			}
			return {
				x: e.originalEvent.targetTouches[0].pageX - local.x,
				y: e.originalEvent.targetTouches[0].pageY - local.y
			};
		} else if (e.originalEvent instanceof MouseEvent) {
			return {
				x: e.offsetX,
				y: e.offsetY
			}
		}
	},

	stopControl: function(e) {
		this.controlling = false;
		this.model.set({
			x: 0,
			y: 0
		})
	},

	onControl: function(e) {
		//this.measure();
		if (this.controlling) {
			var coords = this.getCoords(e);
			this.model.set({
				x: (coords.x/this.dimensions.width - 0.5) * 2,
				y: (coords.y/this.dimensions.height - 0.5) * 2
			});
		}
	}
});


module.exports = JoystickView;