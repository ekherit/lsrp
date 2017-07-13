name := lsrp
G4TARGET := $(name)
G4EXLIB := true
CPPFLAGS := -fPIC -std=c++1y `root-config --cflags` -I./  -I$(WORKDIR) -Wno-unused-variable -Wno-shadow
LDFLAGS :=  `root-config --libs`  \
			-lboost_program_options \
			-lboost_filesystem \
		#	-lMinuit  \

ifndef G4INSTALL
  G4INSTALL = ../../../..
endif

ifndef G4DUMMY_VARIABLE
  include $(G4INSTALL)/config/architecture.gmk
endif

TMP=$(G4TMP)/$(G4SYSTEM)/$(G4TARGET)

.PHONY: all


all: lib bin

myclean : clean
			rm -rf  *.o *.so *.pcm src/*Dict.cc  src/*Dict.h src/*.pcm

EXTRALIBS := $(TMP)/Config.o $(TMP)/RootEvent.o
EXTRA_LINK_DEPENDENCIES :=  $(TMP)/libRootEvent.so  $(TMP)/libConfig.so

$(TMP)/libRootEvent.so : $(TMP)/RootEvent.o $(TMP)/RootEventDict.o 
		@g++ -o $@ -shared -fPIC  $^   $(LDFLAGS) 
		@echo Creating shared library $@
		@mv -f src/*rdict.pcm $(TMP)/


#RootEvent.o, Config.o and RootEventDict.o ConfigDict.o automaticaly
#compiled by GENAT4  make script

#$(TMP)/RootEvent.o : src/RootEvent.cc
#		g++ -o $@ -I/$(G4INCLUDE)  $(CPPFLAGS)  -c $<

#$(TMP)/RootEventDict.o : src/RootEventDict.cc
#		@g++ -o $@ -I$(G4INCLUDE) $(CPPFLAGS)  -c $< 
#		@echo Compiling RootEventDict.cc

src/RootEventDict.cc :  include/RootEvent.hh include/RootEventLinkDef.hh
		@rootcling -f $@ -I$(G4INCLUDE) -c $^
		@echo Generating RootEventDict.cc

$(TMP)/libConfig.so : $(TMP)/Config.o  $(TMP)/ConfigDict.o 
		@g++ -o $@ -shared -fPIC  $^   $(LDFLAGS) 
		@echo Creating shared library $@
		@mv -f src/*rdict.pcm $(TMP)/

#$(TMP)/Config.o : src/Config.cc
#		g++ -o $@ -I/$(G4INCLUDE)  $(CPPFLAGS)  -c $<

#$(TMP)/ConfigDict.o : src/ConfigDict.cc
#		@g++ -o $@ -I$(G4INCLUDE) $(CPPFLAGS)  -c $< 
#		@echo Compiling ConfigDict.cc

src/ConfigDict.cc :  include/Config.hh include/ConfigLinkDef.hh
		@rootcling -f $@ -I$(G4INCLUDE) -c $^
		@echo Generating ConfigDict.cc


include $(G4INSTALL)/config/binmake.gmk


visclean:
	rm -f g4*.prim g4*.eps g4*.wrl
	rm -f .DAWN_*

