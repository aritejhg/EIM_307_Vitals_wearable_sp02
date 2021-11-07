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

// Data model
const patientData = [
  ["Edward", "#10023456", "02", "07", "94", "76", "150/90", "Stable"],
  ["Jit", "#10023463", "02", "03", "99", "66", "140/80", "Urgent"],
];

const dataMap = new Map();
dataMap.set(0, { HR: [], SP: [], BP: [] });
dataMap.set(1, { HR: [(ts, reading)], SP: [], BP: [] });

const sensorMap = new Map();
sensorMap.set(1, 0);
sensorMap.set(3, 1);
sensorMap.set(2, 0);
sensorMap.set(4, 1);

// MQTT Client
const mqttUrl = "";
const mqttClient = mqtt.connect(mqttUrl, {
  clientId: "abcdef",
  clean: true,
  connectTimeout: 4000,
  username: "",
  password: "",
  reconnectPeriod: 1000,
});

const topics = ["wearatals/alert/#", "wearatals/data/#"];
mqttClient.on("connect", () => {
  console.log("MQTT Client connected");

  mqttClient.subscribe(topics, (top) => {
    console.log(`Subscribed to ${top}`);
  });
});

function updateDataArray(dataArray, newData) {
  console.log(dataArray);
  const newSplit = newData.split(",");
  const latestNew = Date.parse(parseInt(newSplit[newSplit.length - 2]));
  const diff30min = 30 * 60 * 1000;

  // Remove data out of range
  while (dataArray.length > 0 && latestNew - dataArray[0][0] > diff30min) {
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
  return newSplit[newSplit.length - 1];
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
  return newSplit[newSplit.length - 1];
}

mqttClient.on("message", (topic, message, packet) => {
  // wearatals/data/<sensorId>/<vitalSign: Heartrate, SpO2, BP>
  const split = topic.split("/");
  if (split[1] === "data") {
    const patientInd = sensorMap.get(arseInt(split[2]));
    if (split[3] === "Heartrate") {
      patientData[patientInd][4] = updateDataArray(
        dataMap.get(patientInd).HR,
        message
      );
    } else if (split[3] === "SpO2") {
      patientData[patientInd][5] = updateDataArray(
        dataMap.get(patientInd).SP,
        message
      );
    } else if (split[3] === "BP") {
      patientData[patientInd][6] = updateBpArray(
        dataMap.get(patientInd).BP,
        message
      );
    } else {
      console.log("Unknown vital: " + split[3]);
    }

    io.emit("update_data", patientInd, patientInd[patientInd]);
  } else if (split[1] === "alert") {
    console.log("ALERT!");
  } else {
    console.log("Unknown topic: " + topic);
    return;
  }
});

// App routes
app.get("/", (req, res) => {
  res.render("index");
});

app.get("/patients", (req, res) => {
  res.render("patients");
});

app.get("/register", (req, res) => {
  res.render("user_registration");
});

app.get("/profile/", (req, res) => {
  const patientInd = req.query.id;
  res.render("profile", { ind: patientInd });
});

// Websockets handlers
io.on("connection", (socket) => {
  console.log("Client connected");

  socket.on("fetch_data", (display_data) => {
    display_data(patientData);
  });

  socket.on("disconnect", () => {
    console.log("Client disconnected");
  });
});

server.listen(3000, () => {
  console.log("Server running on 3000");
});
