import cairo
import numpy as np
from matplotlib import pyplot as plt
import copy

bignum = 1000000
scale = 10

class Text:
  def __init__(self, diagram, point, text):
    diagram.blocks.append(self)
    x, y = point
    self.x = x
    self.y = y
    self.text = text

  def render(self, context):
    context.set_line_width(1)
    context.move_to(scale * self.x, scale * (self.y-0.5) )
    context.set_font_size(scale)
    context.show_text(self.text)
    context.set_source_rgba(0.0, 0.0, 0.0, 1.0)
    context.stroke()

class Block:
  def __init__(self, diagram, x, y):
    diagram.blocks.append(self)
    self.x = x
    self.y = y

    self.arrows = []

    for i in range(3):
      for j in range(3):
        diagram.obstacles.append((x+i, y+j))
    
  def origin(self):
    return self.x, self.y

  def top(self):
    return self.x+1, self.y

  def bottom(self):
    return self.x+1, self.y+2

  def left(self):
    return self.x, self.y+1

  def right(self):
    return self.x+2, self.y+1

  def arrow(self, endpoint):
    if endpoint == self.top():
      direction = "down"
    elif endpoint == self.bottom():
      direction = "up"
    elif endpoint == self.left():
      direction = "right"
    elif endpoint == self.right():
      direction = "left"
    self.arrows.append((endpoint, direction))

  def render(self, context):
    context.set_line_width(1)
    context.move_to(scale * self.x,     scale * self.y)
    context.line_to(scale * (self.x+2), scale * self.y)
    context.line_to(scale * (self.x+2), scale * (self.y+2))
    context.line_to(scale * self.x,     scale * (self.y+2))
    context.line_to(scale * self.x,     scale * self.y)
    context.set_source_rgba(124/256, 224/256, 174/256, 1.0)
    context.fill_preserve()
    context.set_source_rgba(0.0, 0.0, 0.0, 1.0)
    context.stroke()

  def render_arrows(self, context):
    for endpoint, direction in self.arrows:
      x, y = endpoint
      context.set_line_width(1)
      context.set_source_rgba(0.0, 0.0, 0.0, 1.0)
      if direction == "up":
        context.move_to(scale * (x-0.25), scale * (y+0.25))
        context.line_to(scale * x, scale * y)
        context.move_to(scale * (x+0.25), scale * (y+0.25))
        context.line_to(scale * x, scale * y)
      if direction == "down":
        context.move_to(scale * (x-0.25), scale * (y-0.25))
        context.line_to(scale * x, scale * y)
        context.move_to(scale * (x+0.25), scale * (y-0.25))
        context.line_to(scale * x, scale * y)
      if direction == "left":
        context.move_to(scale * (x+0.25), scale * (y-0.25))
        context.line_to(scale * x, scale * y)
        context.move_to(scale * (x+0.25), scale * (y+0.25))
        context.line_to(scale * x, scale * y)
      if direction == "right":
        context.move_to(scale * (x-0.25), scale * (y-0.25))
        context.line_to(scale * x, scale * y)
        context.move_to(scale * (x-0.25), scale * (y+0.25))
        context.line_to(scale * x, scale * y)
      context.stroke()
        
class GainBlock(Block):
  def render(self, context):
    context.set_line_width(1)
    context.move_to(scale * self.x,     scale * self.y)
    context.line_to(scale * (self.x+2), scale * (self.y+1))
    context.line_to(scale * self.x,     scale * (self.y+2))
    context.line_to(scale * self.x,     scale * self.y)

    context.set_source_rgba(124/256, 224/256, 174/256, 1.0)
    context.fill_preserve()
    context.set_source_rgba(0.0, 0.0, 0.0, 1.0)
    context.stroke()

    self.render_arrows(context)

class RGainBlock(Block):
  def render(self, context):

    context.set_line_width(1)
    context.move_to(scale * (self.x+2), scale * self.y)
    context.line_to(scale * self.x,     scale * (self.y+1))
    context.line_to(scale * (self.x+2), scale * (self.y+2))
    context.line_to(scale * (self.x+2), scale * self.y)

    context.set_source_rgba(124/256, 224/256, 174/256, 1.0)
    context.fill_preserve()
    context.set_source_rgba(0.0, 0.0, 0.0, 1.0)
    context.stroke()

    self.render_arrows(context)

