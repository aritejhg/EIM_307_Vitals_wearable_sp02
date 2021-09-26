from flask import Flask, render_template
from flask import request, escape
from flask_mqtt import Mqtt
import pandas as pd
from flask_socketio import SocketIO

app = Flask(__name__)

app.config['MQTT_BROKER_URL'] = 'broker.hivemq.com'  # use the free broker from HIVEMQ
app.config['MQTT_BROKER_PORT'] = 1883  # default port for non-tls connection
app.config['MQTT_USERNAME'] = ''  # set the username here if you need authentication for the broker
app.config['MQTT_PASSWORD'] = ''  # set the password here if the broker demands authentication
app.config['MQTT_KEEPALIVE'] = 5  # set the time interval for sending a ping to the broker to 5 seconds
app.config['MQTT_TLS_ENABLED'] = False  # set TLS to disabled for testing purposes

mqtt = Mqtt(app)
socketio = SocketIO(app)

#patients dict, contains a list of active patients and sensors attached to them. always loaded. format is {sensorid:patientid}
patients_dict = {} #enter demo data based on names on website

#each patient has a file, which contains details in registeration + status (active/not). also has historical data of sensors. loaded when clicked on patient
#create files for demo

# 1 active file for each patients sensor inputs? backed up to patient file every half/full day (not implementing this). always loaded.

#handle mqtt stuff first
@mqtt.on_connect()
def subscribe_to_topic(topics):
    topics = ["alert/#", "data/#"]
    for topic in topics:
        mqtt.subscribe(topic)

@mqtt.on_topic('alert/#')
def handle_alert(client, userdata, message):
    topic=str(message.topic)
    payload=message.payload.decode()
    sensor = topic.lstrip("alert/")
    patient = patient_dict[sensor] #refer to dict, get patient id
    patient.status = payload

@mqtt.on_topic('data/#')
def handle_mqtt_message(client, userdata, message):
    topic=str(message.topic)
    payload=message.payload.decode()
    sensor = topic.lstrip("data/")
    patient = patient_dict[sensor] #refer to dict, get patient id
    patient.df #add data to patients dataframe

@mqtt.on_log()
def handle_logging(client, userdata, level, buf):
    print(level, buf)

#next, we handle wesite stuff
@app.route('/login')
    return render_template('login.html')

@app.route('/user_registeration')
def user_registeration():
    full_name = str(escape(request.args.get("Full Name", ''))) #using dict .get method, the '' allows for empty return when page is loaded
    nric = str(escape(request.args.get("NRIC", )))
    id = str(escape(request.args.get("Patient ID", )))
    ward = str(escape(request.args.get("Ward", )))
    bed = str(escape(request.args.get("Bed", )))
    sensor_ids = str(escape(request.args.get("Sensor IDs", )))
    gender = str(escape(request.args.get("Gender", )))
    phone = int(str(escape(request.args.get("Phone Number",))))
    admission_date = str(escape(request.args.get("Admission Date",)))
    allergies = str(escape(request.args.get("Allergies", )))
    condition = str(escape(request.args.get("Medical Condition", )))
    if full_name or nric or id or ward or bed or sensor_ids or gender or phone or admission_date or allergies or condition:
        #update patient csv
        #update sensor dict
    return (render_template('user_registeration.html')
    + full_name + nric + id + ward + bed + sensor_ids + gender + phone + admission_date + allergies + condition)


@app.route('/patients')
def patients_page():
    return render_template('patients.html')





if __name__ == '__main__':
    socketio.run(app, host='0.0.0.0', port=5000, use_reloader=False, debug=True)