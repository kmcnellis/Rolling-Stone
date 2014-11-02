from pymouse import PyMouse
from pykeyboard import PyKeyboard

import time


class Mouse(object):
    """
    Wrapper to handle the computer interface.
    """

    def __init__(self):
        super(Mouse, self).__init__()
        self.m = PyMouse()
        self.x, self.y = self.m.position()

    def move(self, end_x, end_y):
        '''
        Takes in values that you want to offset
        '''
        end_x += self.x
        end_y += self.y

        x_diff = (end_x - self.x)
        y_diff = (end_y - self.y)

        steps = max(x_diff,y_diff)

        if steps != 0:
            x_step = float(x_diff) / steps
            y_step = float(y_diff) / steps

        step = 0
        while step < steps:
            self.x += x_step
            self.y += y_step
            self.m.move(self.x,self.y)
            time.sleep(self.time_delay)
            step += 1

    def click(self):
        self.m.click(self.x,self.y,1)

    def right_click(self):
        self.m.click(self.x,self.y,2)

    x = 0
    y = 0
    time_delay = .0005

class Keyboard(object):
    def __init__(self):
        super(Keyboard, self).__init__()
        self.k = PyKeyboard()

    def write(self,text):
        self.k.type_string(text)