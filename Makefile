# General Makefile

# Output directory and name
OUT_DIR = ./
OUTPUT_DEBUG_NAME = um2nc_dbg
OUTPUT_RELEASE_NAME = um2nc

# Compiler variables
CPP = g++
INCLUDE_DIRS = tclap/include/
LIBRARY_DIRS = ./

DEBUG_FLAGS = -Wno-deprecated -O0 -pg -g -Wall -DTIXML_USE_STL -include cstdio
RELEASE_FLAGS = -Wno-deprecated -O3 -Wall -DTIXML_USE_STL -fno-strict-aliasing -include cstdio

FLAGS = $(DEBUG_FLAGS)
#FLAGS = $(RELEASE_FLAGS)
UNAME := $(shell uname)

# Linker variables - cannot static link in Mac OS X / Darwin
ifeq ($(UNAME), Darwin)
LD = g++ -Wunused $(FLAGS)
else
LD = g++ -Wunused --static $(FLAGS)
endif

LIBS = -L/usr/local/lib -lnetcdf_c++ -lnetcdf -lstdc++

ALL_DEPEND = RawConvert.h

OBJS = main.o ConvertFile.o ReadUMFile.o Region.o Level.o TimeField.o \
	   VarField.o Variable.o MTime.o ExtractVariables.o WriteNetCDFFile.o \
	   MinimiseDimensions.o Rot2Global.o VarLevTrans.o Attribute.o \
	   AttributeList.o GlobalAtts.o Packing.o

TINY_XML_OBJS = tinyxml/tinyxml.o \
				tinyxml/tinystr.o \
				tinyxml/tinyxmlerror.o \
				tinyxml/tinyxmlparser.o

# cpp file do all
%.o: %.cpp %.h $(ALL_DEPEND)
	$(CPP) $(FLAGS) -I$(INCLUDE_DIRS) -c $<
%.o: %.cpp $(ALL_DEPEND)
	$(CPP) $(FLAGS) -I$(INCLUDE_DIRS) -c $<
%.o: %.c
	$(CPP) $(FLAGS) -I$(INCLUDE_DIRS) -c $<

$(OUTPUT_RELEASE_NAME): $(OBJS)
	$(LD) $(OBJS) $(TINY_XML_OBJS) $(LIBS) -o $(OUT_DIR)$(OUTPUT_RELEASE_NAME)

$(OUTPUT_DEBUG_NAME): $(OBJS)
	$(LD) $(OBJS) $(TINY_XML_OBJS) $(LIBS) -o $(OUT_DIR)$(OUTPUT_DEBUG_NAME)

clean:
	rm $(OBJS) $(OUT_DIR)$(OUTPUT_DEBUG_NAME) $(OUT_DIR)$(OUTPUT_RELEASE_NAME)
