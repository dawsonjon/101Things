#  _  ___  _   _____ _     _                 
# / |/ _ \/ | |_   _| |__ (_)_ __   __ _ ___ 
# | | | | | |   | | | '_ \| | '_ \ / _` / __|
# | | |_| | |   | | | | | | | | | | (_| \__ \
# |_|\___/|_|   |_| |_| |_|_|_| |_|\__, |___/
#                                  |___/    
#
# Copyright (c) Jonathan P Dawson 2024
# filename: static_fft.py
# description:
# License: MIT
#

"""A static code generator for fixed size FFTs"""


from cmath import exp, pi
from math import gcd
import numpy as np

class StaticFFT:

    def static_fft(self, x):

        """Return Code to calculate FFT, similar to recursive function to calculate FFT"""

        N = len(x)
        if N <= 1: return x
        even = self.static_fft(x[0::2])
        odd =  self.static_fft(x[1::2])
        T=[self.define(self.twiddle(k, N, odd[k])) for k in range(N//2)]
        return [self.define(self.cadd(even[k], T[k])) for k in range(N//2)] + [self.define(self.csub(even[k], T[k])) for k in range(N//2)]


    def define(self, x):

        """Define a (const) variable and the code needed to calcualte it"""

        self.definitions.append(x)
        return "v%u_real"%(len(self.definitions)-1), "v%u_imag"%(len(self.definitions)-1) 


    def twiddle(self, k, N, x):
        
        """Return code to multiply sample X by appropriate twiddle factor"""

        #Normalise k/N
        kd = k//gcd(k, N)
        Nd = N//gcd(k, N)
        x_real, x_imag = x
        
        #Some tiwddle factors consist of 1s and 0s, resulting in trivial multiplications
        #treat these as a special case
        if kd == 0 and Nd == 1:
            return x_real, x_imag
        elif kd == 1 and Nd == 4:
            return x_imag, "-"+x_real
        elif kd == 1 and Nd == 2:
            return "-"+x_real, "-"+x_imag
        elif kd == 3 and Nd == 4:
            return "-"+x_imag, x_real
        else:
            T=exp(-2j*pi*k/N)
            if self.type_ == "float":
                t_real = "%0.7ff"%T.real
                t_imag = "%0.7ff"%T.imag
            else:
                t_real = str(T.real)
                t_imag = str(T.imag)
            return "(%s*%s)-(%s*%s)"%(x_real, t_real, x_imag, t_imag), "(%s*%s)+(%s*%s)"%(x_real, t_imag, x_imag, t_real)

    def varname_to_complex(self, x):
        varname, varnum = x.split("_")
        return "%s_real[%s]"%(varname, varnum), "%s_imag[%s]"%(varname, varnum)

    def cadd(self, x, y):
        x_real, x_imag = x
        y_real, y_imag = y
        return "%s+%s"%(x_real, y_real), "%s+%s"%(x_imag, y_imag)

    def csub(self, x, y):
        x_real, x_imag = x
        y_real, y_imag = y
        return "%s-%s"%(x_real, y_real), "%s-%s"%(x_imag, y_imag)

    def make_FFT(self, size, type_="double"):
        self.type_ = type_

        self.definitions = []

        #Assume input vector is held in an array called x
        x = [self.varname_to_complex("x_"+str(i)) for i in range(size)]
        y = self.static_fft(x)

        body = []

        #write out each definition in order
        index = 0
        for real, imag in self.definitions:
            name = str(index)
            index += 1
            body.append("const %s v%s_real = %s;"%(self.type_, name, real))
            body.append("const %s v%s_imag = %s;"%(self.type_, name, imag))

        index = 0
        for real, imag in y:
            name = str(index)
            index += 1
            body.append("x_real[%s] = %s;"%(name, real))
            body.append("x_imag[%s] = %s;"%(name, imag))

        body = ["    "+line for line in body]
        
        code = ["void fft_%s(%s x_real[], %s x_imag[])"%(size, self.type_, self.type_), "{"] + body + ["}"]
        code = "\n".join(code)

        return code
 
with open("fft_64.h", "w") as outf:
    outf.write(StaticFFT().make_FFT(64, "float"))

