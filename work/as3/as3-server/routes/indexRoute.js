var router = require('express').Router();
var path = require('path');

router.get('/', function(req, res) {
	res.sendFile(path.join(__dirname + "/../views/index.html"));
});

router.get('/index.css', function(req, res) {
	res.sendFile(path.join(__dirname + "/../views/index.css"));
});

router.get('/index.js', function(req, res) {
	res.sendFile(path.join(__dirname + "/../views/index.js"));
});

router.get('/beatbox_ui.js', function(req, res) {
	res.sendFile(path.join(__dirname + "/../views/beatbox_ui.js"));
});

// router.get('/sock.js', function(req, res) {
// 	res.sendFile(path.join(__dirname + "/../views/sock.js"));
// });

module.exports = router;