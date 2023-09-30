# SPDX-FileCopyrightText: 2020 Jeff Epler for Adafruit Industries
#
# SPDX-License-Identifier: MIT

"""CircuitPython Essentials Audio Out MP3 Example"""
import board
import digitalio
import os
import random

from audiomp3 import MP3Decoder

try:
    from audioio import AudioOut
except ImportError:
    try:
        from audiopwmio import PWMAudioOut as AudioOut
    except ImportError:
        pass  # not always supported by every board!

files = [i for i in os.listdir() if i.endswith("mp3")]

# You have to specify some mp3 file when creating the decoder
mp3 = open(files[0], "rb")
decoder = MP3Decoder(mp3)
audio = AudioOut(board.GP0)

while True:
    sound = random.choice(files)
    print(sound)
    decoder.file = open(sound, "rb")
    audio.play(decoder)
    while audio.playing:
        pass
