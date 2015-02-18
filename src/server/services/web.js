var express = require('express'),
	path	= require('path');

function WebService() {
	this.app = express();
};

WebService.prototype.start = function() {
	this.app.use(express.static(path.resolve(__dirname, '../../../build')));
	this.server = this.app.listen(4545, function() {
		console.log("Webserver started");
	});
};

 module.exports = WebService;