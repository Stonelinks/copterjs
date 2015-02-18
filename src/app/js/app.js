var $			= require('jquery');
var Backbone	= require('backbone');
var io			= require('socket.io-client');
Backbone.$ = $;
var Marionette 	= require('backbone.marionette');


var app = new Marionette.Application();
window.app = app;

app.addRegions({
	content: '#content'
});

var Router = Marionette.AppRouter.extend({

	routes: {
		'console' : 'showConsole'
	},

	showConsole: function() {
		var ConsoleView = require('./view/console');
		app.getRegion('content').show(new ConsoleView());
	}
});

// open the socket
app.addInitializer(function(opts) {
	console.log("opening socket");
	this.socket = io();
}.bind(app));

// start the router
app.addInitializer(function(opts) {
	this.router = new Router();
	Backbone.history.start({pushState: true});
});

app.start();
