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
import logging

class DisplayLogic():
    def __init__(self, display_controller):
        self.logger = logging.getLogger('logger')
        self._display_controller = display_controller
        self._doors = {}
        self._state = False

    def tick(self):
        pass

    def update_state(self, state):
        self.logger.debug('DisplayLogic: update_state().')
        self._tainted = state.is_state_tainted()
        self._state = state
        self._update_display()
        
    def _update_display(self):
        if self._state == False:
            return

        self._display_controller.clear()

        if self._state.state == self._state.State.DOWN:
            if not self._tainted:
                pass
            else:
                pass
            self._render_state_name('DOWN')
        if self._state.state == self._state.State.CLOSED:
            pass
            self._render_state_name('CLOSED')
        if self._state.state == self._state.State.MEMBER:
            pass
            self._render_state_name('MEMBER')
        if self._state.state == self._state.State.PUBLIC:
            pass
            self._render_state_name('PUBLIC')

        i = 20
        for door in self._doors.values():
            door_state = ('Unknown', 'red')
            if door.is_locked():
                door_state = ('Locked', 'green')
            if door.is_perm_unlocked():
                door_state = ('Unlocked', 'green')

            self._display_controller.render_small((0,i), door.name, 'green')
            self._display_controller.render_small((50,i), door_state[0], door_state[1])
            i += 8
            pass

        self._display_controller.update()

    def _render_state_name(self, state_name):
        self._display_controller.render_large((0,0), state_name, 'red')

    def add_door(self, door):
        self._doors[door.name] = door
        door.add_state_listener(self._door_state_update)
 
    def _door_state_update(self, door):
        self.logger.debug('DisplayLogic: _door_state_update().')
        self._update_display()

        #state_changed = False
        #door.is_locked()
        #door.is_perm_unlocked()
        #print 'bar'

       







