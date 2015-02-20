var Marionette 	= require('backbone.marionette');
	Backbone 	= require('backbone'),
	smoothie	= require('smoothie');

var LiveChart = Marionette.ItemView.extend({
	template: false,
	tagName: "canvas",
	className: "liveChart",

	initialize: function(options) {
    console.log(options)
    if (options.min && options.max) {
      this.chart = new smoothie.SmoothieChart({
        maxValue: options.max,
        minValue: options.min
      });
    }
    else {
      this.chart = new smoothie.SmoothieChart();
    }
		
		this.series = new smoothie.TimeSeries();
        this.chart.addTimeSeries(this.series, { strokeStyle: 'rgba(0, 255, 0, 1)', fillStyle: 'rgba(0, 255, 0, 0.2)', lineWidth: 4 });
	},

	onRender: function() {
		this.chart.streamTo(this.el, 500);
	},

	addPoint: function(value) {
		this.series.append(new Date().getTime(), value);
	}
});


module.exports = LiveChart;
