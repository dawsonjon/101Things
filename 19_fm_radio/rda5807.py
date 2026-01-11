#  _  ___  _   _____ _     _
# / |/ _ \/ | |_   _| |__ (_)_ __   __ _ ___
# | | | | | |   | | | '_ \| | '_ \ / _` / __|
# | | |_| | |   | | | | | | | | | | (_| \__ \
# |_|\___/|_|   |_| |_| |_|_|_| |_|\__, |___/
#                                  |___/
#
# Copyright (c) Jonathan P Dawson 2024
# filename: rda5807.py
# description:
#
# Python module to interface with RDA5807 FM radio device using I2C bus
#
# License: MIT
#

from machine import RTC

random_access_address = 17

#Register addresses
RDA5807M_REG_CHIPID = 0x00
RDA5807M_REG_CONFIG = 0x02
RDA5807M_REG_TUNING = 0x03
RDA5807M_REG_GPIO   = 0x04
RDA5807M_REG_VOLUME = 0x05
RDA5807M_REG_I2S    = 0x06
RDA5807M_REG_BLEND  = 0x07
RDA5807M_REG_FREQ   = 0x08
RDA5807M_REG_STATUS = 0x0A
RDA5807M_REG_RSSI   = 0x0B
RDA5807M_REG_RDSA   = 0x0C
RDA5807M_REG_RDSB   = 0x0D
RDA5807M_REG_RDSC   = 0x0E
RDA5807M_REG_RDSD   = 0x0F
RDA5800_REG_LNA     = 0x10
RDA5807M_REG_SEEK   = 0x20

#Flag bits (to the chip)
RDA5807M_FLG_DHIZ = 0x8000
RDA5807M_FLG_DMUTE = 0x4000
RDA5807M_FLG_MONO = 0x2000
RDA5807M_FLG_BASS = 0x1000
RDA5807M_FLG_RCLKNOCAL = 0x0800
RDA5807M_FLG_RCLKDIRECT = 0x0400
RDA5807M_FLG_SEEKUP = 0x0200
RDA5807M_FLG_SEEK = 0x0100
RDA5807M_FLG_SKMODE = 0x0080
RDA5807M_FLG_RDS = 0x0008
RDA5807M_FLG_NEW = 0x0004
RDA5807M_FLG_RESET = 0x0002
RDA5807M_FLG_ENABLE = 0x0001
RDA5807M_FLG_DIRECT = 0x0020
RDA5807M_FLG_TUNE = 0x0010
RDA5807M_FLG_DE = 0x0800
RDA5807M_FLG_SOFTMUTE = 0x0200
RDA5807M_FLG_AFCD = 0x0100
RDA5807P_FLG_INTMODE = 0x8000
RDA5807M_FLG_EASTBAND65M = 0x0200
RDA5807M_FLG_SOFTBLEND = 0x0002
RDA5807M_FLG_FREQMODE = 0x0001
RDA5807M_FLG_FMTRUE = 0x0100
RDA5807M_FLG_FMREADY = 0x0080
RDA5807M_FLG_BLOCKE = 0x0010
RDA5807P_FLG_STCIEN = 0x4000
RDA5807P_FLG_I2S = 0x0040
RDA5807P_FLG_I2SSLAVE = 0x1000
RDA5807P_FLG_SWLR = 0x0800
RDA5807P_FLG_SCLKINVERT_I = 0x0400
RDA5807P_FLG_SIGNED = 0x0200
RDA5807P_FLG_WSINVERT_I = 0x0100
RDA5807P_FLG_WSINVERT_O = 0x0008
RDA5807P_FLG_SCLKINVERT_O = 0x0004
RDA5807P_FLG_DELAY_L = 0x0002
RDA5807P_FLG_DELAY_R = 0x0001
RDA5800_FLG_SPACE_200K = 0x0001
RDA5800_FLG_SPACE_50K = 0x0004
RDA5800_FLG_BAND_JAPAN = 0x0002

rds_program_types_europe = [
"No programme type defined", "News", "Current affairs", "Information",
"Sport", "Education", "Drama", "Culture", "Science", "Varied",
"Popular Music (Pop)", "Rock Music", "Easy Listening", "Light Classical",
"Serious Classical", "Other Music", "Weather", "Finance", "Children's Programmes",
"Social Affairs", "Religion", "Phone-in", "Travel", "Leisure", "Jazz Music",
"Country Music", "National Music", "Oldies Music", "Folk Music", "Documentary",
"Alarm Test","Alarm"]

