# Server program
# UDP VERSION


from socket import *

# Set the socket parameters
host = "localhost"
port = 23514
buf = 1024
addr = (host,port)

while 1:
    try:
        # Create socket and bind to address
        UDPSock = socket(AF_INET,SOCK_DGRAM)
        UDPSock.bind(addr)

        # Receive messages
        while 1:
            data,addr = UDPSock.recvfrom(buf)
            if not data:
                print "Client has exited!"
                break
            else:
                print data

        # Close socket
        UDPSock.close()
    except Exception,e:
        print e
    import time
    time.sleep(1)
