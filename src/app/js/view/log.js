var Marionette 	= require('backbone.marionette');
	Backbone 	= require('backbone');

var LogLineView = Marionette.ItemView.extend({
	tagName: 'tr',
	template: require('../../tmpl/logline.hbs'),
});

var LogView = Marionette.CollectionView.extend({
	tagName: 'table',
	className: 'log table table-striped',
	template: false,
	childView: LogLineView,
	tail: true,

	initialize: function() {
		this.collection = new Backbone.Collection();

		var client = require('mqtt').connect();
		client.subscribe('vehicle/log');
		client.on('message', function(topic, payload) {
			this.collection.add(new Backbone.Model({msg: msg}));
		}.bind(this));
	},

	onAddChild: function() {
		if (this.collection.length > 1000) {
			this.collection.shift();
		}
		if (this.tail && this.el.parentElement) {
			var elem = this.el.parentElement;
			elem.scrollTop = elem.scrollHeight;
		}
	}
});



module.exports = LogView;