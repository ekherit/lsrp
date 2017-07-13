#!/usr/bin/python3
# This program get input file as template and 
# duplicate command with parameter from begin to end  with a step
# and generate start run command (/run/beamOn) with the number of events

from argparse import *
#example
# ../variate.py  -N 10 --begin=1 --end=30 --step=1 --command="/lsrp/Converter/Width %f mm"  --format="%04.1f.root" run.mac

parser = ArgumentParser(description='')
parser.add_argument('file',type=str, help='input file')
parser.add_argument('-N', help='input file', type=int, default = 10)
parser.add_argument('--begin', help='begin', type=float, default = 0)
parser.add_argument('--end', help='end', type=float, default = 0)
parser.add_argument('--step', help='step', type=float, default = 1)
parser.add_argument('--command', help='command', type=str, default ='')
parser.add_argument('--prefix', help='prefix for output root files', type=str, default ='')
parser.add_argument('--format', help='format for root file', type=str, default ='%020.5f.root')

args = parser.parse_args()
#print args.N
#print args.file

f = open(args.file, 'r');
for line in f:
    print (line,end='')


x = args.begin
#for  x in xfrange(args.begin, args.end, args.step):
print ("")
while  x < args.end:
    rootfile = args.prefix+args.format % x
    print ("/lsrp/RootFile %s" % rootfile)
    print (args.command % x )
    print ("/run/beamOn ", args.N)
    print ("")
    x += args.step


