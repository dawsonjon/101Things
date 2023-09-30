import socket
import time
import os
import numpy as np
from matplotlib import pyplot as plt

SERVER_ADDR = '192.168.1.120'
SERVER_PORT = 4242

while 1:

    #list available clips
    path = os.path.split(__file__)[0]
    path = os.path.join(path, "clips")
    clips = os.listdir(path)
    print("available clips")
    clips = dict(enumerate(sorted(clips)))
    for idx, clip in clips.items():
        print(idx, clip)

    #prompt user to select clip
    while 1:
        print("select clip >")
        idx = input()
        clip = clips.get(int(idx), None)
        if clip is not None:
            clip = os.path.join(path, clip)
            break

    #play clip
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.connect((SERVER_ADDR, SERVER_PORT))
    print("connected")

    packet_size = 1024
    with open(clip, "rb") as clip:
        start_time = time.time()
        total_samples = 0
        while 1:
            samples = clip.read(packet_size)
            samples = bytes([(i+128)&0xff for i in samples])
            total_samples += len(samples)
            if len(samples) == 0:
                break
            s.sendall(samples)

    s.close()
    print("Samples:", total_samples)
    print("Duration: (seconds)", time.time()-start_time)
    print("Sample Rate (samples/second): ", total_samples/(time.time()-start_time))

