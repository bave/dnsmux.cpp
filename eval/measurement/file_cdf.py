#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys
from pylab import *

def psum(x):
    x.sort()
    y = []
    step = float(100)/len(x)
    for i in range(1, len(x)+1):
        y.append(i*step)
    return y


def name(dir_n, file_n):
    dir  = [ "./nodelay/", "./0.25ms/", "./0.75ms/", "./0.5ms/", "./1ms/", "./name_pkt/"]
    file = [ "1q1a", "1q1a(tfo)", "udp", "multi"]
    return dir[dir_n] + file[file_n]

def open_x(dir_n, file_n):
    dir  = [ "./nodelay/", "./0.25ms/", "./0.75ms/", "./0.5ms/", "./1ms/", "./name_pkt/"]
    file = [ "1q1a", "1q1a(tfo)", "udp", "multi"]
    filename = dir[dir_n] + file[file_n]
    x = []
    for line in open(filename, 'r'):
        x.append( float(line.replace('\n', '')) * 1000000 )
    return x

if len(sys.argv) != 3:
    print "python file_cdf.py dir_no file_no"
    sys.exit(1);


x = open_x(int(sys.argv[1]), int(sys.argv[2]))
y = psum(x)


rcParams['figure.facecolor'] = 'w'
rcParams['legend.numpoints'] = 2
rcParams['font.size'] = 12
rcParams['lines.linewidth'] = 2

plot(x, y, 'r')

#v = [0, 800, 0, 100]
#axis(v)

filename = name(int(sys.argv[1]), int(sys.argv[2]))
savename = filename.replace('/', '').replace('.', '')

print filename

title(filename, fontsize=14, fontname='serif')

xlabel("[micro second]")
ylabel("[percentage]")
#legend()

savefig('png/' + savename + '.png')
savefig('eps/' + savename + '.eps')
#show()

