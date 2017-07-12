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


.PHONY: all

TARGET = RootEvent

all: lib$(TARGET).so lib bin  libConfig.so


lib$(TARGET).so : $(TARGET).o $(TARGET)Dict.o 
		g++ -o $@ -shared -fPIC  $^   $(LDFLAGS) 


RootEvent.o : src/RootEvent.cc
		g++ -o $@ -I/$(G4INCLUDE)  $(CPPFLAGS)  -c $<

RootEventDict.o : src/RootEventDict.cc
		g++ -o $@ -I/$(G4INCLUDE) $(CPPFLAGS)  -c $< 

src/RootEventDict.cc :  include/RootEvent.hh include/RootEventLinkDef.hh
		rootcling -f $@ -I$(G4INCLUDE) -c $^

libConfig.so : Config.o  ConfigDict.o 
		g++ -o $@ -shared -fPIC  $^   $(LDFLAGS) 

Config.o : src/Config.cc
		g++ -o $@ -I/$(G4INCLUDE)  $(CPPFLAGS)  -c $<

ConfigDict.o : src/ConfigDict.cc
		g++ -o $@ -I/$(G4INCLUDE) $(CPPFLAGS)  -c $< 

src/ConfigDict.cc :  include/Config.h include/ConfigLinkDef.hh
		rootcling -f $@ -I$(G4INCLUDE) -c $^


myclean :
			rm -rf dict.* $(G4TMP)/$(G4SYSTEM)/lsrp *.o *.so src/$(TARGET)Dict.cc src/*.pcm   *.pcm

include $(G4INSTALL)/config/binmake.gmk

visclean:
	rm -f g4*.prim g4*.eps g4*.wrl
	rm -f .DAWN_*

