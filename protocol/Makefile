WORKDIR = `pwd`

CC = gcc
CXX = g++
AR = ar
LD = g++
WINDRES = windres

INC = 
CFLAGS = -Wall
CXXFLAGS = -Wall -std=c++11
LIB = 

INC_DEBUG = $(INC) -I../server_master
CXXFLAGS_DEBUG = $(CXXFLAGS) -g
OBJDIR_DEBUG = obj/Debug
OUT_DEBUG = ../protocol/libprotocol.a

INC_RELEASE = $(INC) -I../server_master
CXXFLAGS_RELEASE = $(CXXFLAGS) -O2
OBJDIR_RELEASE = obj/Release
OUT_RELEASE = ../protocol/libprotocol.a

OBJ_DEBUG = $(OBJDIR_DEBUG)/build_pack.o $(OBJDIR_DEBUG)/parse_pack.o

OBJ_RELEASE = $(OBJDIR_RELEASE)/build_pack.o $(OBJDIR_RELEASE)/parse_pack.o

all: release debug 

clean: clean_release clean_debug

before_debug: 
	test -d $(OBJDIR_DEBUG) || mkdir -p $(OBJDIR_DEBUG)

after_debug: 

debug: before_debug out_debug after_debug

out_debug: before_debug $(OBJ_DEBUG) $(DEP_DEBUG)
	$(AR) rcs $(OUT_DEBUG) $(OBJ_DEBUG)

$(OBJDIR_DEBUG)/build_pack.o: build_pack.cpp
	$(CXX) $(CXXFLAGS_DEBUG) $(INC_DEBUG) -c build_pack.cpp -o $(OBJDIR_DEBUG)/build_pack.o

$(OBJDIR_DEBUG)/parse_pack.o: parse_pack.cpp
	$(CXX) $(CXXFLAGS_DEBUG) $(INC_DEBUG) -c parse_pack.cpp -o $(OBJDIR_DEBUG)/parse_pack.o

clean_debug: 
	- rm -f $(OBJ_DEBUG) $(OUT_DEBUG)
	- rm -rf $(OBJDIR_DEBUG)
	rmdir -v ./obj

before_release: 
	test -d $(OBJDIR_RELEASE) || mkdir -p $(OBJDIR_RELEASE)

after_release: 

release: before_release out_release after_release

out_release: before_release $(OBJ_RELEASE) $(DEP_RELEASE)
	$(AR) rcs $(OUT_RELEASE) $(OBJ_RELEASE)

$(OBJDIR_RELEASE)/build_pack.o: build_pack.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c build_pack.cpp -o $(OBJDIR_RELEASE)/build_pack.o

$(OBJDIR_RELEASE)/parse_pack.o: parse_pack.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c parse_pack.cpp -o $(OBJDIR_RELEASE)/parse_pack.o

clean_release: 
	- rm -f $(OBJ_RELEASE) $(OUT_RELEASE)
	- rm -rf $(OBJDIR_RELEASE)
	rmdir -v ./obj

.PHONY: before_debug after_debug clean_debug before_release after_release clean_release