class CircularBlock(Block):
  def render(self, context):
    context.set_line_width(1)
    context.arc(scale * (self.x+1), scale * (self.y+1), scale, 0, np.pi*2)
    context.set_source_rgba(124/256, 224/256, 174/256, 1.0)
    context.fill_preserve()
    context.set_source_rgba(0.0, 0.0, 0.0, 1.0)
    context.stroke()

    self.render_arrows(context)

class TextBlock(Block):
  def __init__(self, d, x, y, text):
    Block.__init__(self, d, x, y)
    self.text = text

  def render(self, context):
    Block.render(self, context)

    #reduce font size if necassary
    context.set_font_size(scale)
    xbearing, ybearing, width, height, dx, dy = context.text_extents(self.text)
    if width > (2*scale-5):
      font_size = scale * (2*scale-5)/width
    else:
      font_size = scale

    context.set_font_size(font_size)
    xbearing, ybearing, width, height, dx, dy = context.text_extents(self.text)
    x_space = (2*scale) - width
    y_space = (2*scale) - height

    context.move_to((scale * self.x) + x_space/2,  (scale*(self.y+1)) + height/2 )
    context.show_text(self.text)
    context.set_source_rgba(0.0, 0.0, 0.0, 1.0)
    context.stroke()

class Mixer(CircularBlock):
  def render(self, context):
    CircularBlock.render(self, context)
    context.move_to(scale * (self.x+0.5),  scale * (self.y+0.5))
    context.line_to(scale * (self.x+1.5),  scale * (self.y+1.5))
    context.move_to(scale * (self.x+1.5),  scale * (self.y+0.5))
    context.line_to(scale * (self.x+0.5),  scale * (self.y+1.5))
    context.set_source_rgba(0.0, 0.0, 0.0, 1.0)
    context.stroke()

class Adder(CircularBlock):
  def render(self, context):
    CircularBlock.render(self, context)
    context.move_to(scale * (self.x+0.5),  scale * (self.y+1))
    context.line_to(scale * (self.x+1.5),  scale * (self.y+1))
    context.move_to(scale * (self.x+1),  scale * (self.y+0.5))
    context.line_to(scale * (self.x+1),  scale * (self.y+1.5))
    context.set_source_rgba(0.0, 0.0, 0.0, 1.0)
    context.stroke()

class Oscillator(CircularBlock):
  def render(self, context):
    CircularBlock.render(self, context)
    context.move_to(scale * (self.x+1.0),  scale * (self.y+1))
    context.arc(scale * (self.x+0.75),  scale * (self.y+1), scale * 0.25, 0, np.pi)
    context.move_to(scale * (self.x+1.0),  scale * (self.y+1))
    context.arc(scale * (self.x+1.25),  scale * (self.y+1), scale * 0.25, np.pi, 0)
    context.set_source_rgba(0.0, 0.0, 0.0, 1.0)
    context.stroke()


class Join(Block):

  def __init__(self, diagram, x, y):
    diagram.blocks.append(self)
    self.x = x
    self.y = y
    diagram.obstacles.append((x+1, y+1))

  def centre(self):
    return self.x+1, self.y+1

  def render(self, context):
    context.set_line_width(1)
    context.move_to(scale * (self.x+1), scale * (self.y+1))
    context.arc(scale * (self.x+1), scale * (self.y+1), 2, 0, 2*np.pi)
    context.set_source_rgba(0.0, 0.0, 0.0, 1.0)
    context.fill()

class IO(Block):

  def render(self, context):
    context.set_line_width(1)
    context.move_to(scale * self.x,        scale * (self.y+1))
    context.line_to(scale * self.x,        scale * (self.y+1.25))
    context.line_to(scale * (self.x+1.75), scale * (self.y+1.25))
    context.line_to(scale * (self.x+2),    scale * (self.y+1))
    context.line_to(scale * (self.x+1.75), scale * (self.y+0.75))
    context.line_to(scale * self.x,        scale * (self.y+0.75))
    context.line_to(scale * self.x,        scale * (self.y+1))
    context.set_source_rgba(124/256, 224/256, 174/256, 1.0)
    context.fill_preserve()
    context.set_source_rgba(0.0, 0.0, 0.0, 1.0)
    context.stroke()


