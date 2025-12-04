import random
import json
class PseudoSensor:

    h_range = [0, 20, 20, 40, 40, 60, 60, 80, 80, 90, 70, 70, 50, 50, 30, 30, 10, 10]

    t_range = [15, 20, 25, 30, 35, 40, 45, 40, 35, 30, 25, 20, 15, 10, 5, 10]

    h_range_index = 0

    t_range_index = 0

    humVal = 0

    tempVal = 0

  

    def __init__(self):

        self.humVal = self.h_range[self.h_range_index]

        self.tempVal = self.t_range[self.t_range_index]

  

    def generate_values(self):

        self.humVal = self.h_range[self.h_range_index] + random.uniform(0, 10);

        self.tempVal = self.t_range[self.t_range_index] + random.uniform(0, 10);

        self.h_range_index += 1

        if self.h_range_index > len(self.h_range) - 1:

            self.h_range_index = 0

        self.t_range_index += 1

        if self.t_range_index > len(self.t_range) - 1:

            self.t_range_index = 0

        data = {
            "humidity": round(self.humVal, 2),
            "temperature": round(self.tempVal, 2)
        }
        value = json.dumps(data)

        return value
