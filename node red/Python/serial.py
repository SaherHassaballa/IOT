import json
import paho.mqtt.client as mqtt

class PotentiometerSensor:
    pot_value = 0

    def __init__(self):
        self.pot_value = 0

    def generate_json(self, value):
        """ترجع البيانات في شكل JSON"""
        self.pot_value = value
        data = {
            "potentiometer": round(self.pot_value, 2)
        }
        return json.dumps(data)

# HiveMQ Cloud details
broker = "3091b71eb24541b4a5593fa28711f867.s1.eu.hivemq.cloud"
port = 8883
topic = "esp32/potentiometer"

username = "YOUR_HIVEMQ_USERNAME"
password = "YOUR_HIVEMQ_PASSWORD"

sensor = PotentiometerSensor()

# Callback لما تستقبل رسالة
def on_message(client, userdata, msg):
    value = int(msg.payload.decode())
    json_data = sensor.generate_json(value)
    print(json_data)

# إنشاء العميل
client = mqtt.Client("PythonSubscriber")
client.username_pw_set(username, password)
client.tls_set()  # ضروري لـ TLS
client.on_message = on_message

# الاتصال بالـ broker
client.connect(broker, port)

# الاشتراك في الموضوع
client.subscribe(topic)

# تشغيل اللوب
client.loop_forever()
