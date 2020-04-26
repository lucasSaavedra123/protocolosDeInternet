var socket;
var isClientConnected = false;

var timeElement = document.getElementById("connectedTime");
var idElement = document.getElementById("userID");
var statusElement = document.getElementById("connectionStatus");

var connectButton = document.getElementById("connectButton");
var disconnectButton = document.getElementById("disconnectButton");

window.onbeforeunload = disconnect;

function connect(){
    socket = io();
    socket.on('connect', () => {isClientConnected = true; finishConnectionEstablishment();});
    socket.on('disconnect', () => {isClientConnected = false; finishDisconnection();});

    socket.on('time update', (time) => { timeElement.innerHTML = time });
}

function tryConnection(){
    if(isClientConnected){
        toastr.warning('You have already connected');
    } 
    else{
        activeLoadingState();
        connect();
    }
}

function finishConnectionEstablishment(){
    desactivateLoadingState();
    socket.emit('connection as client');
    toastr.success('Successful Connection');
}

function disconnect(){
    socket.emit('disconnection as client');
    socket.close();
}

function finishDisconnection(){
    desactivateLoadingState();
    toastr.success('Successful Disconnection');
}

function tryDisconnection(){

    if(!isClientConnected){
        toastr.warning('You have already disconnected');
    }
    else{
        activeLoadingState();
        disconnect();
    }

}

/*Functions of Loading Behavior*/
function activeLoadingState(){
    idElement.innerHTML = "<span class='spinner-border spinner-border-sm'></span>";
    setPendingStatus();
}

function desactivateLoadingState(){
    if (isClientConnected){
        idElement.innerHTML = socket.id;
        setConnectedStatus();
    }
    else{
        idElement.innerHTML = '-';
        timeElement.innerHTML = '--:--:--';
        setDisconnectedStatus();
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