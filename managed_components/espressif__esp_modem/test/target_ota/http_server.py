# SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import ssl
from http.server import HTTPServer

from RangeHTTPServer import RangeRequestHandler

server_address = ('0.0.0.0', 1234)
httpd = HTTPServer(server_address, RangeRequestHandler)
httpd.socket = ssl.wrap_socket(httpd.socket,
                               server_side=True,
                               certfile='srv.crt',
                               keyfile='srv.key',
                               ssl_version=ssl.PROTOCOL_TLS)
httpd.serve_forever()
