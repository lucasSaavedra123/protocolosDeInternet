var socket;
var isAdminConnected = false;

var listAdviseElement = document.getElementById("listAdvise");
var loadingSignalElement = document.getElementById("loadingSignal");
var clientListElement = document.getElementById("listOfClients");

window.onbeforeunload = disconnect;

function connect(){
    socket = io();
    socket.on('connect', () => {isAdminConnected = true; finishConnectionEstablishment();});
    socket.on('disconnect', () => {isAdminConnected = false; finishDisconnection();});

    socket.on('update table', (clientsArray) => { addClientsToTable(clientsArray) });
    socket.on('new client', (client) => { addClientToTable(client); toastr.success('New client connected!'); });
    socket.on('update client status', (clientID) => { updateClientStatus(clientID); toastr.warning('Client disconnected!'); });
}

function tryConnection(){

    if(isAdminConnected){
        toastr.warning('You have already connected');
    } 
    else{
        activeLoadingState();
        connect();
    }

}

function finishConnectionEstablishment(){
    desactivateLoadingState();
    socket.emit('connection as admin');
    toastr.success('Successful Connection');
}

function disconnect(){
    socket.close();
}

function finishDisconnection(){
    desactivateLoadingState();
    document.getElementById('tbodyList').innerHTML='';
    toastr.success('Successful Disconnection');
}

function tryDisconnection(){

    if(!isAdminConnected){
        toastr.warning('You have already disconnected');
    }
    else{
        activeLoadingState();
        disconnect();
    }

}

/*Functions of Loading Behavior*/
function activeLoadingState(){
    listAdviseElement.style.display = "none";
    loadingSignalElement.style.display = "inline";
    hideClientTable();
}

function desactivateLoadingState(){
    if (isAdminConnected){
        loadingSignalElement.style.display = "none";
        listAdviseElement.style.display = "none";
        showClientTable();
    }

    else{
        loadingSignalElement.style.display = "none";
        listAdviseElement.style.display = "inline";
        hideClientTable();
    }
}

function setPendingStatus(){
    statusElement.classList.remove("connected");
    statusElement.classList.remove("disconnected");
    statusElement.classList.add("pending");
}

function setDisconnectedStatus(){
    statusElement.classList.remove("connected");
    statusElement.classList.add("disconnected");
    statusElement.classList.remove("pending");
}

function setConnectedStatus(){
    statusElement.classList.add("connected");
    statusElement.classList.remove("disconnected");
    statusElement.classList.remove("pending");
}

function showClientTable(){
    clientListElement.classList.remove("invisible");
    clientListElement.classList.add("visible");
}

function hideClientTable(){
    clientListElement.classList.add("invisible");
    clientListElement.classList.remove("visible");
}

function addClientsToTable(clientsArray){
    clientsArray.forEach(addClientToTable);
}

function addClientToTable(client){
    $('#tbodyList').append("<tr><td>"+client.id+"</td><td>"+client.time+"</td><td><span id='"+client.id+"status' class='connected'>&#11044</span></td></tr>");
}

function updateClientStatus(clientID){
    var statusElement = document.getElementById(clientID+"status");

    statusElement.classList.remove("connected");
    statusElement.classList.add("disconnected");
}
