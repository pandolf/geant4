CC = g++
CFLAGS = -Wall -c -g


INCLUDES = -I. -I$(ROOTSYS)/include  -I$(BASEDIR) -I$(BASEDIR)/CommonTools -I$(BASEDIR)/UserCode/pandolf/ -I$(BASEDIR)/CondFormats/JetMETObjects/interface

ROOTSYS ?= ERROR_RootSysIsNotDefined

ROOTFLAG = `${ROOTSYS}/bin/root-config --cflags --libs`

EXTRALIBS := -L$(ROOTSYS)/lib -L$(BASEDIR)/lib/slc5_amd64_gcc462/ -lHtml -lMathCore -lGenVector -lMinuit -lEG -lRooFitCore -lRooFit




drawResolutionVsLength: drawResolutionVsLength.cpp DrawBase.o fitTools.o
	$(CC) -Wall $(INCLUDES) -o drawResolutionVsLength drawResolutionVsLength.cpp DrawBase.o fitTools.o $(ROOTFLAG) $(EXTRALIBS)

drawResolutionVsTrasv: drawResolutionVsTrasv.cpp DrawBase.o fitTools.o
	$(CC) -Wall $(INCLUDES) -o drawResolutionVsTrasv drawResolutionVsTrasv.cpp DrawBase.o fitTools.o $(ROOTFLAG) $(EXTRALIBS)

drawResolutionVsImpactPosition: drawResolutionVsImpactPosition.cpp DrawBase.o fitTools.o
	$(CC) -Wall $(INCLUDES) -o drawResolutionVsImpactPosition drawResolutionVsImpactPosition.cpp DrawBase.o fitTools.o $(ROOTFLAG) $(EXTRALIBS)

drawLeadVsTung: drawLeadVsTung.cpp DrawBase.o fitTools.o
	$(CC) -Wall $(INCLUDES) -o drawLeadVsTung drawLeadVsTung.cpp DrawBase.o fitTools.o $(ROOTFLAG) $(EXTRALIBS)

drawGaussian: drawGaussian.cpp DrawBase.o fitTools.o
	$(CC) -Wall $(INCLUDES) -o drawGaussian drawGaussian.cpp DrawBase.o fitTools.o $(ROOTFLAG) $(EXTRALIBS)

computeCost: computeCost.cpp DrawBase.o fitTools.o
	$(CC) -Wall $(INCLUDES) -o computeCost computeCost.cpp DrawBase.o fitTools.o $(ROOTFLAG) $(EXTRALIBS)






DrawBase.o: $(BASEDIR)/CommonTools/DrawBase.C fitTools.o
	$(CC) $(CFLAGS) $(INCLUDES) fitTools.o $(BASEDIR)/CommonTools/DrawBase.C $(ROOTFLAG) $(EXTRALIBS)

fitTools.o: $(BASEDIR)/CommonTools/fitTools.C
	$(CC) $(CFLAGS) $(INCLUDES) $(BASEDIR)/CommonTools/fitTools.C $(ROOTFLAG) $(EXTRALIBS)


clean:
	rm *.o


