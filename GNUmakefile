# $Id: GNUmakefile,v 1.2 2000-10-19 12:22:10 stanaka Exp $
# --------------------------------------------------------------
# GNUmakefile for examples module.  Gabriele Cosmo, 06/04/98.
# --------------------------------------------------------------

name := lsrp
G4TARGET := $(name)
G4EXLIB := true
CPPFLAGS := -std=c++11 `root-config --cflags` -I./ -I$(HOME)/work -I$(WORKDIR)  
LDFLAGS := \
			`root-config --libs` -lMinuit  \
			-L/usr/lib \
			-lboost_program_options \
			src/RootEventDict.cc \

ifndef G4INSTALL
  G4INSTALL = ../../../..
endif

.PHONY: all
all: RootEventDict.cc lib bin 

RootEventDict.cc :
			rootcint -f src/RootEventDict.cc -c include/RootEvent.hh include/RootLinkDef.hh

myclean :
			rm -rf dict.* $(G4TMP)/$(G4SYSTEM)/lsrp

include $(G4INSTALL)/config/binmake.gmk

visclean:
	rm -f g4*.prim g4*.eps g4*.wrl
	rm -f .DAWN_*

