bw = 2 + 2 + 2 + 1
cw = 7 + bw
ch = 5 + bw
path = '../../resources/images'

def setup():
    size(144, 168)
    background(0, 0, 0)
    stroke(0xff)
    noSmooth()
    
def draw():
    ww = cw * 7 + 1
    left = (width - ww) >> 1
    right = width - left - 1
    top = ch * 1.5
    y = top
    bottom = y
    while y < height - ch:
        line(left, y, right - 1, y)
        bottom = y
        y += ch
    for x in range(left, right + 1, cw):
        line(x, top, x, bottom)
        
    save(path + '/background.png')
