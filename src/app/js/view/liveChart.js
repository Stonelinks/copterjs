var Marionette 	= require('backbone.marionette');
	Backbone 	= require('backbone'),
	d3			= require('d3');

var LiveChart = Marionette.ItemView.extend({
	template: false,

	onRender: function() {
		/*
		var valueline = d3.svg.line()
    		.x(function(d) { return x(d.date); })
    		.y(function(d) { return y(d.close); });
*/
		var n = 40, random = d3.random.normal(0, .2);
	  	var data = d3.range(n).map(random);

	  	console.log(data);
/*
		this.svg = d3.select(this.el)
						.append('svg');
		this.path = this.svg
						.append('path')
						.attr('class', 'line')
						.attr("d", valueline);*/
	}
});


module.exports = LiveChart;