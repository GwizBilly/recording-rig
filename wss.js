const WebSocket = require('ws');
const faker = require('faker');
const {spawn} = require('child_process');
const wss = new WebSocket.Server({ port: 8080 });
const fs = require('fs');
const path = require('path');

var copyFile = (file, dir) => {
	var f = path.basename(file);
	var s = fs.createReadStream(file);
	var d = fs.createWriteStream(path.resolve(dir, f));
	s.pipe(d);
	s.on('end', function() { console.log("copied file");});
	s.on('error', function(err) { console.log(err);});
}

var moveFile = (file, dir)=>{
	var f = path.basename(file);
	//var d = path.resolve(dir, f);
	var ff = dir;
	fs.rename(f, ff, (err)=>{
		if(err) throw err;
		else console.log('moved file');
	});
};

var msg = 'welcome';
var counter = 0;

console.log("waiting for websocket connection (Port:8080)");

wss.on('connection', function connection(ws) {
	console.log("connection event!");
	ws.on('message', function incoming(message) {
		msg = message + " return to sender #: " + counter;
		counter++;
		console.log('received: %s', message);
		var dataToSend;
		// spawn new child process to call the python script
		const python = spawn('python3', ['script1.py']);
		// collect data from script
		python.stdout.on('data', function (data) {
			console.log('Pipe data from python script ...');
			dataToSend = data.toString();
		});
		// in close event we are sure that stream from child process is closed
		python.on('close', (code) => {
			console.log(`child process close all stdio with code ${code}`);
			console.log(dataToSend);
			ws.send('d');
			console.log('done');
			var fname = faker.lorem.word();1
			var rando = faker.name.firstName();
			var wav = ".wav";
			var o = "./" + fname +"_"+ rando + wav;
			console.log(o);
			// rename file
			moveFile('./g.wav', o);
			// copy & move to places
			copyFile(o, '/home/pi/mnt/gdrive/Gwiz\'s music');
			moveFile(o,'/home/pi/music');
		});
	})
	console.log("now sending welcome to ws");
	ws.send(msg);
});
