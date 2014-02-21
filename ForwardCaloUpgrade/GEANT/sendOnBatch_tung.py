#! /usr/bin/env python
import os
import sys
import time
import re


if (len(sys.argv) != 5) and (len(sys.argv) != 6) and (len(sys.argv) != 7):
    print "usage sendOnBatch.py batchName nLayers activeLayerThickness absorberLayerThickness transverseCellSize[default=20mm] impactPosition[=0mm]"
    sys.exit(1)


batchName = str(sys.argv[1])
nLayers = int(sys.argv[2])
activeLayerThickness = int(sys.argv[3])
absorberLayerThickness = int(sys.argv[4])
transverseCellSize = 20
if len(sys.argv) > 5:
    transverseCellSize = sys.argv[5]
impactPosition = 0
if len(sys.argv) > 6:
    impactPosition = sys.argv[6]


#queue = "1nd"
queue = "8nh"

diskoutputdir = "/cmsrm/pc24_2/pandolf/geant/EcalEndcaps/750MeV/" + batchName

dir = "/afs/cern.ch/work/p/pandolf/geant4/ForwardCaloUpgrade/GEANT/batchOutput_750MeV_" + batchName
os.system("mkdir -p "+dir)
os.system("mkdir -p "+dir+"/log/")
os.system("mkdir -p "+dir+"/inputfiles/")
os.system("mkdir -p "+dir+"/src/")
os.system("mkdir -p "+dir+"/rootfiles/temp/")
os.system("mkdir -p "+dir+"/tables/")



if diskoutputdir != "none": 
    os.system("ssh -o BatchMode=yes -o StrictHostKeyChecking=no pccmsrm24 mkdir -p "+diskoutputdir)
    os.system("ssh -o BatchMode=yes -o StrictHostKeyChecking=no pccmsrm24 mkdir -p "+diskoutputdir+"/rootfiles/")
    os.system("ssh -o BatchMode=yes -o StrictHostKeyChecking=no pccmsrm24 mkdir -p "+diskoutputdir+"/tables/")


pwd = os.environ['PWD']

suffix = "n"+ str(nLayers) + "_act" + str(activeLayerThickness) + "_abs" + str(absorberLayerThickness) + "_trasv" + str(transverseCellSize)
if (int(impactPosition) > 0) :
    suffix = suffix + "_impact" + str(impactPosition)


# prepare the script to run
outputname = dir+"/src/submit_" + suffix + ".src"
outputfile = open(outputname,'w')
outputfile.write('#!/bin/bash\n')
outputfile.write('export LANGUAGE=C\n')
outputfile.write('export LC_ALL=C\n')
outputfile.write('cd /afs/cern.ch/work/p/pandolf/geant4/ForwardCaloUpgrade/GEANT/\n')
outputfile.write('source newconfig.sh\n')
#outputfile.write('source /afs/cern.ch/sw/lcg/app/releases/ROOT/5.34.08/x86_64-slc6-gcc46-opt/root/bin/thisroot.sh\n')
#outputfile.write('cd -\n')
#outputfile.write('cd $WORKDIR\n')
outputfile.write( "bash "+ pwd + "/scripts/simulation_750MeV_tung_batch.sh full " + str(nLayers)+ " " + str(activeLayerThickness) + " " + str(absorberLayerThickness) + " " + str(transverseCellSize) + " " + str(impactPosition) + "\n")
#outputfile.write('ls rootfiles/samplinghistos_' + suffix + '*.root | xargs -i scp -o BatchMode=yes -o StrictHostKeyChecking=no {} pccmsrm24:'+diskoutputdir+'/rootfiles/{}\n') 
#outputfile.write('ls tables/parameters_' + suffix + '*.dat | xargs -i scp -o BatchMode=yes -o StrictHostKeyChecking=no {} pccmsrm24:'+diskoutputdir+'/tables/{}\n') 
outputfile.write('ls rootfiles/samplinghistos_' + suffix + '*.root | xargs -i cp {} '+dir+'/{}\n') 
outputfile.write('ls rootfiles/temp/temp_' + suffix + '*.root | xargs -i cp {} '+dir+'/{}\n') 
outputfile.write('ls tables/parameters_' + suffix + '*.dat | xargs -i cp {} '+dir+'/{}\n') 
outputfile.write('ls inputfiles/tempinput_' + suffix + '*.in | xargs -i cp {} '+dir+'/{}\n') 
outputfile.close
os.system("echo bsub -q "+queue+" -o "+dir+"/log/log_"+suffix+".log source "+outputname)
os.system("bsub -q "+queue+" -o "+dir+"/log/log_"+suffix+".log source "+outputname+" -copyInput= "+suffix)

