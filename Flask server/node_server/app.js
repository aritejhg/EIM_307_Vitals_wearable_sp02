const express = require("express");
const socketio = require("socket.io");
const http = require("http");
const cors = require("cors");
const mqtt = require("mqtt");

const app = express();
const server = http.createServer(app);
const io = socketio(server);

app.use(cors());
app.set("view engine", "ejs");
app.use(express.static("public"));

const patientData = [
  ["Edward", "#10023456", "02", "07", "Stable"],
  ["Jit", "#10023463", "02", "03", "Urgent"],
];

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

mqttClient.on("message", (topic, message, packet) => {
  console.log("Got message");
  console.log(`Message: ${message}`);
  console.log(`Topic: ${topic}`);
  if (topic) {
    io.emit("update_data", []);
  }
});

// App routes
app.get("/", (req, res) => {
  res.render("index");
});

app.get("/patients", (req, res) => {
  res.render("patients");
});

app.get("/profile/?=id", (req, res) => {
  res.render("", id);
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
