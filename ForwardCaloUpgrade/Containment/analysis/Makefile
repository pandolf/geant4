CC = g++
CFLAGS = -Wall -c -g


INCLUDES = -I. -I$(ROOTSYS)/include  -I$(BASEDIR) -I$(BASEDIR)/CommonTools -I$(BASEDIR)/UserCode/pandolf/ -I$(BASEDIR)/CondFormats/JetMETObjects/interface

ROOTSYS ?= ERROR_RootSysIsNotDefined

ROOTFLAG = `${ROOTSYS}/bin/root-config --cflags --libs`

EXTRALIBS := -L$(ROOTSYS)/lib -L$(BASEDIR)/lib/slc5_amd64_gcc462/ -lHtml -lMathCore -lGenVector -lMinuit 




drawCeF3_vs_LYSO: drawCeF3_vs_LYSO.cpp 
	$(CC) -Wall $(INCLUDES) -o drawCeF3_vs_LYSO drawCeF3_vs_LYSO.cpp $(ROOTFLAG) $(EXTRALIBS)




clean:
	rm *.o


