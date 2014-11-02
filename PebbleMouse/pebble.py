import serial
import json
import time

import controller

MOUSE = 0
KEYBOARD = 1
ROBOT = 2


class Pebble(object):

    def __init__(self, port_name="ttyACM1"):
        super(Pebble, self).__init__()
        self.port = port_name
        self.ser = serial.Serial('/dev/' + port_name, 9600, timeout=1)
        time.sleep(1)

    def _get_port(self):
        pass

    def _read(self):
        """
        Read in files from serial. Returns Json data
        Expects JSON of the Following form:
        {
            'action': INTEGER [0,2]
            'x': NUMBER,
            'y': NUMBER,
            'z': NUMBER,
            'select': BOOL,
            'mod': BOOL
        }
        """

        self.ser.flushInput()
        time.sleep(.05)

        data_buffer = ''

        while True:
            if '{' in data_buffer and '}' in data_buffer:
                print "Success! "
                break
            else:
                data_buffer += self.ser.readline().strip()
                # print data_buffer
                time.sleep(.06)

        start = data_buffer.find('{')
        end = data_buffer.find('}', start+1)
        data = data_buffer[start:end+1]
        # print data
        return json.loads(data)

    def _write(self):
        """
        Write to serial
        """
        pass

    def update(self):
        """
        Reads in serial and then updates variables accordingly.
        """
        json_data = self._read()

        print json_data

        next_x = json_data['x']
        print next_x
        if abs(self.x - next_x) > self.threshold:
            self.x = next_x

        next_y = json_data['y']
        print next_y
        if abs(self.y - next_y) > self.threshold:
            self.y = next_y

        next_z = json_data['z']
        print next_z
        if abs(self.z - next_z) > self.threshold:
            self.z = next_z

        self.select = json_data['select']
        self.mod = json_data['mod']

    def is_mouse(self):
        return self.action == MOUSE

    def is_keyboard(self):
        return self.action == KEYBOARD

    def is_robot(self):
        return self.action == ROBOT

    def clicked_select(self):
        if self.select:
            self.select = False
            return True
        else:
            return False

    def clicked_mod(self):
        if self.mod:
            self.mod = False
            return True
        else:
            return False

    def close(self):
        self.ser.close()

    x = 0
    y = 0
    z = 0

    threshold = 0

    # Android Variables
    robot_move = False
    mouse = False
    keyboard = False

    # Button States
    select = False
    mod = False

    #Function of the current input
    action = MOUSE

if __name__ == "__main__":
    peb = Pebble("ttyACM1")
    m = controller.Mouse()
    k = controller.Keyboard()

    while True:
        peb.update()
        print peb.x, peb.y
        m.move(peb.x,peb.y)
        time.sleep(.01)

    peb.close()