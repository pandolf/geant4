#!/usr/bin/python

import sys
import os
import math
from optparse import OptionParser



parser = OptionParser()
parser.add_option("-l","--label",dest="batchName",default="testCost")

(options,args)=parser.parse_args()

if options.batchName=="testCost" :
  print("Please provide label through option -l or --label")
  sys.exit(1)


X0_tung = 3.5 # in mm
X0_cef3 = 16.8 # in mm

X0_target = 24.

detectorLength = 0.

tung_thickness = 2.5 # in mm, fixed
cef3_thickness = 1. # in mm, will vary


while ( detectorLength<220. ) :

  X0_tung_singleLayer = tung_thickness/X0_tung
  X0_cef3_singleLayer = cef3_thickness/X0_cef3

  nLayers = math.ceil( (( X0_target - X0_cef3_singleLayer ) / ( X0_cef3_singleLayer + X0_tung_singleLayer )) )

  X0_eff = (nLayers+1)*X0_cef3_singleLayer + nLayers*X0_tung_singleLayer

  detectorLength = ((nLayers+1)*cef3_thickness + nLayers*tung_thickness)

  print("python sendOnBatch2.py -e 1   -l " + str(options.batchName) + " -n " +str((int)(nLayers)) + " --act " + str(cef3_thickness) + " --abs " + str(tung_thickness) )
  os.system("python sendOnBatch2.py -e 1   -l " + str(options.batchName) + " -n " +str((int)(nLayers)) + " --act " + str(cef3_thickness) + " --abs " + str(tung_thickness) )
  print("python sendOnBatch2.py -e 5  -l " + str(options.batchName) + " -n " +str((int)(nLayers)) + " --act " + str(cef3_thickness) + " --abs " + str(tung_thickness) )
  os.system("python sendOnBatch2.py -e 5  -l " + str(options.batchName) + " -n " +str((int)(nLayers)) + " --act " + str(cef3_thickness) + " --abs " + str(tung_thickness) )
  print("python sendOnBatch2.py -e 10  -l " + str(options.batchName) + " -n " +str((int)(nLayers)) + " --act " + str(cef3_thickness) + " --abs " + str(tung_thickness) )
  os.system("python sendOnBatch2.py -e 10  -l " + str(options.batchName) + " -n " +str((int)(nLayers)) + " --act " + str(cef3_thickness) + " --abs " + str(tung_thickness) )
  #print("python sendOnBatch2.py -e 100 -l " + str(options.batchName) + " -n " +str((int)(nLayers)) + " --act " + str((int)(cef3_thickness)) + " --abs " + str((int)(tung_thickness)) )
  #os.system("python sendOnBatch2.py -e 100 -l " + str(options.batchName) + " -n " +str((int)(nLayers)) + " --act " + str((int)(cef3_thickness)) + " --abs " + str((int)(tung_thickness)) )


  cef3_thickness = cef3_thickness + 0.5
  #cef3_thickness = cef3_thickness + 0.5

