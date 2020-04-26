var portNumber = 4000
var express = require('express')
var socketIO = require('socket.io')
var socketSrv;
var listOfClients = [];
var application = express();
var server = application.listen(portNumber, function(){ console.log('Server is listening!!!') });
var client;

application.use(express.static('public'));

socketSrv = socketIO(server);

socketSrv.on('connection', (socket) => {

    socket.on('connection as client', () => {
        var connectionTime = new Date();

        connectionTime = connectionTime.getHours() + ":" + connectionTime.getMinutes() + ":" + connectionTime.getSeconds();

        client = {id:socket.id , time: connectionTime}
        listOfClients.push(client);

        socket.emit('time update', connectionTime);

        socket.broadcast.emit('new client', client);

        console.log("User Connected...");
    } );

    socket.on('connection as admin', () => {
        socket.emit('update table', listOfClients);
        console.log("Admin Connected...");
    });

    socket.on('disconnection as client', () =>{
        var clientID = socket.id;
        deleteClientFromList(clientID);
        socket.broadcast.emit('update client status', clientID);
        console.log("User Disconnected...");
    });

});

function deleteClientFromList(clientID){

    var index = 0

    while (index < listOfClients.length) { 

        if( listOfClients[index].id == clientID ){
            listOfClients.splice(index, 1);
        }

        index++;
    }

}