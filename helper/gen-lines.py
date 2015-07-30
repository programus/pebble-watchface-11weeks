import math

def get_points(w, h, n):
  l = []
  for s in xrange(n + 1):
    r = math.radians(s * 90. / n)
    t = math.tan(r)
    b = False
    x = h
    y = x * t
    if y > w:
      y = w
      x = y / t
      b = True
    l.append((b, y, x))
  return l

def get_lines(ox, oy, points):
  ret = []
  l = [
    (1, -1),
    (1, 1),
    (-1, 1),
    (-1, -1)
    ]
  ex = round(points[-1][1])
  ey = round(points[0][2])
  hist = [(0, -ey - 1), (ex, -ey - 1), (ex, ey), (-ex - 1, ey), (-ex - 1, -ey - 1)]
  stack = []
  for i, (a, b) in enumerate(l):
    for p in points:
      s = ''
      n = i + 2
      if p[0] != bool(i & 1):
        n += 1
      
      s = '%d, %s, 0x%04x' % (n, ', '.join(['0x%04x' % (hex_point(ox + x[0], oy + x[1]), ) for x in hist[0: n - 1]]), hex_point(ox + a * round(p[1]) + (a if a < 0 else 0), oy + b * round(p[2]) + (b if b < 0 else 0)))
      ret.append(s)
    points.reverse()
      
  return ret

def hex_point(x, y):
  return (int(x) << 8) | int(y)

def gen_minutes_lines():
  w = 71
  h = 83
  n = 900
  points = get_points(w, h, n)
  lines = get_lines(72, 84, points)
  print 'uint16_t min_lines[] = {\n\t%s\n};'% (',\n\t'.join(lines), )
  
  
def gen_hours_lines():
  w = 69
  h = 81
  n = 12 * 60 / 4
  points = get_points(w, h, n)
  lines = get_lines(72, 84, points)
  print 'uint16_t hour_lines[] = {\n\t%s\n};'% (',\n\t'.join(lines), )
  
if __name__ == '__main__':
  gen_minutes_lines()
  gen_hours_lines()