class Radio:

    """ Access RDA5807M Device """

    def __init__(self, i2c, region="US/Europe", frequency_spacing_kHz=100):

        """ Configure RDA5807M Device

        arguments:

        i2c                     - an I2C bus object
        region                  - "US/Europe" | "Japan" | "World Wide" | "East Europe"
        frequency_spacing_kHz   - 25 | 50 | 100 | 200

        """

        self.i2c = i2c
        self.mute_flag = False
        self.bass_boost_flag = True
        self.mono = False
        self.rtc = RTC()

        #read chip ID and check
        data = self.read_reg(RDA5807M_REG_CHIPID)
        if(data>>8 == 0x58):
            print("Radio Found!")

        #configure radio
        flags = RDA5807M_FLG_DHIZ | RDA5807M_FLG_BASS | RDA5807M_FLG_ENABLE | RDA5807M_FLG_NEW | RDA5807M_FLG_SEEKUP | RDA5807M_FLG_RDS
        self.write_reg(RDA5807M_REG_CONFIG, 0b11000001 | RDA5807M_FLG_RESET)
        self.write_reg(RDA5807M_REG_TUNING, 0x0)
        self.write_reg(RDA5807M_REG_GPIO, 0x1a00)
        self.write_reg(RDA5807M_REG_VOLUME, 0x880f)
        self.write_reg(RDA5807M_REG_I2S, 0x0000)
        self.write_reg(RDA5807M_REG_BLEND, 0x4202)

        self.write_reg(RDA5807M_REG_CONFIG, flags)

        if region == "US/Europe":
            self.start_frequency_MHz = 87.0
            self.band = 0
        elif region == "Japan":
            self.start_frequency_MHz = 76.0
            self.band = 1
        elif region == "World Wide":
            self.start_frequency_MHz = 76.0
            self.band = 2
        elif region == "East Europe":
            self.start_frequency_MHz = 65.0
            self.band = 3

        self.frequency_spacing_MHz = frequency_spacing_kHz/1000.0
        if frequency_spacing_kHz == 100:
            self.spacing = 0
        elif frequency_spacing_kHz == 200:
            self.spacing = 1
        elif frequency_spacing_kHz == 50:
            self.spacing = 2
        elif frequency_spacing_kHz == 25:
            self.spacing = 3

        self.write_reg(RDA5807M_REG_TUNING, 0x10 | (self.band << 2) | self.spacing)
        self.clear_rds_data()

    def clear_rds_data(self):

        """ Clear RDS data, e.g. after retuning """

        self.last_ab = 0
        self.station_name = [" " for i in range(8)]
        self.station_name_buffer = [" " for i in range(8)]
        self.radio_text = [" " for i in range(64)]
        self.radio_text_buffer = [" " for i in range(64)]
        self.last_offset = 0
        self.last_st_offset = 0
        self.hours = 0
        self.minutes = 0

    def read_reg(self, reg):

        """ Read data from i2c register """

        self.i2c.writeto(random_access_address, bytes([reg]))
        data = self.i2c.readfrom(random_access_address, 2)
        return (data[0] << 8) | data[1]

    def write_reg(self, reg, data):

        """ Write data to i2c register """

        data = bytes([reg, data >> 8, data&0xff])
        self.i2c.writeto(random_access_address, data)

    def update_reg(self, reg, mask, value):

        """ Update specific bits in I2C register """

        data = self.read_reg(reg)
        data = (data & ~mask) | value
        self.write_reg(reg, data)

    def set_volume(self, volume):

        """ Set volume 0 to 15 """

        volume &= 0xf
        self.update_reg(RDA5807M_REG_VOLUME, 0xf, volume) #bits 3:0

    def get_volume(self):

        """ Get Volume 0 to 15 """

        return self.read_reg(RDA5807M_REG_VOLUME) & 0xf #bits 3:0

    def mute(self, mute):

        """ Mute True = mute False = Normal """

        if not mute:
          self.update_reg(RDA5807M_REG_CONFIG, RDA5807M_FLG_DMUTE, RDA5807M_FLG_DMUTE)
        else:
          self.update_reg(RDA5807M_REG_CONFIG, RDA5807M_FLG_DMUTE, 0)
        self.mute_flag = mute

    def bass_boost(self, bass_boost):

        """ Bass Boost True = enable bass boost False = disable bass boost """

        if bass_boost:
          self.update_reg(RDA5807M_REG_CONFIG, RDA5807M_FLG_BASS, RDA5807M_FLG_BASS)
        else:
          self.update_reg(RDA5807M_REG_CONFIG, RDA5807M_FLG_BASS, 0)
        self.bass_boost_flag = bass_boost

    def mono(self, mono):

        """ Force Mono True = force mono False = stereo """

        if mono:
          self.update_reg(RDA5807M_REG_CONFIG, RDA5807M_FLG_MONO, RDA5807M_FLG_MONO)
        else:
          self.update_reg(RDA5807M_REG_CONFIG, RDA5807M_FLG_MONO, 0)
        self.mono_flag = mono

    def seek_up(self):

        """ Find next station (blocks until tuning completes) """

        self.clear_rds_data()
        self.update_reg(RDA5807M_REG_CONFIG,
            (RDA5807M_FLG_SEEKUP | RDA5807M_FLG_SEEK),
            (RDA5807M_FLG_SEEKUP | RDA5807M_FLG_SEEK))
        while 1:
            data = self.read_reg(RDA5807M_REG_CONFIG)
            if not data & RDA5807M_FLG_SEEK:
                break

    def seek_down(self):

        """ Find previous station (blocks until tuning completes) """

        self.clear_rds_data()
        self.update_reg(RDA5807M_REG_CONFIG,
            (RDA5807M_FLG_SEEKUP | RDA5807M_FLG_SEEK),
            RDA5807M_FLG_SEEK)
        while 1:
            data = self.read_reg(RDA5807M_REG_CONFIG)
            if not data & RDA5807M_FLG_SEEK:
                break

    def get_frequency_MHz(self):

        """ Get tuned frequency in MHz """

        frequency = self.start_frequency_MHz + ((self.read_reg(RDA5807M_REG_STATUS) & 0x3ff) * self.frequency_spacing_MHz)
        return frequency

    def set_frequency_MHz(self, frequency_MHz):

        """ Set tuned frequency in MHz """

        self.clear_rds_data()
        frequency_steps = int((frequency_MHz - self.start_frequency_MHz)/self.frequency_spacing_MHz)
        data = (frequency_steps << 6) | 0x10 | (self.band << 2) | self.spacing
        self.write_reg(RDA5807M_REG_TUNING, data)

    def get_signal_strength(self):

        """ Recieved Signal Strength Indicator 0 = low, 7 = high (logarithmic)"""

        rssi = round(7*(self.read_reg(RDA5807M_REG_RSSI) >> 9)/127)
        return rssi

    def get_rds_block_group(self):

        """ Read all 4 RDS blocks from device """

        return self.read_reg(RDA5807M_REG_RDSA), self.read_reg(RDA5807M_REG_RDSB), self.read_reg(RDA5807M_REG_RDSC), self.read_reg(RDA5807M_REG_RDSD)

    def update_rds(self):

        """ Check for new RDS messages and decode if present

        Should be polled regularly so that we don't miss any.
        Returns true if new data received.

        .station_name, .radio_text contain decoded data.

        machine RTC is updated upon reception of time/date messages"""

        if (self.read_reg(RDA5807M_REG_STATUS) & 0x8000):

            #check for uncorrectable errors
            if (self.read_reg(RDA5807M_REG_RSSI) & 0x3) == 0x3:
                return False
            if (self.read_reg(RDA5807M_REG_RSSI) & 0xc) == 0xc:
                return False

            a, b, c, d = self.get_rds_block_group()

            program_information = a
            group_type = b >> 12
            group_version = (b >> 11) & 1
            traffic_program = (b >> 10) & 1
            program_type = (b >> 5) & 0x1f

            #station name
            if group_type == 0:
                offset = b & 0x3
                character_a = chr(d >> 8)
                character_b = chr(d & 0xff)

                #Check multiple messages for consistency
                self.station_name_buffer[offset*2] = character_a
                self.station_name_buffer[(offset*2)+1] = character_b
                if offset < self.last_st_offset:
                    self.station_name = self.station_name_buffer
                self.last_st_offset = offset

            #radio text
            elif group_type == 2 and group_version == 0:
                offset = b & 0xf

                ab = (b >> 4) & 1
                character_a = c >> 8
                character_b = c & 0xff
                character_c = d >> 8
                character_d = d & 0xff
                if ab != self.last_ab:
                    self.clear_buffer = True
                    self.radio_text_buffer = [" " for i in range(64)]
                self.last_ab = ab
                self.radio_text_buffer[offset*4] = chr(character_a)
                self.radio_text_buffer[(offset*4)+1] = chr(character_b)
                self.radio_text_buffer[(offset*4)+2] = chr(character_c)
                self.radio_text_buffer[(offset*4)+3] = chr(character_d)

                if offset < self.last_offset:
                    self.radio_text = self.radio_text_buffer
                self.last_offset = offset

            #radio text type 2
            elif group_type == 2 and group_version == 1:
                offset = b & 0xf
                ab = (b >> 4) & 1
                character_c = d >> 8
                character_d = d & 0xff
                if ab != self.last_ab:
                    self.radio_text_buffer = [" " for i in range(64)]
                self.radio_text_buffer[(offset*2)+0] = chr(character_c)
                self.radio_text_buffer[(offset*2)+1] = chr(character_d)

                if offset < self.last_offset:
                    self.radio_text = self.radio_text_buffer
                self.last_offset = offset

            elif group_type == 4 and group_version == 0:
                hours_utc = ((c & 1) << 4) | ((d & 0xf000) >> 12)
                minutes = ((d & 0xfc0) >> 6)
                utc_offset = (d & 0x1f) * 0.25
                utc_sign = d & 0x20

                if utc_sign:
                    hours = hours_utc + utc_offset
                else:
                    hours = hours_utc - utc_offset
                try:
                    self.rtc.datetime((2000, 1, 1, 1, int(hours), int(minutes), 0, 0))
                except OSError:
                    pass

        return True
