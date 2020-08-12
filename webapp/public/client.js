
var sock = new WebSocket("ws://localhost:2222");
var data = document.getElementById('data');

sock.onopen = function(event){
    alert("Connected!");
    console.log("Connected to websocket server");
};

sock.onmessage = function(event){
    data.innerHTML = event.data;
};
