const clientId = "Nrrl8BbCwS";
    const mqttBrokerURL = "wss://mqtt-dashboard.com:8884/mqtt";
    const client = new Paho.MQTT.Client(mqttBrokerURL, clientId);

    // Throttle, Yaw, Pitch, Roll
    var controlSignal = {
        throttle: 0,
        yaw: 0,
        pitch: 0,
        roll: 0,
        active: 0,
    };

    client.onConnectionLost = onConnectionLost;
    client.onMessageArrived = onMessageArrived;

    document.onkeydown = function (event) { 
      if (event.code === "Space") {
        controlSignal.active = 1;
      }
      if (event.key === "c"){
        controlSignal.throttle = 0;
      }
      if(controlSignal.active == 1){
        switch (event.keyCode) {
          case 37: // Left arrow key pressed
            controlSignal.yaw-=50;
            break;
          case 38: // Up arrow key pressed
            controlSignal.throttle+=10;
            break;
          case 39: // Right arrow key pressed
            controlSignal.yaw+=50;
            break;
          case 40: // Down arrow key pressed
            controlSignal.throttle-=10;
            break;
          case 87: // W forward arrow key pressed
            if(controlSignal.pitch === 100)
              break;
            else{
              controlSignal.pitch+=50;
              break;
            }
          case 83: // S Backword key pressed
          if(controlSignal.pitch === -100)
            break;
          else{
            controlSignal.pitch-=50;
            break;
          }
          case 68: // D Right move key pressed
          if(controlSignal.roll === 100)
            break;
          else{
            controlSignal.roll+=50;
            break;
          }
          case 65: // A Left move key pressed
          if(controlSignal.roll === -100)
            break;
          else{
            controlSignal.roll-=50;
            break;
          }
            break;
        }
        sendControlSignal();
        //console.log(event.keyCode)
      }
    };

    function sendControlSignal() {
        client.send("Smart_Flight_Drone_2000", JSON.stringify(controlSignal), 0, false);
    }

    function onConnectionLost(responseObject) {
        if (responseObject.errorCode !== 0) {
        console.log("Connection lost:", responseObject.errorMessage);
        }
    }

    // handle data from mqtt broker
    function onMessageArrived(message) {
        console.log(message._getPayloadString());
    }

    // Connect to MQTT broker
    const connectOptions = {
        onSuccess: onConnect,
        useSSL: false,
    };

    client.connect(connectOptions);

    function onConnect() {
        console.log("Connected to MQTT broker");
        client.subscribe("Smart_Flight_Drone_2000");
        publishLightMessage("Turn on");
    }

    // Example: Publish a message to the "light" topic
    function publishLightMessage(message) {
        if (client.isConnected()) {
        client.send("Smart_Flight_Drone_2000", message);
        }
    }