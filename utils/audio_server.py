import socket
import time
import os
import sys
import subprocess

SERVER_PORT = 4242

while 1:

    #Use sox utility to stream access sound card
    record_process = subprocess.Popen(["rec", "-c", "1", "-t", "u8", "-r", "10000", "-"], stdout=subprocess.PIPE)
    play_process = subprocess.Popen(["play", "-c", "1", "-t", "u8", "-r", "10000", "-"], stdin=subprocess.PIPE)
    
    #connect via TCP socket
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
      s.bind(("", SERVER_PORT))
      s.listen()
      print("listening")
      conn, addr = s.accept()
      conn.setblocking(0)
      print("connected to", addr, SERVER_PORT, file=sys.stderr)

      #transfer sampels between wifi and soundcard
      while 1:
        try:
          samples = record_process.stdout.read(1024)
          conn.sendall(samples)
        except BlockingIOError:
          pass
        try:
          samples = conn.recv(1024)
          play_process.stdin.write(samples)
        except BlockingIOError:
          pass
      
#
    s.close()
