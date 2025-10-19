const express = require('express')
const app = express()

const PORT = 3000

app.get('/',function(req,res){
    res.sendFile(__dirname+"/html/index.html");
});

const server = app.listen(PORT,function(){
    console.log("Server is runinng at port "+PORT);
});

const SocketServer = require('ws').Server;
const wss = new SocketServer({ server });

let button_status = "off";
let range_value = 0;

wss.on('connection', function(ws){
	console.log('Client connected');
	
	ws.send(button_status);
    ws.send(range_value);

	ws.on('message', function(msg){
        type = msg.toString();
        if (type === "off" || "on"){
		button_status = type
		console.log(button_status);
		broadcast(button_status);
        }else{
            range_value = type;
            console.log(range_value);
        };
	});
	
	ws.on('close', function(){
		console.log('Client disconnected');
	});
});

function broadcast(msg) {
	wss.clients.forEach(function (client) {
		if (client.readyState === client.OPEN) { // if client still connected
			client.send(msg);
		}else{
			console.log("client off");
		}
	});
}