WORKDIR = `pwd`

CC = gcc
CXX = g++
AR = ar
LD = g++
WINDRES = windres

INC = -I./
CFLAGS = -Wall
CXXFLAGS = -Wall -fexceptions -std=c++11
LIBDIR = 
LIB = 
LDFLAGS = 

INC_DEBUG = $(INC) -I../protocol
CXXFLAGS_DEBUG = $(CXXFLAGS) -g
LIBDIR_DEBUG = $(LIBDIR) -L../protocol
LIB_DEBUG = $(LIB)../protocol/libprotocol.a
LDFLAGS_DEBUG = $(LDFLAGS)
OBJDIR_DEBUG = obj/Debug
DEP_DEBUG = 
OUT_DEBUG =../GChat_client

INC_RELEASE = $(INC) -I../protocol
CXXFLAGS_RELEASE = $(CXXFLAGS) -O2
LIBDIR_RELEASE = $(LIBDIR)  -L../protocol
LIB_RELEASE = $(LIB)../protocol/libprotocol.a
LDFLAGS_RELEASE = $(LDFLAGS) -s
OBJDIR_RELEASE = obj/Release
DEP_RELEASE = 
OUT_RELEASE = ../GChat_client

OBJ_DEBUG = $(OBJDIR_DEBUG)/client.o $(OBJDIR_DEBUG)/epoll.o $(OBJDIR_DEBUG)/g_lib.o $(OBJDIR_DEBUG)/signal_handle.o

OBJ_RELEASE = $(OBJDIR_RELEASE)/client.o $(OBJDIR_RELEASE)/epoll.o $(OBJDIR_RELEASE)/g_lib.o $(OBJDIR_RELEASE)/signal_handle.o

all: release

clean: clean_release clean_debug

before_debug: 
	test -d ../protocol || exit 1
	test -e ../protocol/libprotocol.a || exit 1
	test -d $(OBJDIR_DEBUG) || mkdir -p $(OBJDIR_DEBUG)

after_debug: 

debug: before_debug out_debug after_debug

out_debug: before_debug $(OBJ_DEBUG) $(DEP_DEBUG)
	$(LD) $(LIBDIR_DEBUG) -o $(OUT_DEBUG) $(OBJ_DEBUG)  $(LDFLAGS_DEBUG) $(LIB_DEBUG)

$(OBJDIR_DEBUG)/client.o: client.cpp
	$(CXX) $(CXXFLAGS_DEBUG) $(INC_DEBUG) -c client.cpp -o $(OBJDIR_DEBUG)/client.o

$(OBJDIR_DEBUG)/epoll.o: epoll.cpp
	$(CXX) $(CXXFLAGS_DEBUG) $(INC_DEBUG) -c epoll.cpp -o $(OBJDIR_DEBUG)/epoll.o

$(OBJDIR_DEBUG)/g_lib.o: g_lib.cpp
	$(CXX) $(CXXFLAGS_DEBUG) $(INC_DEBUG) -c g_lib.cpp -o $(OBJDIR_DEBUG)/g_lib.o

$(OBJDIR_DEBUG)/signal_handle.o: signal_handle.cpp
	$(CXX) $(CXXFLAGS_DEBUG) $(INC_DEBUG) -c signal_handle.cpp -o $(OBJDIR_DEBUG)/signal_handle.o

clean_debug: 
	- rm -f $(OBJ_DEBUG) $(OUT_DEBUG)
	- rm -rf $(OBJDIR_DEBUG)
	rmdir -v ./obj

before_release: 
	test -d ../protocol || exit 1
	test -e ../protocol/libprotocol.a || exit 1
	test -d $(OBJDIR_RELEASE) || mkdir -p $(OBJDIR_RELEASE)

after_release: 

release: before_release out_release after_release

out_release: before_release $(OBJ_RELEASE) $(DEP_RELEASE)
	$(LD) $(LIBDIR_RELEASE) -o $(OUT_RELEASE) $(OBJ_RELEASE)  $(LDFLAGS_RELEASE) $(LIB_RELEASE)

$(OBJDIR_RELEASE)/client.o: client.cpp
	$(CXX) $(CXXFLAGS_RELEASE) $(INC_RELEASE) -c client.cpp -o $(OBJDIR_RELEASE)/client.o

$(OBJDIR_RELEASE)/epoll.o: epoll.cpp
	$(CXX) $(CXXFLAGS_RELEASE) $(INC_RELEASE) -c epoll.cpp -o $(OBJDIR_RELEASE)/epoll.o

$(OBJDIR_RELEASE)/g_lib.o: g_lib.cpp
	$(CXX) $(CXXFLAGS_RELEASE) $(INC_RELEASE) -c g_lib.cpp -o $(OBJDIR_RELEASE)/g_lib.o

$(OBJDIR_RELEASE)/signal_handle.o: signal_handle.cpp
	$(CXX) $(CXXFLAGS_RELEASE) $(INC_RELEASE) -c signal_handle.cpp -o $(OBJDIR_RELEASE)/signal_handle.o

clean_release: 
	- rm -f $(OBJ_RELEASE) $(OUT_RELEASE)
	- rm -rf $(OBJDIR_RELEASE)
	rmdir -v ./obj

.PHONY: before_debug after_debug clean_debug before_release after_release clean_release

