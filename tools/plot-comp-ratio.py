#!/usr/bin/python

import sys
import pylab
import matplotlib
import matplotlib.pyplot as p


def safe_div(a, b):
  NaN = float('nan')
  return a / b if b != 0 else NaN


# This program computes a head to head performance comparison between two
# tests. The usage of the program is:
#
#   cmd [-ylabel=LABEL] [-title=TITLE] f1.txt f2.txt
#
# The input files must have the same structure:
#
#   n m1 m2 ... m_n
#
# where n is the test size (always integral) and m1, m2, m_n measurements,
# which can be safely converted to doubles.


from optparse import OptionParser
parser = OptionParser()
parser.add_option("-o", "--output", dest="output")
parser.add_option("--ylabel", dest="ylabel")
parser.add_option("--title", dest="title")

(opts, args) = parser.parse_args()

data = []
for i in args:
  f = open(i)
  results = []
  for j in f.readlines():
    line = j.split()
    n = int(line[0])
    m = [float(i) for i in line[1:]]
    a = sum(m) / len(m)
    results += [[n] + m + [a]]
  data += [results]

assert len(data) == 2

sizes = [i[0] for i in data[0]]
avg1 = [i[-1] for i in data[0]]
avg2 = [i[-1] for i in data[1]]

# Compare the performance of the f1 vs. f2.
comp = [safe_div(avg1[i], avg2[i]) for i in range(len(sizes))]

# Generate the plot
p.plot(comp)
p.xlabel('Data size')
if opts.ylabel:
  p.ylabel(opts.ylabel)
if opts.title:
  p.title(opts.title)


# How are we going to write the image to disk? Look at the file extension and
# decide which format to save in.
#
# If there isn't an output file specified, then it's not really safe to guess
# on the output, so we'll just quit. Should that be an error?
out = opts.output
if out:
  p.savefig(out)

# pylab.show()
