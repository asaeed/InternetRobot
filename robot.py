
import multiprocessing
import serial
import time
#from time import gmtime, strftime
from datetime import datetime
from ws4py.client.threadedclient import WebSocketClient

class PiClient(WebSocketClient):
    def __init__(self, serverAddress):
        WebSocketClient.__init__(self, serverAddress)
        self.pingTime = datetime.now()

    def opened(self):
        self.send("Hi there!")
        print "Connection opened..."

    def closed(self, code, reason=None):
        print "Connection closed... ", code, reason

    def received_message(self, m):
        print "Received from server: %s" % (str(m))

    def ping(self):
        # ping on 30 minute interval
        elapsedTime = datetime.now() - self.pingTime
        if (elapsedTime.seconds > 1800):
            self.send(str(self.pingTime))
            self.pingTime = datetime.now()

################################ MAIN ################################

def main():

    try:
        # initialize websocket client
        piClient = PiClient('ws://ec2-23-20-219-99.compute-1.amazonaws.com:8080/ws')
        piClient.connect()

        # initialize serial port
        serialPort = serial.Serial(port='/dev/ttyUSB0',baudrate='9600')
        serialPort.flushInput()

        while True:
            # keep websocket alive by sending ping on interval
            piClient.ping()

            # look for incoming serial data
            if (serialPort.inWaiting() > 0):
                result = serialPort.readline().replace("\n", "")

                # send it back to tornado
                piClient.send(result)

    except KeyboardInterrupt:
        piClient.close()
        serialPort.close()


if __name__ == "__main__":
    main()