#! /usr/bin/env python
import os
import sys
import time
import re


if (len(sys.argv) != 5):
    print "usage sendOnBatch.py batchName nLayers activeLayerThickness absorberLayerThickness"
    sys.exit(1)


batchName = str(sys.argv[1])
nLayers = int(sys.argv[2])
activeLayerThickness = int(sys.argv[3])
absorberLayerThickness = int(sys.argv[4])


queue = "8nh"

diskoutputdir = "/cmsrm/pc24_2/pandolf/geant/EcalEndcaps/750MeV/" + batchName

dir = "batchDir_750MeV_" + batchName
os.system("mkdir -p "+dir)
os.system("mkdir -p "+dir+"/log/")
os.system("mkdir -p "+dir+"/input/")
os.system("mkdir -p "+dir+"/src/")



if diskoutputdir != "none": 
    os.system("ssh -o BatchMode=yes -o StrictHostKeyChecking=no pccmsrm24 mkdir -p "+diskoutputdir)
    os.system("ssh -o BatchMode=yes -o StrictHostKeyChecking=no pccmsrm24 mkdir -p "+diskoutputdir+"/rootfiles/")
    os.system("ssh -o BatchMode=yes -o StrictHostKeyChecking=no pccmsrm24 mkdir -p "+diskoutputdir+"/tables/")


pwd = os.environ['PWD']

suffix = "n"+ str(nLayers) + "_act" + str(activeLayerThickness) + "_abs" + str(absorberLayerThickness)

# prepare the script to run
outputname = dir+"/src/submit_n" + str(nLayers) + "_act" + str(activeLayerThickness) + "_abs" + str(absorberLayerThickness) + ".src"
outputfile = open(outputname,'w')
outputfile.write('#!/bin/tcsh\n')
outputfile.write('setenv LANGUAGE C\n')
outputfile.write('setenv LC_ALL C\n')
outputfile.write('cd /afs/cern.ch/work/p/pandolf/geant4/ForwardCaloUpgrade/GEANT/; source newconfig.csh ; cd -\n')
outputfile.write('cd $WORKDIR\n')
outputfile.write("./scripts/simulation_750MeV_batch.sh full " + str(nLayers)+ " " + str(activeLayerThickness) + " " + str(absorberLayerThickness) + "\n")
outputfile.write('ls rootfiles/samplinghistos_' + suffix + '*.root | xargs -i scp -o BatchMode=yes -o StrictHostKeyChecking=no {} pccmsrm24:'+diskoutputdir+'/rootfiles/{}\n') 
outputfile.write('ls tables/parameters_' + suffix + '*.dat | xargs -i scp -o BatchMode=yes -o StrictHostKeyChecking=no {} pccmsrm24:'+diskoutputdir+'/tables/{}\n') 
outputfile.close
os.system("echo bsub -q "+queue+" -o "+dir+"/log/log_"+suffix+".log source "+pwd+"/"+outputname)
os.system("bsub -q "+queue+" -o "+dir+"/log/log_"+suffix+".log source "+pwd+"/"+outputname+" -copyInput= "+suffix)

