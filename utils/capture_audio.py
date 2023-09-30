import socket
import time
import os
import sys
import subprocess

SERVER_ADDR = '192.168.1.120'
SERVER_PORT = 4242

while 1:

    process = subprocess.Popen(["play", "-t", "u8", "-r", "10000", "-"], stdin=subprocess.PIPE)
    

    #play clip
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.connect((SERVER_ADDR, SERVER_PORT))
    print("connected to", SERVER_ADDR, SERVER_PORT, file=sys.stderr)

    while 1:
      samples=s.recv(1024)
      process.stdin.write(samples)
      

    s.close()

