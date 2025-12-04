from psuedoSensor import PseudoSensor
import paho.mqtt.client as paho
from paho import mqtt
import time


def on_connect(client, userdata, flags, rc, properties=None):
    print("CONNACK received with code %s." % rc)


# with this callback you can see if your publish was successful
def on_publish(client, userdata, mid, properties=None):
    print("mid: " + str(mid))


# print which topic was subscribed to
def on_subscribe(client, userdata, mid, granted_qos, properties=None):
    print("Subscribed: " + str(mid) + " " + str(granted_qos))


# print message, useful for checking if it was successful
def on_message(client, userdata, msg):
    print(msg.topic + " " + str(msg.qos) + " " + str(msg.payload))


USERNAME = "SaherHassaballa"
PASSWORD = "Saher12345678"
CLOUD_URL = "3091b71eb24541b4a5593fa28711f867.s1.eu.hivemq.cloud"
PORT = 8883

client = paho.Client(client_id="", userdata=None, protocol=paho.MQTTv5)
client.on_connect = on_connect

# enable TLS for secure connection
client.tls_set(tls_version=mqtt.client.ssl.PROTOCOL_TLS)
# set username and password
client.username_pw_set(USERNAME, PASSWORD)
# connect to HiveMQ Cloud on port 8883 (default for MQTT)
client.connect(CLOUD_URL, PORT)

# setting callbacks, use separate functions like above for better visibility
client.on_subscribe = on_subscribe
client.on_message = on_message
client.on_publish = on_publish

# subscribe to all topics of encyclopedia by using the wildcard "#"
client.subscribe("pi/#", qos=1)
# loop_forever for simplicity, here you need to stop the loop manually
# you can also use loop_start and loop_stop
# client.loop_forever()
client.loop_start()
ps = PseudoSensor()

while True:

    value = ps.generate_values()

    # a single publish, this can also be done in loops, etc.
    client.publish("pi/sensor", payload=value, qos=1)
    time.sleep(10)  # Sleep for 3 seconds
