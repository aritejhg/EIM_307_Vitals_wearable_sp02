from flask import flask
from flask import request, escape
from flask_mqtt import mqtt

app = flask(__name__)

app.config['MQTT_BROKER_URL'] = 'broker.hivemq.com'  # use the free broker from HIVEMQ
app.config['MQTT_BROKER_PORT'] = 1883  # default port for non-tls connection
app.config['MQTT_USERNAME'] = ''  # set the username here if you need authentication for the broker
app.config['MQTT_PASSWORD'] = ''  # set the password here if the broker demands authentication
app.config['MQTT_KEEPALIVE'] = 5  # set the time interval for sending a ping to the broker to 5 seconds
app.config['MQTT_TLS_ENABLED'] = False  # set TLS to disabled for testing purposes

topics = ["alert/#", "data/#"]

@mqtt.on_connect()
def subscribe_to_topic(topics):
    topics = ["alert/#", "data/#"]
    for topic in topics:
        mqtt.subscribe(topic)


@mqtt.on_message()
def handle_mqtt_message(client, userdata, message):
    topic=str(message.topic)
    payload=message.payload.decode()
    if topic == "alert":
        alertmessage(client)
    elif topic == "data"

def


mqtt = mqtt()

@app.route('/user-registeration')
def user_registeration():
    full_name = str(escape(request.args.get("Full Name", ''))) #using dict .get method, the '' allows for empty return when page is loaded
    nric = str(escape(request.args.get("Full Name", )))
    id = str(escape(request.args.get("Full Name", )))
    ward = str(escape(request.args.get("Full Name", )))
    bed = str(escape(request.args.get("Full Name", )))
    sensor_ids = str(escape(request.args.get("Full Name", )))
    gender = str(escape(request.args.get("Full Name", )))
    phone = int(str(escape(request.args.get("Full Name",))))
    admission_date = str(escape(request.args.get("Full Name",)))
    allergies = str(escape(request.args.get("Full Name", )))
    condition = str(escape(request.args.get("Full Name", )))

    return (render_homepage('user-registeration.html')
    + full_name + nric + id + ward + bed + sensor_ids + gender + phone + admission_date + allergies + condition)



@app.route('/index.html')


@app.route('/patients/<patient_name>')