const express = require("express");
const socketio = require("socket.io");
const http = require("http");
const cors = require("cors");
const mqtt = require("mqtt");

// Server setup
const app = express();
const server = http.createServer(app);
const io = socketio(server);

app.use(cors());
app.set("view engine", "ejs");
app.use(express.static("public"));

// Data model for current values and status
// [name, patientID, ward no, bed no, (ts, hr), (ts, sp ), (ts, bp), isUrgent]
const patientData = [
  ["Edward", "#10023456", "02", "07", [0, "324"], [1, "76"], [2, "150/90"], false, "50/120", 95, "120/80"],
  ["Jit", "#10023463", "02", "03", [4, "99"], [5, "66"], [6, "140/80"], true, "50/120", 95, "120/80"],
];

const dataMap = new Map();
//dataMap.set(patientID, { HR: [(ts, reading)], SP: [(ts, reading)], BP: [(ts, reading)] });
dataMap.set(0, {HR: [[ 1636211312702, '98' ], [ 1636211412702, '108' ], [ 1636211512702, '130' ]], SP: [], BP: []});


const sensorPatientMap = new Map();
//.set(sensorID, patientID);
sensorPatientMap.set(1, 0);
sensorPatientMap.set(3, 1);
sensorPatientMap.set(2, 0);
sensorPatientMap.set(4, 1);


const patientSensorMap = new Map();
//patientSensorMap.set(patientID, [(HR,id),[SpO2,id],(BP,id)]);


// MQTT Client
const mqttUrl = "mqtt://broker.mqttdashboard.com:1883";
const mqttClient = mqtt.connect(mqttUrl, {
  clientId: "mqtt_sdfasd",
  clean: true,
  connectTimeout: 4000,
  username: "",
  password: "",
  reconnectPeriod: 1000,
});

const topics = ["wearatals/alert/#", "wearatals/data/#"];
mqttClient.on("connect", () => {
  console.log("MQTT Client connected");

  mqttClient.subscribe(topics[0], () => {
    console.log(`Subscribed to ${topics[0]}`);
  });

  mqttClient.subscribe(topics[1], () => {
    console.log(`Subscribed to ${topics[1]}`);
  });
});

function updateDataArray(dataArray, newData) {
  console.log(dataArray);
  console.log(newData);
  const newSplit = newData.split(",");
  const latestNew = Date.parse(newSplit[newSplit.length - 2]);
  const diff30min = 30 * 60 * 1000;

  // Remove data out of range
  console.log(latestNew)
  console.log(latestNew - dataArray[0][0])
  console.log(diff30min)
  while (dataArray.length > 0 && ((latestNew - dataArray[0][0]) > diff30min)) {
    dataArray.shift();
  }

  // Append new data to store
  let ts = 0;
  let reading = 0;
  for (var i = 0; i < newSplit.length; i = i + 2) {
    ts = Date.parse(newSplit[i]);
    reading = newSplit[i + 1];
    dataArray.push([ts, reading]);
  }
  console.log(dataArray);
  return [ts, newSplit[newSplit.length - 1]];
}

function updateBpArray(dataArray, newData) {
  console.log(dataArray);
  const newSplit = newData.split(",");

  let ts = 0;
  let reading = 0;
  for (var i = 0; i < newSplit.length; i = i + 2) {
    ts = Date.parse(newSplit[i]);
    reading = newSplit[i + 1];
    dataArray.push([ts, reading]);
  }

  while (dataArray.length > 30) {
    dataArray.shift();
  }
  console.log(dataArray);
  return [ts, newSplit[newSplit.length - 1]];
}

mqttClient.on("message", (topic, message, packet) => {
  // topic: wearatals/data/<sensorId>/<vitalSign: Heartrate, SpO2, BP>
  const split = topic.split("/");
  const patientID = sensorPatientMap.get(parseInt(split[2]));

  //Data handler based on vital sign type
  if (split[1] === "data") {
    if (split[3] === "Heartrate") {
      patientData[patientID][4] = updateDataArray(
        dataMap.get(patientID).HR,
        message.toString()
      );
    } else if (split[3] === "SpO2") {
      patientData[patientID][5] = updateDataArray(
        dataMap.get(patientID).SP,
        message.toString()
      );
    } else if (split[3] === "BP") {
      patientData[patientID][6] = updateBpArray(
        dataMap.get(patientID).BP,
        message.toString()
      );
    } else {
      console.log("Unknown vital: " + split[3]);
    }
    //emit patient Data column for that specific patient
    io.emit("update_data", patientID, patientData[patientID]);
  } else if (split[1] === "alert") {
  //   console.log("ALERT!" + patientID);
  //   patientData[patientID][7] = "Urgent";
  //   if (split[3] === "Heartrate") {
  //     patientData[patientID][4] = updateDataArray(
  //       dataMap.get(patientID).HR,
  //       message
  //     );
  //   } else if (split[3] === "SpO2") {
  //     patientData[patientID][5] = updateDataArray(
  //       dataMap.get(patientID).SP,
  //       message
  //     );
  //   } else if (split[3] === "BP") {
  //     patientData[patientID][6] = updateBpArray(
  //       dataMap.get(patientID).BP,
  //       message
  //     );
  //   io.emit("update_data", patientID, patientData[patientID]);
  // } else {
  //   console.log("Unknown topic: " + topic);
  //   return;
  }
});

// App routes
app.get("/", (req, res) => {
  res.render("index");
});

app.get("/patients", (req, res) => {
  console.log("absd")
  res.render("patients");
});

app.get("/register", (req, res) => {
  res.render("user_registration");
});

app.get("/profile/", (req, res) => {
  const patientID = req.query.id;
  res.render("profile", { ind: patientID });
});

// Websockets handlers
io.on("connection", (socket) => {
  console.log("Client connected");

  socket.on("fetch_data", display_data => {
    display_data(patientData);
  })

  socket.on("fetch_profile", (patientID, fetcher) => {
    var ind = parseInt(patientID);
    console.log("profile")
    console.log(ind);
    console.log(dataMap.get(ind));
    fetcher(patientData[ind], dataMap.get(ind));
  }) ;

  socket.on("disconnect", () => {
    console.log("Client disconnected");
  });
});

server.listen(3000, () => {
  console.log("Server running on 3000");
});
