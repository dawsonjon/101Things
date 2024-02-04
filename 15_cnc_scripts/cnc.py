#  _  ___  _   _____ _     _                 
# / |/ _ \/ | |_   _| |__ (_)_ __   __ _ ___ 
# | | | | | |   | | | '_ \| | '_ \ / _` / __|
# | | |_| | |   | | | | | | | | | | (_| \__ \
# |_|\___/|_|   |_| |_| |_|_|_| |_|\__, |___/
#                                  |___/    
#
# Copyright (c) Jonathan P Dawson 2024
# filename: cnc.py
# description:
# License: MIT
#

import numpy as np
from matplotlib import pyplot as plt

tool_radius = 6.0
safe_height = 3.0
from numpy.linalg import solve

def quadratic_through_points(x1, y1, x2, y2, x3, y3):
    # Create a system of three equations using the three points
    A = np.array([
        [x1**2, x1, 1],
        [x2**2, x2, 1],
        [x3**2, x3, 1]
    ])

    B = np.array([y1, y2, y3])

    # Solve the system of equations to get the coefficients a, b, and c
    coefficients = solve(A, B)

    a, b, c = coefficients

    # Return the coefficients
    return a, b, c

def display_moves(moves):
  ax = plt.figure().add_subplot(projection='3d')
  x = [x for x, y, z in moves]
  y = [y for x, y, z in moves]
  z = [z for x, y, z in moves]
  ax.plot3D(x, y, z)
  plt.show()

def carve_neck(radius_1, radius_2, width_1, width_2, easement_1, easement_2, length, num_passes):
  curve1_x = np.linspace(-width_1/2-tool_radius, width_1/2+tool_radius, num_passes) 
  curve2_x = np.linspace(-width_2/2-tool_radius, width_2/2+tool_radius, num_passes) 
  a, b, c = quadratic_through_points(0, 0, -width_1/2, -easement_1, width_1/2, -easement_1)
  curve1_y = (a * curve1_x**2) + (b * curve1_x) + c
  a, b, c = quadratic_through_points(0, length, -width_2/2, length+easement_2, width_2/2, length+easement_2)
  curve2_y = (a * curve2_x**2) + (b * curve2_x) + c
  curve1_z = -radius_1 -tool_radius + np.sqrt((radius_1+tool_radius)**2 - curve1_x**2)
  curve2_z = -radius_2 -tool_radius + np.sqrt((radius_2+tool_radius)**2 - curve2_x**2)
  drop = curve1_z[0] - curve2_z[0]
  curve2_z -= drop

  fig, ax = plt.subplots()
  ax.plot(curve1_x, curve1_y)
  ax.plot(curve2_x, curve2_y)
  plt.show()

  fig, ax = plt.subplots()
  ax.plot(curve1_x, curve1_z)
  ax.plot(curve2_x, curve2_z)
  for x, y in zip(curve1_x, curve1_z):
    c = plt.Circle((x, y), tool_radius, fill=False)
    ax.add_patch(c)
  for x, y in zip(curve2_x, curve2_z):
    c = plt.Circle((x, y), tool_radius, fill=False)
    ax.add_patch(c)
  plt.show()

  #move to stating point
  lines = [(curve1_x[0], curve1_y[0], safe_height),
           (curve1_x[0], curve1_y[0], curve1_z[0])]
  old_x = curve1_x[0]
  old_y = curve1_y[0]
  old_z = curve1_z[0]
  fwd=True
  for x1, x2, y1, y2, z1, z2 in zip(curve1_x, curve2_x, curve1_y, curve2_y, curve1_z, curve2_z):

    #move to other end of fretboard
    if fwd:
      fwd=False
      #if moving down hill, move sideways before lowering cutter
      if old_z > z1:
        lines.append((x1, old_y, old_z))
        lines.append((x1, y1, old_z))
        lines.append((x1, y1, z1))
      #if moving up hill, lift cutter before moving sideways
      else:
        lines.append((old_x, old_y, z1))
        lines.append((old_x, y1, z1))
        lines.append((x1, y1, z1))
      lines.append((x2, y2, z2))
      old_x, old_y, old_z = x2, y2, z2
    else:
      fwd=True
      #if moving down hill, move sideways before lowering cutter
      if old_z > z2:
        lines.append((x2, old_y, old_z))
        lines.append((x2, y2, old_z))
        lines.append((x2, y2, z2))
      #if moving up hill, lift cutter before moving sideways
      else:
        lines.append((old_x, old_y, z2))
        lines.append((old_x, y2, z2))
        lines.append((x2, y2, z2))
      lines.append((x1, y1, z1))
      old_x, old_y, old_z = x1, y1, z1

  return lines

def carve_fingerboard(radius_1, radius_2, width_1, width_2, length, num_passes):
  curve1_x = np.linspace(-width_1/2, width_1/2, num_passes) 
  curve2_x = np.linspace(-width_2/2, width_2/2, num_passes) 
  curve1_z = -radius_1 -tool_radius + np.sqrt((radius_1+tool_radius)**2 - curve1_x**2)
  curve2_z = -radius_2 -tool_radius + np.sqrt((radius_2+tool_radius)**2 - curve2_x**2)

  fig, ax = plt.subplots()
  ax.plot(curve1_x, curve1_z)
  ax.plot(curve2_x, curve2_z)
  for x, y in zip(curve1_x, curve1_z):
    c = plt.Circle((x, y), tool_radius, fill=False)
    ax.add_patch(c)
  for x, y in zip(curve2_x, curve2_z):
    c = plt.Circle((x, y), tool_radius, fill=False)
    ax.add_patch(c)
  plt.show()

  y = 0
  #move to stating point
  lines = [(curve1_x[0], y, safe_height),
           (curve1_x[0], y, curve1_z[0])]
  old_x = curve1_x[0]
  old_z = curve1_z[0]
  fwd=True
  for x1, x2, z1, z2 in zip(curve1_x, curve2_x, curve1_z, curve2_z):

    #move to other end of fretboard
    if fwd:
      fwd=False
      #if moving down hill, move sideways before lowering cutter
      if old_z > z1:
        lines.append((x1, 0, old_z))
        lines.append((x1, 0, z1))
      #if moving up hill, lift cutter before moving sideways
      else:
        lines.append((old_x, 0, z1))
        lines.append((x1, 0, z1))
      lines.append((x2, length, z2))
      old_x, old_z = x2, z2
    else:
      fwd=True
      #if moving down hill, move sideways before lowering cutter
      if old_z > z2:
        lines.append((x2, length, old_z))
        lines.append((x2, length, z2))
      #if moving up hill, lift cutter before moving sideways
      else:
        lines.append((old_x, length, z2))
        lines.append((x2, length, z2))
      lines.append((x1, 0, z1))
      old_x, old_z = x1, z1

  return lines

  
#lines = carve_fretboard(292, 254, 55, 40, 380, 20)
lines = carve_neck(30, 25, 55, 40, 50, 30, 380, 20)
display_moves(lines)
