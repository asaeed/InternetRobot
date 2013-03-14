#!/usr/bin/python27

import tornado.httpserver
import tornado.ioloop
import tornado.web
import tornado.websocket
import tornado.gen
from tornado.options import define, options
import json

define("port", default=8080, help="run on the given port", type=int)

browsers = []
robots = []

class IndexHandler(tornado.web.RequestHandler):
    def get(self):
        self.render('index.html')

class WebSocketHandler(tornado.websocket.WebSocketHandler):
    def open(self):
        print 'new connection'
        #self.write_message("connected")

    def on_message(self, message):
        print 'tornado received from client: %s' % message
        
        # decode to check contents of json
        decoded = json.loads(message)

        # if the client is informing server who it is, store it
        if ('clientType' in decoded):
            clientType = decoded['clientType']
            print "clientType: " + clientType
            if (clientType == 'robot'):
                robots.append(self)
            if (clientType == 'browser'):
                browsers.append(self)
        else:
            # if sender is robot, send to browsers
            if (decoded['from'] == 'robot'):
                for b in browsers:
                    b.write_message(message)

            # if sender is browser, send to robot and other browsers
            if (decoded['from'] == 'browser'):
                for r in robots:
                    r.write_message(message)
                for b in browsers:
                    if (b != self):
                        b.write_message(message)

    def on_close(self):
        print 'connection closed'
        browsers.remove(self)
        robots.remove(self)

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

    
