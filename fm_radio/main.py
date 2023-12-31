from machine import Pin, I2C
from time import sleep

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

RDA5807M_BAND_MASK  = 0x000C
RDA5807M_BAND_SHIFT = 2
RDA5807M_BAND_WEST = (0x0 << 2)
RDA5807M_BAND_JAPAN = (0x1 << 2)
RDA5807M_BAND_WORLD = (0x2 << 2)
RDA5807M_BAND_EAST = (0x3 << 2)

class Radio:
    
    def __init__(self, i2c):
        self.i2c = i2c
        
        #read chip ID and check
        data = self.read_reg(RDA5807M_REG_CHIPID)
        if(data>>8 == 0x58):
            print("Radio Found!")
        
        #configure radio
        flags = RDA5807M_FLG_DHIZ | RDA5807M_FLG_DMUTE | RDA5807M_FLG_BASS | RDA5807M_FLG_ENABLE | RDA5807M_FLG_NEW | RDA5807M_FLG_SEEKUP | RDA5807M_FLG_RDS
        print(hex(flags))#

        self.write_reg(0x02, 0b1100000000000011)
        self.write_reg(0x03, 0b0000000000000000)
        self.write_reg(0x04, 0b0000101000000000)
        self.write_reg(0x05, 0b1000100000001111)
        self.write_reg(0x06, 0b0000000000000000)
        self.write_reg(0x07, 0b0100001000000010)
        
        sleep(1)
        self.write_reg(0x02, 0b1100000000000001)
        frequency_MHz = 104
        frequency_steps = int((frequency_MHz - 87)/0.1)
        data = (frequency_steps << 6) | 0x10
        self.write_reg(0x03, data)

 


        #self.write_reg(RDA5807M_REG_CONFIG, 0b11000001 | RDA5807M_FLG_RESET)
        #self.write_reg(RDA5807M_REG_TUNING, 0x0)
        #self.write_reg(RDA5807M_REG_GPIO, 0x0a00)
        #self.write_reg(RDA5807M_REG_VOLUME, 0x880f)
        #self.write_reg(RDA5807M_REG_I2S, 0x0000)
        #self.write_reg(RDA5807M_REG_BLEND, 0x4202)
        
        #self.write_reg(RDA5807M_REG_CONFIG, flags)#release reset
        #self.write_reg(RDA5807M_REG_TUNING, 0x10)
        #self.set_frequency(104)
        #self.seek_up()

        #while 1:
            #data = self.read_reg(RDA5807M_REG_TUNING)
            #print("tuning register:", hex(data))
            #if not data & 0x10:
                #break
        #self.write_reg(RDA5807M_REG_GPIO, 0x0)

    def read_reg(self, reg):
        self.i2c.writeto(random_access_address, bytes([reg]))
        data = self.i2c.readfrom(random_access_address, 2)
        return (data[0] << 8) | data[1]

    def write_reg(self, reg, data):
        data = bytes([reg, data >> 8, data&0xff])
        self.i2c.writeto(random_access_address, data)
        
    def update_reg(self, reg, mask, value):
        data = self.read_reg(reg)
        print(hex(reg))
        print("old", hex(data))
        data = (data & ~mask) | value
        print("new", hex(data))
        self.write_reg(reg, data)
        
    def set_volume(self, volume):
        """0 to 15"""
        volume &= 0xf
        self.update_reg(RDA5807M_REG_VOLUME, 0xf, volume) #bits 3:0
        
    def get_volume(self):
        """0 to 15"""
        return self.read_reg(RDA5807M_REG_VOLUME) & 0xf #bits 3:0
    
    def seek_up(self):
        self.update_reg(RDA5807M_REG_CONFIG,
            (RDA5807M_FLG_SEEKUP | RDA5807M_FLG_SEEK), 
            (RDA5807M_FLG_SEEKUP | RDA5807M_FLG_SEEK))
        while 1:
            data = self.read_reg(RDA5807M_REG_CONFIG)
            print("control register:", hex(data))
            if not data & RDA5807M_FLG_SEEK:
                break
        
    def seek_down(self):
        self.update_reg(RDA5807M_REG_CONFIG,
            (RDA5807M_FLG_SEEKUP | RDA5807M_FLG_SEEK),
            RDA5807M_FLG_SEEK)
        
    def get_frequency(self):
        frequency = 87 + ((self.read_reg(RDA5807M_REG_STATUS) & 0x3ff) * 0.1)
        return frequency
    
    def set_frequency(self, frequency_MHz):
        frequency_steps = int((frequency_MHz - 87)/0.1)
        data = (frequency_steps << 6) | 0x10
        self.write_reg(RDA5807M_REG_TUNING, data)
        
        while 1:
            data = self.read_reg(RDA5807M_REG_TUNING)
            print("tuning register:", hex(data))
            if not data & 0x10:
                break
    
    def get_status(self):
        return hex(self.read_reg(RDA5807M_REG_STATUS))
    
    
                
                 
i2c=I2C(1,sda=Pin(26), scl=Pin(27), freq=40000)
radio = Radio(i2c)
radio.set_volume(5)


