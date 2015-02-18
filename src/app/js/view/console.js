var Marionette = require('backbone.marionette');

var ConsoleView	= Marionette.ItemView.extend({
	template: false,

	render: function() {
		this.$el.html('console');
	}


});

module.exports = ConsoleView;

