import serial
import string
import sys
import time
import logging

class SerialInterface:
    def  __init__ ( self, path2device, baudrate, timeout=0):
      self.portopen = False
      self.dummy = False

      if path2device == '/dev/null':
            self.dummy = True
            self.portopen = True

      self.logger = logging.getLogger('logger')

      while not self.portopen:
        try:
            self.ser = serial.Serial(path2device, baudrate)
            self.path2device = path2device
            self.baudrate = baudrate
            self.timeout = timeout
            self.ser.flushInput()
            self.ser.flushOutput()
            if timeout:
                self.ser.setTimeout(timeout)
            self.portopen = True
            self.last = time.time()

        except serial.SerialException:
            self.logger.warning("Exception while opening %s"%path2device)
        time.sleep(1)

      self.logger.info("Opened %s"%path2device)

    def close(self):
        try:
            self.portopen = False
            self.ser.close()
        except serial.SerialException:
            pass
    def reinit(self):
        self.close()
        self.logger.warning("reopening")
        while not self.portopen:
            self.__init__(self.path2device, self.baudrate, self.timeout)
            time.sleep(1)
        self.logger.debug("done")

    def writeMessage(self,command,message):
        enc = "\\"+ command + message.replace('\\','\\\\') + "\\9";
        #self.logger.debug('writing %s' % list(enc))
        try:
            self.ser.write(enc)
            # Hack to avoid collisions, needs to be
            # convertet to some sort of queue management
            time.sleep(.05)
        except :
            pass
            #self.reinit()


    def write(self,message):
        self.logger.debug('writing %s'%list(message))
        if self.dummy:
            return
        try:
            self.ser.write(message)
        except :
            self.reinit()

    def readMessage(self):
        data = ""
        escaped = False
        stop = False
        start = False
        inframe = False
        command = ''
        while True:
            starttime = time.time()
            try:
                c = self.ser.read(1)
                #print list(c)
            except KeyboardInterrupt:
                raise KeyboardInterrupt
            except Exception, e:
                self.logger.warning("port broken 2")
                self.reinit()
                return (False, '')
            endtime = time.time()
            if len(c) == 0:             #A timout occured
            #    if endtime-starttime < (self.timeout * 0.5):
            #        self.reinit()
            #        raise Exception('No data after %f seconds. Port seems broken.'%(endtime-starttime))
            #    else:
                    #print 'TIMEOUT'
                    return (False, '')
            if escaped:
                if c == '\\':
                    d = '\\'
                elif c == '9':
                    stop = True
                    inframe = False
                else:
                    start = True
                    inframe = True
                    command = c
                    data = ""
                escaped = False
            elif c == '\\':
                escaped = 1
            else:
                d = c
                
            if start and inframe:
                start = False
            elif stop:
                #print 'received message: len=%d data=%s'%(len(data),data)
                #print 'received message. command=',command, "data=" ,list(data)
                #print time.time() - self.last
                self.last = time.time()
                #print (command, data)
                return (command, data)
            elif escaped == False and inframe:
                data += str(d)

