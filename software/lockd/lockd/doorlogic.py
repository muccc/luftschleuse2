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
import time

class DoorLogic():
    class Origin:
        DOOR            =   1
        NETWORK         =   2
        CONTROL_PANNEL  =   3
        INTERNAL        =   4
    
    class Input:
        BUTTON          =   1
        COMMAND         =   2
        SWITCH          =   3
        SENSOR          =   4

    class State:
        DOWN            =   1
        CLOSED          =   2
        MEMBER          =   3
        OPEN            =   4
        PUBLIC          =   5
        UNKNOWN         =   6

    def __init__(self):
        self.logger = logging.getLogger('logger')
        self.allow_public = True
        self.timers = []
        self.doors = {}
        self.state_listeners = []
        self.state = self.State.UNKNOWN
        self.all_doors_locked = False
        self.all_doors_unlocked = False

    '''
    Accepts an input from a device. Checks if the action is valid and allowed.
    Returns the command that should be executed by the system
    '''
    def policy(self, origin_name, origin_type, input_name, input_type, input_value):
        self.logger.debug('New DoorLogic input: %s, %d, %s, %d, %s'%(origin_name, origin_type, input_name, input_type, input_value))

        if origin_type == self.Origin.NETWORK:
            if input_type == self.Input.COMMAND:
                if input_value == 'unlock':
                    # TODO: change this to doors with the
                    # initial unlock property set to 'true'.
                    self.temp_unlock('Back Door')
                if input_value == 'lock':
                    self.lock('all')
                    self.set_state(self.State.DOWN)
                if input_value == 'unlockfront':
                    self.temp_unlock('Front Door')

        if origin_type == self.Origin.DOOR:
            if input_type == self.Input.BUTTON and input_value == True:
                if input_name == 'manual_control':
                    if self.state == self.State.DOWN:
                        self.set_state(self.State.CLOSED)
                    elif self.state == self.State.CLOSED:
                        self.set_state(self.State.MEMBER)
                    elif self.state == self.State.MEMBER:
                        self.set_state(self.State.PUBLIC)
                    elif self.state == self.State.PUBLIC:
                        self.set_state(self.State.DOWN)
                    elif self.is_locked(origin_name):
                        self.unlock(origin_name)
                    else:
                        self.lock(origin_name)
                elif input_name == 'bell_code':
                    if (self.state == self.State.MEMBER or self.state == self.State.PUBLIC) and \
                            self.is_locked(origin_name):
                        self.temp_unlock(origin_name)

        if False: # FIXME: pannel b0rk :( origin_type == self.Origin.CONTROL_PANNEL:
            if input_type == self.Input.BUTTON and input_value == True:
                if input_name == 'down':
                    self.lock('all')
                    self.set_state(self.State.DOWN)
                elif input_name == 'closed':
                    self.lock('all')
                    self.set_state(self.State.CLOSED)
                elif input_name == 'member':
                    self.lock('all')
                    self.set_state(self.State.MEMBER)
                elif input_name == 'public':
                    self.unlock('Front Door')
                    self.set_state(self.State.PUBLIC)
            elif input_type == self.Input.BUTTON and input_value == False:
                if input_name == 'public':
                    if self.state == self.State.PUBLIC:
                        self.lock('all')
                        self.set_state(self.State.MEMBER)
        if origin_type == self.Origin.INTERNAL:
            if input_type == self.Input.COMMAND:
                if input_value == 'down':
                    self.lock('all')
                    self.set_state(self.State.DOWN)

    def set_state(self, state):
        if state != self.state:
            self.state = state
            self.notify_state_listeners()

    def add_state_listener(self, listener):
        if listener not in self.state_listeners:
            self.state_listeners.append(listener)
    
    def notify_state_listeners(self):
        for listener in self.state_listeners:
            listener(self)

    # The state of a door has changed
    # Check what is going on and send updates to all parts of the system
    def door_state_update(self, door):
        state_changed = False

        all_doors_locked = all([d.is_locked() for d in self.doors.values()])
        all_doors_unlocked = all([not d.is_locked() for d in self.doors.values()])
        
        if self.all_doors_locked != all_doors_locked or self.all_doors_unlocked != all_doors_unlocked:
            state_changed = True

        self.all_doors_locked = all_doors_locked
        self.all_doors_unlocked = all_doors_unlocked

        if state_changed:
            self.notify_state_listeners()

    # Locks one or all doors in the system
    def lock(self, door_name):
        if door_name == 'all':
            for door in self.doors.values():
                 door.lock()
        else:
            self.doors[door_name].lock()

    # Opens a specific door for a short amount of time to
    # allow someone to enter and operate the system from the inside
    def temp_unlock(self, door_name):
        self.logger.debug("temp_unlock(%s)", door_name)
        self.unlock(door_name)
        self.add_timer(5, self.lock, (door_name,))

    # Unlocks one door without a time limit
    def unlock(self, door_name):
        if door_name == 'all':
            for door in self.doors.values():
                 door.unlock()
        else:
            self.doors[door_name].unlock()

    # returns true if a specific door is manually unlocked
    def is_manual_unlocked(self, door_name):
        #if door_name in doors:
        return self.doors[door_name].is_manual_unlocked();
        #return False
    
    # returns true if a specific door is locked
    def is_locked(self, door_name):
        #if door_name in doors:
        return self.doors[door_name].is_locked();
        #return False
   
    def add_timer(self, timeout, function, arguments):
        self.logger.debug('timer: new %s %s %s'%(timeout, function, arguments))
        self.timers.append((time.time()+timeout, function, arguments))

    def tick(self):
        the_time = time.time()
        
        execution_list = [timer for timer in self.timers if timer[0] <= the_time]
        self.timers = [timer for timer in self.timers if timer[0] > the_time]

        for timer in execution_list:
            self.logger.debug('timer: executing %s %s'%(timer[1], timer[2]))
            timer[1](*timer[2])
                
    def add_door(self, door):
        self.doors[door.name] = door
        door.add_state_listener(self.door_state_update)
    
    def get_state_as_string(self):
        if self.state == self.State.DOWN:
            return 'down'
        elif self.state == self.State.CLOSED:
            return 'closed'
        elif self.state == self.State.MEMBER:
            return 'member'
        elif self.state == self.State.PUBLIC:
            return 'public'
        elif self.state == self.State.UNKNOWN:
            return 'unknown'
    
    # Returns true if a door is in a state not in
    # sync with the global system state.
    def is_state_tainted(self):
        if self.state == self.State.PUBLIC and \
                not self.all_doors_unlocked:
            return True
        
        if self.state != self.State.PUBLIC and \
                not self.all_doors_locked:
            return True

        return False
        
