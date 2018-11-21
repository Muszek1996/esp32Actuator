var rainbowEnable = false;
var connection = new WebSocket('ws://' + location.hostname + '/ws', ['arduino']);
connection.onopen = function () {
    var a = {'type': 'Connection'};
    connection.send(JSON.stringify(a));
    console.log("Connected to WebSocketServer");
};
connection.onerror = function (error) {
    console.log('WebSocket Error ', error);
    document.getElementById("avgTemp").textContent = "ERROR"
    document.getElementById("avgTemp").style = "display:inline; font-size: 100px; color:PURPLE";

};
connection.onclose = function () {
    console.log('WebSocket connection closed');
    document.getElementById("avgTemp").textContent = "ERROR"
    document.getElementById("avgTemp").style = "display:inline; font-size: 100px; color:PURPLE";
};

var blocked = false;

connection.onmessage = function (e) {

    if(e.data!=null){
        try {
            var data = JSON.parse(e.data);
        }catch(f){
            console.log(e.data);
            return;
        }
        if(data.type == "TempSensorReadings"){
            console.log("TempSensorReadings Received");
            console.log(data);
            if(data.data[0])
                document.getElementById('tempParagraph0').textContent = data.data[0];
            if(data.data[1])
                document.getElementById('tempParagraph1').textContent = data.data[1];
                document.getElementById('avgTemp').textContent = ((data.data[1]+data.data[0])/2);
                if(!blocked){
                    document.getElementById('temperatureText').textContent = data.tempSlider;
                    document.getElementById('tempSlider').value = data.tempSlider;
                }

            document.getElementById("speedSlider").value = data.speedSlider;

        }else{
            console.log('Server: ', e.data);
        }
    }


};


function sendTemp () {
    changeTemp();
    var temp = document.getElementById('tempSlider').value;
    document.getElementById('temperatureText').textContent = temp.toString();
    var obj = { 'type': "tempOnSlider", 'data': temp };
    var str = JSON.stringify(obj);
    console.log(str);
    connection.send(str);
    blocked = false;
}
function sendSpeed () {
    var temp = document.getElementById('speedSlider').value;
    var obj = { 'type': "speedOnSlider", 'data': temp };
    var str = JSON.stringify(obj);
    console.log(str);
    connection.send(str);
}

function openWindow(){
    var obj = { 'type': "windowAction", 'data': 1 };
    var str = JSON.stringify(obj);
    console.log(str);
    connection.send(str);
}


function closeWindow(){
    var obj = { 'type': "windowAction", 'data': 0 };
    var str = JSON.stringify(obj);
    console.log(str);
    connection.send(str);
}
function stopWindow(){
    var obj = { 'type': "windowAction", 'data': 7 };
    var str = JSON.stringify(obj);
    console.log(str);
    connection.send(str);
}

function changeTemp(){
    blocked = true;
    var temp = document.getElementById('tempSlider').value;
    document.getElementById('temperatureText').textContent = temp.toString();
}