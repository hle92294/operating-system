import threading
import SocketServer
import time

class EchoRequestHandler(SocketServer.BaseRequestHandler):
    def setup(self):
        print self.client_address, 'connected!'
        self.request.send('hi ' + str(self.client_address) + '\n')

    def handle(self):
        while 1:
            data = self.request.recv(1024)
            self.request.send(data)
            if data.strip() == 'bye':
                return

    def finish(self):
        print self.client_address, 'disconnected!'
        self.request.send('bye ' + str(self.client_address) + '\n')

#server host is a tuple ('host', port)
server = SocketServer.ThreadingTCPServer(('localhost', 3000), EchoRequestHandler)
#server.serve_forever()
th = threading.Thread(target=server.serve_forever)
th.daemon = True
th.start()
while True:

    print('Doing something else')
    time.sleep(1)
