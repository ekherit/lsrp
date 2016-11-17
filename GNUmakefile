# $Id: GNUmakefile,v 1.2 2000-10-19 12:22:10 stanaka Exp $
# --------------------------------------------------------------
# GNUmakefile for examples module.  Gabriele Cosmo, 06/04/98.
# --------------------------------------------------------------

name := lsrp
G4TARGET := $(name)
G4EXLIB := true
CPPFLAGS := -fPIC -std=c++11 `root-config --cflags` -I./  -I$(WORKDIR)
LDFLAGS :=  `root-config --libs`  \
			-lboost_program_options \
		#	-lMinuit  \

ifndef G4INSTALL
  G4INSTALL = ../../../..
endif


.PHONY: all

TARGET = RootEvent

all: lib$(TARGET).so lib bin 


lib$(TARGET).so : $(TARGET).o $(TARGET)Dict.o
		g++ -o $@ -shared -fPIC  $^ $(LDFLAGS) 

$(TARGET).o : src/$(TARGET).cc
		g++ -o $@ -I/$(G4INCLUDE)  $(CPPFLAGS)  -c $<

$(TARGET)Dict.o : src/$(TARGET)Dict.cc
		g++ -o $@ -I/$(G4INCLUDE) $(CPPFLAGS)  -c $< 

src/$(TARGET)Dict.cc : include/$(TARGET).hh include/$(TARGET)LinkDef.hh
		rootcling -f $@ -I$(G4INCLUDE) -c $^



myclean :
			rm -rf dict.* $(G4TMP)/$(G4SYSTEM)/lsrp *.o *.so src/$(TARGET)Dict.cc src/*.pcm   *.pcm

include $(G4INSTALL)/config/binmake.gmk

visclean:
	rm -f g4*.prim g4*.eps g4*.wrl
	rm -f .DAWN_*

