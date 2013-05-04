import logging

class DoorLogic():
    class Origin:
        DOOR            =   1
        NETWORK         =   2
        CONTROL_PANNEL  =   3
        INTERNAL        =   4
    
    class Input:
        BUTTON           =   1
        COMMAND          =   2
        SWITCH           =   3
        SENSOR           =   4

    def __init__(self):
        self.logger = logging.get_logger('logger')
        self.allow_public = True
        self.timers = []
        self.doors = []

    '''
    Accepts an input from a device. Checks if the action is valid and allowed.
    Returns the command that should be executed by the system
    '''
    def policy(self, origin_name, origin_type, input_name, input_type, input_value):
        self.logger('New DoorLogic input: %d, %d, %d, %d, %d', origin_name, origin_type, input_name, input_type, input_value)

        if origin_type == self.Origin.NETWORK:
            if input_type == self.Input.COMMAND:
                if input_value == 'open':
                    self.open_temp()
                if input_value == 'close':
                    self.lock('all')
        if origin_type == self.Origin.DOOR:
            if input_type == self.Input.BUTTON:
                if input_name == 'public':
                    if self.allow_public:
                        self.public(origin_name)
                if input_name == 'private':
                    self.private(origin_name)
                if input_name == 'close':
                    self.lock(origin_name)
        if origin_type == self.Origin.CONTROL_PANNEL
            if input_type == self.Input.BUTTON:
                if input_name == 'private':
                    self.private('all')
                if input_name == 'close':
                    self.lock('all')
    
    # The state of a door has changed
    # Check what is going on and send updates to all parts of the system
    def door_state_update(self, door):
        system_state_changed = False

        all_locked = all([door.is_locked() for door in self.doors])
        all_private = all([door.is_private() for door in self.doors])
        all_public = all([door.is_public() for door in self.doors])
        

        if all_locked and not self.all_locked and self.announce_open:
            self.announce_open = False
            system_state_changed = True

        if self.all_locked != all_locked or
                self.all_private != all_private or
                self.all_public != all_public:
            system_state_changed = True

        if system_state_changed:
            self.notify_system_state_listeners()

    # Locks one or all doors in the system
    def lock(self, door_name):
        for door in self.doors:
            if value == 'all' or door.name == door_name:
                door.lock()
        
    # Opens a specific door for a short amount of time to
    # allow someone to enter and operate the system from the inside
    def temp_public(self):
        self.public('front')
        self.add_timer(5, self.close, ('front',))

    # Unlocks one door without a time limit for the public
    def public(self, door_name):
        for door in self.doors:
            if door.name == door_name:
                 door.unlock()
    
    # Puts one or all doors into the private mode.
    # If the door is in public mode, it gets locked for the public
    def private(self, door_name):
        pass

    def add_timer(self, timeout, function, arguments):
        self.timers.append((time.time()+timeout, function, arguments))

    def tick(self):
        the_time = time.time()
        
        execution_list = [timer for timer in self.timers if timer[0] > the_time]
        self.timers = [timer for timer in self.timers if timer[0] <= the_time]

        for timer in execution_list:
            timer[1](*timer[2])
                
    def add_door(self, door):
        self.doors.append(door)
        door.add_state_listener(self.door_state_update)