class Diagram:

  def __init__(self, width, height, title):
    self.width = width
    self.height = height
    self.costs = np.ones((width, height))*bignum
    self.obstacles = []
    self.paths = []
    self.blocks = []
    self.title = title

  def plot_costs(self):
    costs_to_plot = copy.copy(self.costs)
    costs_to_plot[costs_to_plot == bignum] = 0
    plt.imshow(costs_to_plot, cmap='hot', interpolation='nearest')
    plt.show()

  def connect(self, p1, p2, line_width = 1):
    x1, y1 = p1
    x2, y2 = p2
    self.costs = np.ones((self.width, self.height))*bignum
    if self.update_costs(x2, y2, (x1, y1)):
      #self.plot_costs()
      self.paths.append((line_width, self.route(x1, y1, x2, y2, "up")))
    else:
      print("failed to route")

  def update_costs(self, x, y, endpoint):
    cost = 0
    points_to_check = [(x, y)]
    while 1:
      next_points_to_check = []
      for x1, y1 in points_to_check:

        if (x1, y1) == endpoint:
          self.costs[x1][y1] = cost
          return True

        if ((x1, y1) not in self.obstacles) or ((x1, y1) == (x, y)):
          if cost < self.costs[x1][y1]:
            self.costs[x1][y1] = cost
            if x1+1 < self.width:
                next_points_to_check.append((x1+1, y1))
            if y1+1 < self.height:
                next_points_to_check.append((x1, y1+1))
            if x1 > 0:
                next_points_to_check.append((x1-1, y1))
            if y1 > 0:
                next_points_to_check.append((x1, y1-1))

      points_to_check = next_points_to_check
      cost += 1

      if not next_points_to_check:
        return False
      

  def route(self, x1, y1, x2, y2, preferred_direction):

    path = (x1, y1)
    if path not in self.obstacles:
      self.obstacles.append(path)
    if x1 == x2 and y1 == y2:
      return [path]

    above = bignum
    below = bignum
    left = bignum
    right = bignum

    if x1+1 < self.width:
        right = self.costs[x1+1][y1]
    if y1+1 < self.height:
        below = self.costs[x1][y1+1]
    if x1 > 0:
        left = self.costs[x1-1][y1]
    if y1 > 0:
        above = self.costs[x1][y1-1]

    direction = ["up", "down", "left", "right"][np.argmin([above, below, left, right])]

    #prefer to keep going in the same direction
    if preferred_direction == "up" and above == min([above, below, left, right]):
      direction = "up"
    if preferred_direction == "down" and below == min([above, below, left, right]):
      direction = "down"
    if preferred_direction == "left" and left == min([above, below, left, right]):
      direction = "left"
    if preferred_direction == "right" and right == min([above, below, left, right]):
      direction = "right"

    if direction == "right":
      return [path] + self.route(x1+1, y1, x2, y2, direction)
    elif direction == "down":
      return [path] + self.route(x1, y1+1, x2, y2, direction)
    elif direction == "left":
      return [path] + self.route(x1-1, y1, x2, y2, direction)
    elif direction == "up":
      return [path] + self.route(x1, y1-1, x2, y2, direction)

  def render(self, filename):
    with cairo.SVGSurface(filename, self.width*scale, self.height*scale) as surface:
        context = cairo.Context(surface)
        context.scale(1, 1)

        #show title
        context.set_font_size(scale)
        xbearing, ybearing, width, height, dx, dy = context.text_extents(self.title)
        x_space = self.width * scale - width

        context.move_to(x_space/2, 2*scale)
        context.set_font_size(10)
        context.show_text(self.title)
        context.set_source_rgba(0, 0, 0, 1)
        context.stroke()

        for line_width, path in self.paths:
          context.set_line_width(line_width)
          x, y = path[0]
          context.move_to(x*scale, y*scale)
          for x, y in path:
            context.line_to(x*scale, y*scale)
          context.stroke()

        for block in self.blocks:
          block.render(context)

