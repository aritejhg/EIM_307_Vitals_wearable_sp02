import json
from flask import Flask
from flask_mqtt import Mqtt

app = Flask(__name__)
#app.config['SECRET'] = 'my secret key'
#app.config['TEMPLATES_AUTO_RELOAD'] = True
app.config['MQTT_BROKER_URL'] = 'broker.hivemq.com'
app.config['MQTT_BROKER_PORT'] = 1883
app.config['MQTT_USERNAME'] = ''
app.config['MQTT_PASSWORD'] = ''
app.config['MQTT_KEEPALIVE'] = 5
app.config['MQTT_TLS_ENABLED'] = False

mqtt = Mqtt(app)

topics = ['/swa/Heartrate', '/swa/SpO2']
@mqtt.on_connect()
def handle_connect(client, userdata, flags, rc):
    for topic in topics:
        mqtt.subscribe(topic)

@app.route('/')
def index():
    return render_template('vitals.html')


@mqtt.on_message()
def handle_mqtt_message(client, userdata, message):
    data = dict(
        topic=message.topic,
        payload=message.payload.decode()
    )
    print(data)


@mqtt.on_log()
def handle_logging(client, userdata, level, buf):
    print(level, buf)


if __name__ == '__main__':
    app.run(debug=False)