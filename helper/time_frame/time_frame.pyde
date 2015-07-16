path = '../../resources/images'
import math

def setup():
    size(144, 168)
    background(0, 0, 0)
    stroke(0xff)
    noSmooth()
    
def get_points():
    xx = 83
    yy = 71
    l = []
    for s in xrange(0, 91, 6):
        r = math.radians(s)
        t = math.tan(r)
        x = xx
        y = x * t
        if y > yy:
            y = yy
            x = y / t
#        print s, x, y
        l.append((y, x))
    return l
    
def draw_points(points):
    for p in points:
        x, y = p
        line(0, 0, x, y)
    
def draw():
    points = get_points()
    translate(72, 84)
    draw_points(points)
#     rotate(PI)
#     draw_points(points)

    save(path + '/time-frame.png')
