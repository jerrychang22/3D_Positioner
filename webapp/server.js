const express = require('express');
const app = express();
const port = 3000;
const path = require('path');


app.use('/public', express.static(path.join(__dirname, "public")));
app.get('/', (req, res) => res.sendFile('./index.html', {root: __dirname}));
app.listen(port, () => console.log("Connected at port ${port}"));


//Websocket handling
const WebSocket = require('ws');
const wss = new WebSocket.Server({ port: 2222 });
//CLIENTS = [];

wss.on('connection', (ws) => {
    console.log("A user has been connected");
    //console.log("All users currently connected : ");
    console.log(wss.clients);

    ws.on('message', (message) => {
        //console.log('received: %s', message);
        wss.clients.forEach(function e(client){
            if (client != ws) client.send(message);
        });
    });

    ws.send('something');
});


console.log("Server established");

