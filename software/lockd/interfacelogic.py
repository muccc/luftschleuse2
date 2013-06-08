import logging

class InterfaceLogic():
    def __init__(self, led_controller):
        self.led_controller = led_controller

    def tick(self):
        pass

    def update_state(self, state):
        tainted = state.is_state_tainted()

        if state.state == state.State.DOWN:
            if not tainted:
                self.led_controller.set_led('down', self.led_controller.LedState.ON)
            else:
                self.led_controller.set_led('down', self.led_controller.LedState.BLINK_SLOW)
            self.led_controller.set_led('closed', self.led_controller.LedState.OFF)
            self.led_controller.set_led('member', self.led_controller.LedState.OFF)
        if state.state == state.State.CLOSED:
            if not tainted:
                self.led_controller.set_led('closed', self.led_controller.LedState.ON)
            else:
                self.led_controller.set_led('closed', self.led_controller.LedState.BLINK_SLOW)
            self.led_controller.set_led('down', self.led_controller.LedState.OFF)
            self.led_controller.set_led('member', self.led_controller.LedState.OFF)

        if state.state == state.State.MEMBER:
            if not tainted:
                self.led_controller.set_led('member', self.led_controller.LedState.ON)
            else:
                self.led_controller.set_led('member', self.led_controller.LedState.BLINK_SLOW)
            self.led_controller.set_led('down', self.led_controller.LedState.OFF)
            self.led_controller.set_led('closed', self.led_controller.LedState.OFF)

        if state.state == state.State.PUBLIC:
            if not tainted:
                self.led_controller.set_led('closed', self.led_controller.LedState.ON)
                self.led_controller.set_led('down', self.led_controller.LedState.ON)
                self.led_controller.set_led('member', self.led_controller.LedState.ON)
            else:
                self.led_controller.set_led('closed', self.led_controller.LedState.BLINK_SLOW)
                self.led_controller.set_led('down', self.led_controller.LedState.BLINK_SLOW)
                self.led_controller.set_led('member', self.led_controller.LedState.BLINK_SLOW)

