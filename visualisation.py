# Python visualisation for edge splits
# 
# For visualisation, print edges and watchtowers in the format of 
#   @E<edgeNum> <faceNum> <startX> <startY> <endX> <endY>
#   @W<faceNum> <X> <Y>
# 
# and pipe output to python script.
# 
# Setting edgeNum/faceNum to -1 disables colouring.

import sys
import matplotlib.pyplot as plt
import matplotlib

sys.stdin.reconfigure(encoding='utf-8-sig')
colors = ['orange', 'gold', 'lime', 'cyan', 'blue', 'indigo', 'violet']
matplotlib.use('qt5agg')

def getcolor(x):
    return 'black' if x == -1 else colors[x % len(colors)]

for line in sys.stdin:
    if line[0] != '@':
        print(line, end='')
    elif line[1] == 'W':
        n, *coord = line[2:].split()
        n, (x, y) = int(n), map(float, coord)

        plt.plot(x, y, marker='.', mfc=getcolor(n), mew=0, alpha=0.5, label=n)
        
    elif line[1] == 'E':
        n, f, *coord = line[2:].split()
        n, f, (x1, y1, x2, y2) = int(n), int(f), map(float, coord)
        dx, dy = x2-x1, y2-y1
        
        plt.plot(x1, y1, 'ko', alpha=0.5)
        plt.plot(x2, y2, 'ko', alpha=0.5)
        plt.arrow(x1, y1, dx, dy, length_includes_head=True, width=0.1, fc=getcolor(f), 
                  shape='left', label='1', alpha=0.7, ec=None)
        plt.annotate(n, (x1 + dx/2, y1 + dy/2), c='r')

plt.show()