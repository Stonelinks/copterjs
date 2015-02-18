var Marionette 	= require('backbone.marionette');
var	LogView		= require('./log');

var ConsoleView	= Marionette.ItemView.extend({
	template: require('../../tmpl/console.hbs'),

	onRender: function() {
		this.logView = new LogView();
		this.logView.render();
		this.$el.find('#log').html(this.logView.$el);
	}
});

module.exports = ConsoleView;

