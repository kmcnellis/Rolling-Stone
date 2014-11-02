import time

import pebble
import controller


def demo_move_mouse():
    # current_x, current_y = 200, 200
    m = controller.Mouse()
    m.move(40.5, 400)
    m.click()
    # m.move(-300.7,-300)
    m.right_click()
    time.sleep(1)
    m.move(-10, -10)
    m.click()


def demo_pebble():
    pass


def main(peb, mouse, keyboard):
    while True:
        peb.update()

        if peb.is_mouse():
            mouse.move(peb.x,peb.y)
            if peb.clicked_select():
                mouse.click()
            if peb.clicked_mod():
                mouse.right_click()
        elif peb.is_keyboard():
            pass
        else:
            pass
        time.sleep(.001)

if __name__ == "__main__":
    # mpeb = pebble.Pebble('ttyACM1')
    demo_move_mouse()
    # x_dim, y_dim = m.screen_size()
    # m.click(x_dim/2, y_dim/2, 1)
    # k.type_string('Hello, World!')