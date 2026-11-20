# -*- coding: utf-8 -*-
#test on python 3.4 ,python of lower version  has different module organization.
import os
import http.server
from http.server import HTTPServer, BaseHTTPRequestHandler
import socketserver

PORT = 8000

class CORSRequestHandler (http.server.SimpleHTTPRequestHandler):
    def end_headers (self):
        self.send_header('Access-Control-Allow-Origin', '*')
        self.send_header('Cross-Origin-Embedder-Policy', 'require-corp')
        self.send_header('Cross-Origin-Opener-Policy', 'same-origin')
        http.server.SimpleHTTPRequestHandler.end_headers(self)

    def do_GET(self):
        path = self.path.split('?')[0].split('#')[0]
        if not os.path.exists(self.translate_path(path)) and os.path.isfile(self.translate_path(path.rstrip('/') + '.html')):
            self.path = path.rstrip('/') + '.html' + self.path[len(path):]
        return http.server.SimpleHTTPRequestHandler.do_GET(self)

Handler = CORSRequestHandler#http.server.SimpleHTTPRequestHandler

Handler.extensions_map={
    '.manifest': 'text/cache-manifest',
	'.html': 'text/html',
    '.png': 'image/png',
	'.jpg': 'image/jpg',
	'.svg':	'image/svg+xml',
	'.css':	'text/css',
	'.js':	'application/x-javascript',
	'': 'application/octet-stream', # Default
    }

httpd = socketserver.TCPServer(("", PORT), Handler)

print("serving at port", PORT)
httpd.serve_forever()
