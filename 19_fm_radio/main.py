#  _  ___  _   _____ _     _                 
# / |/ _ \/ | |_   _| |__ (_)_ __   __ _ ___ 
# | | | | | |   | | | '_ \| | '_ \ / _` / __|
# | | |_| | |   | | | | | | | | | | (_| \__ \
# |_|\___/|_|   |_| |_| |_|_|_| |_|\__, |___/
#                                  |___/    
#
# Copyright (c) Jonathan P Dawson 2024
# filename: main.py
# description:
#
# RDA5807 example application
# 
# Uses ssd1306 128x64 OLED display and 4 push buttons to provide a simple user
# interface.
#
# License: MIT
#

import time
import ssd1306
import framebuf
import rda5807
from machine import Pin, I2C, ADC

def draw_image(display, filename):

    """ Load an image from flash into a framebuf object and display on screen """

    width = 128
    height = 64
    with open(filename, "br") as inf:
        data = inf.read(width * height // 8)
    fbuf = framebuf.FrameBuffer(bytearray(data), width, height, framebuf.MONO_HLSB)
    display.blit(fbuf, 0, 0, 0)
    display.show()

def draw_battery(display):

    """ Draw a battery level icon on screen. Uses pi-pico dedicated VSYS ADC channel """

    x = 0
    y = 0
    display.line(x+0,  y+0, x+14, y+0, 1)
    display.line(x+0,  y+8, x+14, y+8, 1)
    display.line(x+0,  y+0, x+0,  y+8, 1)
    display.line(x+14, y+0, x+14, y+2, 1)
    display.line(x+14, y+6, x+14, y+8, 1)
    display.line(x+15, y+2, x+15, y+6, 1)
    
    analogIn = ADC(26)
    batt_voltage = analogIn.read_u16() * 3.3 / 65536

    # based on 2 cell NiMH, values could be adjusted for different chemistry
    batt_voltage_max = 3.0
    batt_voltage_min = 0.9
    
    if batt_voltage > batt_voltage_max:
        batt_level = 4
    elif batt_voltage < batt_voltage_min:
        batt_level = 0
    else:
        batt_level = (batt_voltage - batt_voltage_min) / (batt_voltage_max - batt_voltage_min)
        batt_level = round(batt_level * 10.0)
    for i in range(2, 7):
        display.line(x+2, y+i, x+12-batt_level, y+i, 1)

def draw_signal_strength(display, radio):

    """ Draw a signal strength icon on screen. Queries radio's RSSI. """

    x = 110
    y = 0
    display.line(x+2,  y+0, x+2,  y+8, 1)
    display.line(x+0,  y+0, x+2,  y+4, 1)
    display.line(x+4,  y+0, x+2,  y+4, 1)

    level = radio.get_signal_strength()
    for i in range(level):
        display.line(x+4+(2*i),  y+8, x+4+(2*i),  y+8-i, 1)

def draw_display(display, radio, scroll_position):

    """ Update display based on current status """

    display.fill(0)
    t = radio.rtc.datetime()
    display.text(("%02u:%02u:%02u"%(t[4], t[5], t[6])).center(16), 0, 0)
    draw_signal_strength(display, radio)
    draw_battery(display)
    frequency = str(radio.get_frequency_MHz()).center(16)
    display.text(frequency, 0, 16)
    display.text("".join(radio.station_name).center(16), 0, 24)
    radio_text = "".join(radio.radio_text)
    display.text(radio_text[scroll_position:(scroll_position+16) & 0x3f], 0, 56)
    display.show()
    
def draw_seeking(display):

    """ Display this message when searching for stations """

    display.fill(0)
    display.text("seeking...".center(16), 0, 16)
    display.show()
    
def draw_splash(display):

    """ Display splash screen for first second """

    draw_image(display, "radio.fbuf")
    
def load_settings():

    """ Load settings from a file in flash on power-up """

    #default settings if no file present
    settings = {
        "volume" : 0,
        "mute" : False,
        "frequency_MHz" : 88
    }

    try:
        with open("settings.txt") as input_file:
            for line in input_file:
                key, value = line.split(":")
                key = key.strip()
                value = value.strip()
                settings[key] = value
    except OSError:
        pass
    
    return settings

def save_settings(settings):

    """ Save settings to a file when a change is made """

    with open("settings.txt", 'w') as output_file:
        for key, value in settings.items():
            line = "%s:%s\n"%(key, value)
            output_file.write(line)
    

#disable power save mode to reduce regulator noise
psu_mode = Pin(23, Pin.OUT)
psu_mode.value(1)

#configure buttons
left  = Pin(22, Pin.IN, Pin.PULL_UP)
right = Pin(21, Pin.IN, Pin.PULL_UP)
up    = Pin(20, Pin.IN, Pin.PULL_UP)
down  = Pin(17, Pin.IN, Pin.PULL_UP)

# using default address 0x3C
display_i2c = I2C(1, sda=Pin(18), scl=Pin(19))
display = ssd1306.SSD1306_I2C(128, 64, display_i2c)
display.show()
draw_splash(display)

# configure radio module
radio_i2c=I2C(0, sda=Pin(4), scl=Pin(5), freq=400000)
radio = rda5807.Radio(radio_i2c)
time.sleep_ms(1000)

#load settings from file
settings = load_settings()
radio.set_volume(int(settings["volume"]))
radio.set_frequency_MHz(float(settings["frequency_MHz"]))
radio.mute(settings["mute"]=="True")

#UI main loop
draw_display(display, radio, 0)
count = 0
scroll_position = 0
while 1:
    
    if count == 0:
        count = 19
        if scroll_position == 63:
            scroll_position = 0
        else:
            scroll_position += 1
            
        if(left.value() == 0):
            draw_seeking(display)
            radio.seek_down()
            settings["frequency_MHz"] = radio.get_frequency_MHz()
        elif(right.value() == 0):
            draw_seeking(display)
            radio.seek_up()
            settings["frequency_MHz"] = radio.get_frequency_MHz()
        elif(up.value() == 0):
            if settings["mute"] == "True":
                settings["mute"] = "False"
                radio.mute(settings["mute"] == "True")
            elif int(settings["volume"]) < 15:
                settings["volume"] = int(settings["volume"])+1
                radio.set_volume(settings["volume"])
        elif(down.value() == 0):
            if int(settings["volume"]) > 0:
                settings["volume"] = int(settings["volume"])-1
                radio.set_volume(settings["volume"])
            elif settings["mute"] == "False":
                settings["mute"]="True"
                radio.mute(settings["mute"] == "True")
                
        save_settings(settings)
        draw_display(display, radio, scroll_position)

    else:
        count -= 1
        
    radio.update_rds()    
    time.sleep_ms(10)
