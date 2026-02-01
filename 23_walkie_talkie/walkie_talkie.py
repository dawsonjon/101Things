import socket, select
import subprocess
import time
import numpy as np

sample_number = 0
def generate_tone(frequency):
  global sample_number
  sample_rate = 10e3
  sample_numbers = np.arange(sample_number, sample_number+1024)
  sample_number += 1024
  samples = np.sin(sample_numbers * 2 * np.pi * frequency/sample_rate)
  samples = samples * 127
  samples += 127
  samples = samples.astype(np.uint8)
  return samples


def get_ip():
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    s.settimeout(0)
    try:
        # doesn't even have to be reachable
        s.connect(('10.254.254.254', 1))
        IP = s.getsockname()[0]
    except Exception:
        IP = '127.0.0.1'
    finally:
        s.close()
    return IP

local_address = get_ip()
data_port = 5005
broadcast_address = "255.255.255.255"
advertise_port = 5006

def pair():
  s = socket.socket(socket.AF_INET, # Internet
                                 socket.SOCK_DGRAM) # UDP
  s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
  s.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)
  s.bind((broadcast_address, advertise_port))

  while True:
      s.sendto(bytes("advertise walkie_talkie", "utf-8"), (broadcast_address, advertise_port))
      data, addr = s.recvfrom(1024) # buffer size is 1024 bytes
      if addr[0] != local_address:
        return addr[0]

  s.close()

def exchange_data(remote_ip):
  s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
  s.connect((remote_ip, data_port))

  #Use sox utility to stream access sound card
  record_process = subprocess.Popen(["rec", "-e", "a-law", "-c", "1", "-t", "u8", "-r", "10000", "-"], stdout=subprocess.PIPE)
  #play_process = subprocess.Popen(["play", "-e", "a-law", "-c", "1", "-t", "u8", "-r", "10000", "-"], stdin=subprocess.PIPE)

  while 1:
      samples = record_process.stdout.read(1024)
      s.sendall(samples)
      #try:
      #  samples = s.recv(1024)
      #  play_process.stdin.write(samples)
      #except BlockingIOError:
      #  pass

  s.close()

print("pairing")
remote_ip = pair()
print("paired to: ", remote_ip)
exchange_data(remote_ip)
