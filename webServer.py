#!/usr/bin/python27

import tornado.httpserver
import tornado.ioloop
import tornado.web
import tornado.websocket
import tornado.gen
from tornado.options import define, options
import json

define("port", default=8080, help="run on the given port", type=int)

clients = []
robot = 0

class IndexHandler(tornado.web.RequestHandler):
    def get(self):
        self.render('index.html')

class WebSocketHandler(tornado.websocket.WebSocketHandler):
    def open(self):
        print 'new connection'
        print self
        clients.append(self)
        #self.write_message("connected")

    def on_message(self, message):
        print 'tornado received from client: %s' % message
        
        # decode to check contents of json
        decoded = json.loads(message)

        # if sender is robot, send to clients
        if (decoded['from'] == 'robot'):
            for c in clients:
                c.write_message(message)

        # if sender is client, send to robot and other clients
        if (decoded['from'] == 'client'):
            robot.write_message(message)
            for c in clients:
                if (c != self):
                    c.write_message(message)

    def on_close(self):
        print 'connection closed'
        clients.remove(self)

################################ MAIN ################################

def main():

    tornado.options.parse_command_line()
    app = tornado.web.Application(
        handlers=[
            (r"/", IndexHandler),
            (r"/ws", WebSocketHandler)
        ]
    )
    httpServer = tornado.httpserver.HTTPServer(app)
    httpServer.listen(options.port)
    print "Listening on port:", options.port
    tornado.ioloop.IOLoop.instance().start()

if __name__ == "__main__":
    main()

    
