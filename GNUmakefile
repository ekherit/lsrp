name := lsrp
G4TARGET := $(name)
G4EXLIB := true
CPPFLAGS := -fPIC -std=c++1y `root-config --cflags` -I./  -I$(WORKDIR) -Wno-unused-variable -Wno-shadow
LDFLAGS :=  `root-config --libs`  \
			-lboost_program_options \
			-lboost_filesystem \
		#	-lMinuit  \

ifndef G4DUMMY_VARIABLE
  include $(G4INSTALL)/config/architecture.gmk
endif

TMP=$(G4TMP)/$(G4SYSTEM)/$(G4TARGET)

.PHONY: all


all: lib bin

EXTRALIBS := $(TMP)/Config.o $(TMP)/RootEvent.o
EXTRA_LINK_DEPENDENCIES :=  $(TMP)/libRoot.so 

$(TMP)/libRoot.so : $(TMP)/RootEvent.o $(TMP)/Config.o $(TMP)/RootDict.o 
		@g++ -o $@ -shared -fPIC  $^   $(LDFLAGS) 
		@echo Creating shared library $@

$(TMP)/RootDict.o : $(TMP)/RootDict.cc
		@g++ -o $@ -I$(G4INCLUDE) $(CPPFLAGS)  -c $< 
		@echo Compiling RootDict.cc

$(TMP)/RootDict.cc :  include/RootEvent.hh include/Config.hh include/LinkDef.hh
		@rootcling -f $@ -I$(G4INCLUDE) -c $^
		@echo Generating RootDict.cc

include $(G4INSTALL)/config/binmake.gmk


visclean:
	rm -f g4*.prim g4*.eps g4*.wrl
	rm -f .DAWN_*

