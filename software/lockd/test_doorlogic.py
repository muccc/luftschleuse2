#    This file is part of lockd, the daemon of the luftschleuse2 project.
#
#    See https://github.com/muccc/luftschleuse2 for more information.
#
#    Copyright (C) 2013 Tobias Schneider <schneider@muc.ccc.de> 
#
#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.
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
        #self.doorlogic.logger = MockLogger()
                                

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

    def test_bell_code_opens_door(self):
        door = mock.MagicMock()
        door.name = 'Door'
        self.doorlogic.add_door(door)

        self.press_button('member')

        self.doorlogic.temp_unlock = mock.MagicMock()
        self.doorlogic.policy("Door",
                                doorlogic.DoorLogic.Origin.DOOR,
                                'bell_code',
                                doorlogic.DoorLogic.Input.BUTTON,
                                True)

        self.doorlogic.temp_unlock.assert_called_once_with("Door")


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
