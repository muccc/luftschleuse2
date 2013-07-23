import random
import unittest
import mock
import doorlogic

class MockLogger:
    def __init__(self):
        pass

    def debug(self, message):
        print("Debug: %s"%message)

    def info(self, message):
        print("Info: %s"%message)

    def warning(self, message):
        print("Warning: %s"%message)

class DoorLogicTest(unittest.TestCase):

    def setUp(self):
        self.state_listener = mock.MagicMock()

        self.doorlogic = doorlogic.DoorLogic()
        self.doorlogic.policy("unittest",
                                doorlogic.DoorLogic.Origin.INTERNAL,
                                'dummy_name',
                                doorlogic.DoorLogic.Input.COMMAND,
                                'down')
        self.doorlogic.add_state_listener(self.state_listener)
        self.doorlogic.logger = MockLogger()
                                

    def test_constructor(self):
        self.assertIsInstance(self.doorlogic, doorlogic.DoorLogic)

    def test_initial_state(self):
        self.doorlogic = doorlogic.DoorLogic()
        self.assertEqual(self.doorlogic.get_state_as_string(), 'unknown')

    def test_initialization(self):
        self.assertEqual(self.doorlogic.get_state_as_string(), 'down')

    def test_state_names(self):
        pass
    
    def press_button(self, button):
        self.doorlogic.policy("unittest",
                                doorlogic.DoorLogic.Origin.CONTROL_PANNEL,
                                button,
                                doorlogic.DoorLogic.Input.BUTTON,
                                True)
    def release_button(self, button):
        self.doorlogic.policy("unittest",
                                doorlogic.DoorLogic.Origin.CONTROL_PANNEL,
                                button,
                                doorlogic.DoorLogic.Input.BUTTON,
                                False)
   
    def enter_state(self, state):
        self.press_button(state)
        self.release_button(state)

    def test_state_listener_notification(self):
        self.enter_state('member')
        self.state_listener.assert_called_once()
    
    def test_add_door(self):
        door = mock.MagicMock()
        door.name = 'Door'
        self.doorlogic.add_door(door)
        door.add_state_listener.assert_called_with(self.doorlogic.door_state_update)
        self.assertIn(door.name, self.doorlogic.doors.keys())
        self.assertIn(door, self.doorlogic.doors.values())

    def test_enter_public_state(self):
        self.press_button('public')
        self.assertEqual(self.doorlogic.get_state_as_string(), 'public')

    def test_leave_public_state(self):
        self.press_button('public')
        self.release_button('public')
        self.assertEqual(self.doorlogic.get_state_as_string(), 'member')
    
    def test_leave_public_from_door(self):
        door = mock.MagicMock()
        door.name = 'Door'
        self.doorlogic.add_door(door)

        self.press_button('public')
        self.doorlogic.policy("Door",
                                doorlogic.DoorLogic.Origin.DOOR,
                                'manual_control',
                                doorlogic.DoorLogic.Input.BUTTON,
                                True)
        self.assertEqual(self.doorlogic.get_state_as_string(), 'member')

    def test_leave_public_to_other_mode(self):
        self.press_button('public')
        self.enter_state('down')
        self.assertEqual(self.doorlogic.get_state_as_string(), 'down')

    def test_leave_public_and_release_switch(self):
        self.press_button('public')
        self.enter_state('down')
        self.release_button('public')
        self.assertEqual(self.doorlogic.get_state_as_string(), 'down')

    def test_unlock_timing(self):
        pass

    def test_bell_code_timing(self):
        pass
if __name__ == '__main__':
    unittest.main()
