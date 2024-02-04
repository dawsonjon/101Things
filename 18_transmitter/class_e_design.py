#  _  ___  _   _____ _     _                 
# / |/ _ \/ | |_   _| |__ (_)_ __   __ _ ___ 
# | | | | | |   | | | '_ \| | '_ \ / _` / __|
# | | |_| | |   | | | | | | | | | | (_| \__ \
# |_|\___/|_|   |_| |_| |_|_|_| |_|\__, |___/
#                                  |___/    
#
# Copyright (c) Jonathan P Dawson 2024
# filename: class_e_design.py
# description:
# License: MIT
#

"""Design Class-E Amplifier using standard Capacitor Values and torroid wound inductors"""

from math import sqrt, log10, floor, pi

#key parameters for FDT86256
RDSon_ohms = 0.912
Coss_pF = 8
output_power = 5
supply_voltage = 12

#Standard Capacitor Values
standard_values = [1.0, 1.2, 1.5, 1.8, 2.2, 2.7, 3.3, 3.9, 4.7, 5.6, 6.8, 8.2]
for i in range(3):
  standard_values += [i*10 for i in standard_values]
standard_values += [0]

def calculate_class_e(psu_voltage, output_power, centre_frequency_Hz):

# Class E Amplifier Design
#
#       o VDD
#       |
#       |
#     [ L1 ]
#       |
#       |               
#       +---------+-----[ L2 ]----[ C2 ]----o
#       |         |                          
#     |-+         |                          
#  o--|         [ C1 ]              [ Load Impedance ]
#     |-+         |                          
#       |         |                          
#       +---------+-------------------------o
#       | 
#       o GND     
  
  load_impedance_ohms = 0.32*psu_voltage*psu_voltage/output_power
  load_impedance_ohms -= 1.5*RDSon_ohms
  C1_pF = (1e12*0.19)/(2.0*pi*centre_frequency_Hz*load_impedance_ohms)
  C1_pF -= Coss_pF
  C1_pF = nearest_capacitor(C1_pF)
  C2_pF = 1e12/(2.0*pi*centre_frequency_Hz*load_impedance_ohms*1.5) 
  C2_pF = nearest_capacitor(C2_pF)
  L2_uH = (1.8*load_impedance_ohms)+1.0 / (2*pi*centre_frequency_Hz*C2_pF/1e12)
  L2_uH = 1e6*L2_uH/(2.0*pi*centre_frequency_Hz)
  L2_windings = calculate_inductor_turns(L2_uH)
  L1_uH = 15*L2_uH
  L1_windings = calculate_inductor_turns(L1_uH)

# Pi Impedance Match and 2nd Harmonic Notch
#
#                            +---[ C4 ]---+
#                            |            |
#         o----------------+-+---[ L3 ]---+-+------------o
#                          |                |
#                          |                |
#  [ Load Impedance ]   [ C3 ]           [ C5 ]       [50 Ohm]
#                          |                |
#                          |                |
#         o----------------+----------------+------------o
  

  output_impedance_ohms = 50
  C3_pF = 1e12/(2.0*pi*centre_frequency_Hz*sqrt(load_impedance_ohms*output_impedance_ohms))
  C3_pF = nearest_capacitor(C3_pF)
  C5_pF = C3_pF
  L3_uH = 1e6*0.75*sqrt(load_impedance_ohms*output_impedance_ohms)/(2.0*pi*centre_frequency_Hz)
  L3_windings = calculate_inductor_turns(L3_uH)
  C4_pF = C5_pF/3.0
  C4_pF = nearest_capacitor(C4_pF)

  component_values = C1_pF, C2_pF, C3_pF, C4_pF, C5_pF, L1_uH, L1_windings, L2_uH, L2_windings, L3_uH, L3_windings
  return component_values

def nearest_capacitor(desired_value):
  best = (1000, None, None, None, None)
  for c1 in standard_values:
    actual_value = c1
    error = abs(desired_value - actual_value)
    if error < best[0]:
      best = (error, c1, actual_value, desired_value)
  error, c1, actual_value, desired_value = best
  return actual_value

def calculate_inductor_turns(inductance_uH):
  AL = 3
  turns_AL3 = round(sqrt((1000 * inductance_uH)/AL))
  AL = 4
  turns_AL4 = round(sqrt((1000 * inductance_uH)/AL))
  AL = 55
  turns_AL55 = round(sqrt((1000 * inductance_uH)/AL))
  AL = 350
  turns_AL350 = round(sqrt((1000 * inductance_uH)/AL))

  if turns_AL350 > 10:
    windings = "%uT FT37-43"%turns_AL350
  elif turns_AL55 > 10:
    windings = "%uT FT37-61"%turns_AL55
  elif turns_AL4 > 10:
    windings = "%uT T37-2"%turns_AL4
  else:
    windings = "%uT T37-6"%turns_AL3

  return windings

bands = [
("10m", 28.85e6),
("12m", 24.94e6),
("15m", 21.225e6),
("17m", 18.118e6),
("20m", 14.175e6),
("30m", 10.125e6),
("40m", 7.1e6),
("60m", 5e6),
("80m", 3.65e6),
("160m", 1.9e6)
]

diagram = """Class-E Amplifier Design
========================

Supply Voltage: %uV
Output Power: %uW
RDSon: %6.3fohms
Coss: %3.1fpF

       o VDD     Class-E Amplifier        |  Impedance Match and Harmonic Notch
       |                                  |
       |                                  |
     [ L1 ]                               | 
       |                                  |        +---[ C4 ]---+
       |                                  |        |            |
       +---------+-----[ L2 ]----[ C2 ]-- | -----+-+---[ L3 ]---+-+------------o
       | d       |                        |      |                |                
   g |-+         |                        |      |                |                
  o--| Q1      [ C1 ]                     |   [ C3 ]           [ C5 ]       [50 Ohm]
     |-+         |                        |      |                |                
       | s       |                        |      |                |                
       +---------+----------------------- | -----+----------------+------------o
       |                                  |
       o GND                              |

Based on equations found in:

1. http://www.wa0itp.com/class%%20e%%20design.html
2. Cripe, David, NMØS, "Class E Power Amplifiers for QRP"
   QRP Quarterly Vol 50 Number 3 Summer 2009, pp 32-37 
   Errata: Volume 50 Number 4 Fall 2009 p4

"""%(supply_voltage, output_power, RDSon_ohms, Coss_pF)
print(diagram)
print("  Band             C1     C2     C3     C4     C5       L1                   L2                   L3")
print("====================================================================================================================")
parts = []
for band, frequency in bands:
  values = (band, frequency/1e6)
  values += calculate_class_e(supply_voltage, output_power, frequency)
  text_format = "%5s %6.3fMHz "
  text_format += "%5upF"*5
  text_format += " %6.2fuH %11s"*3
  print(text_format%values)
print("====================================================================================================================")
