# not currently maintained

This repo will combine Arduino code for SparkFun PPG sensor with featherOLED to be used on the Huzzah32. Dev code is contained in huzzah32_featherOLED_SFsensor.
webserver_sparkfun contains code for the server.

Created for IDP Project EIM-307 Wearable Vital Signs Monitor

Node Server v2 is used to establish communication using HiveMQ MQTT broker. Node.js and chart.js are used for the front-end.
We use display_mqtt_datafilter to filter data on ESP32 device and communicate to the MQTT server.

