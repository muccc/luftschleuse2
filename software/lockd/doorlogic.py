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

    class State:
        DOWN            =   1
        CLOSED          =   2
        MEMBER         =   3
        OPEN            =   4
        PUBLIC          =   5

    def __init__(self):
        self.logger = logging.getLogger('logger')
        self.allow_public = True
        self.timers = []
        self.doors = []
        self.state_listeners = []
        self.state = None
        self.all_locked = False
        self.all_private = False
        self.all_public = False

    '''
    Accepts an input from a device. Checks if the action is valid and allowed.
    Returns the command that should be executed by the system
    '''
    def policy(self, origin_name, origin_type, input_name, input_type, input_value):
        self.logger.debug('New DoorLogic input: %s, %d, %s, %d, %s'%(origin_name, origin_type, input_name, input_type, input_value))

        if origin_type == self.Origin.NETWORK:
            if input_type == self.Input.COMMAND:
                if input_value == 'open':
                    self.open_temp()
                if input_value == 'close':
                    self.lock('all')
                    self.set_state(self.State.DOWN)

        if origin_type == self.Origin.DOOR:
            if input_type == self.Input.BUTTON:
                if input_name == 'public':
                    if not self.is_public(origin_name):
                        self.public(origin_name)
                    else:
                        if self.state == self.State.DOWN:
                            self.lock(origin_name)
                        elif self.state == self.State.CLOSED:
                            self.lock(origin_name)
                        elif self.state == self.State.MEMBER:
                            self.private(origin_name)
                        elif self.state == self.State.PUBLIC:
                            pass
                        else:
                            self.lock(origin_name)

        if origin_type == self.Origin.CONTROL_PANNEL:
            if input_type == self.Input.BUTTON:
                if input_name == 'down':
                    self.lock('all')
                    self.set_state(self.State.DOWN)
                elif input_name == 'closed':
                    self.lock('all')
                    self.set_state(self.State.CLOSED)
                elif input_name == 'member':
                    self.private('all')
                    self.set_state(self.State.MEMBER)
                elif input_name == 'public':
                    self.public('all')
                    self.set_state(self.State.PUBLIC)
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
        system_state_changed = False

        all_locked = all([d.is_locked() for d in self.doors])
        all_private = all([d.is_private() for d in self.doors])
        all_public = all([d.is_public() for d in self.doors])
        
        if self.all_locked != all_locked or self.all_private != all_private or self.all_public != all_public:
            system_state_changed = True

        self.all_locked = all_locked
        self.all_private = all_private
        self.all_public = all_public

        #if system_state_changed:
        #    self.notify_system_state_listeners()

    # Locks one or all doors in the system
    def lock(self, door_name):
        for door in self.doors:
            if door_name == 'all' or door.name == door_name:
                door.lock()
        
    # Puts one or all doors into the private mode.
    # If the door is in public mode, it gets locked for the public
    def private(self, door_name):
        # TODO: implement
        pass

    # Opens a specific door for a short amount of time to
    # allow someone to enter and operate the system from the inside
    def temp_public(self):
        self.public('front')
        self.add_timer(5, self.close, ('front',))

    # Unlocks one door without a time limit for the public
    def public(self, door_name):
        for door in self.doors:
            if door.name == door_name or door_name == 'all':
                 door.unlock()

    # Unlocks one door without a time limit for the public
    def is_public(self, door_name):
        for door in self.doors:
            if door.name == door_name:
                 return door.is_public()
        return False
   
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
    
    def get_state_as_string(self):
        if self.state == self.State.DOWN:
            return 'down'
        if self.state == self.State.CLOSED:
            return 'closed'
        if self.state == self.State.MEMBER:
            return 'memeber'
        if self.state == self.State.PUBLIC:
            return 'public'

        return 'fnord'
        
