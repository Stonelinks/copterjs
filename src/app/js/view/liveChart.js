var Marionette 	= require('backbone.marionette');
	Backbone 	= require('backbone'),
	smoothie	= require('smoothie');

var LiveChart = Marionette.ItemView.extend({
	template: false,
	tagName: "canvas",
	className: "liveChart",

	initialize: function() {
		this.$el.attr('width', 1200);
		this.chart = new smoothie.SmoothieChart();
		this.series = new smoothie.TimeSeries();
        this.chart.addTimeSeries(this.series, { strokeStyle: 'rgba(0, 255, 0, 1)', lineWidth: 3 });
	},

	onRender: function() {
		this.chart.streamTo(this.el, 500);
	},

	addPoint: function(value) {
		this.series.append(new Date().getTime(), value);
	}
});


module.exports = LiveChart;