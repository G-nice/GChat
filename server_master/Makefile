WORKDIR = `pwd`

CC = gcc
CXX = g++
AR = ar
LD = g++
WINDRES = windres

INC = -I./ -I../protocol
CFLAGS = -Wall -std=c++11 -fexceptions
RESINC = 
LIBDIR = -L../protocol
LIB = ../protocol/libprotocol.a
LDFLAGS = 

INC_DEBUG = $(INC)
CFLAGS_DEBUG = $(CFLAGS) -g
RESINC_DEBUG = $(RESINC)
RCFLAGS_DEBUG = $(RCFLAGS)
LIBDIR_DEBUG = $(LIBDIR)
LIB_DEBUG = $(LIB)
LDFLAGS_DEBUG = $(LDFLAGS)
OBJDIR_DEBUG = obj/Debug
DEP_DEBUG = 
OUT_DEBUG = ../GChatServer_master

INC_RELEASE = $(INC)
CFLAGS_RELEASE = $(CFLAGS) -O2
RESINC_RELEASE = $(RESINC)
RCFLAGS_RELEASE = $(RCFLAGS)
LIBDIR_RELEASE = $(LIBDIR)
LIB_RELEASE = $(LIB)
LDFLAGS_RELEASE = $(LDFLAGS) -s
OBJDIR_RELEASE = obj/Release
DEP_RELEASE = 
OUT_RELEASE = ../GChatServer_master

OBJ_DEBUG = $(OBJDIR_DEBUG)/epoll.o $(OBJDIR_DEBUG)/g_lib.o $(OBJDIR_DEBUG)/server_master.o $(OBJDIR_DEBUG)/signal_handle.o $(OBJDIR_DEBUG)/user_manager.o

OBJ_RELEASE = $(OBJDIR_RELEASE)/epoll.o $(OBJDIR_RELEASE)/g_lib.o $(OBJDIR_RELEASE)/server_master.o $(OBJDIR_RELEASE)/signal_handle.o $(OBJDIR_RELEASE)/user_manager.o

all: release debug

clean: clean_release clean_debug

before_debug: 
	test -d ../protocol || exit 1
	test -e ../protocol/libprotocol.a || exit 1
	test -d $(OBJDIR_DEBUG) || mkdir -p $(OBJDIR_DEBUG)

after_debug: 

debug: before_debug out_debug after_debug

out_debug: before_debug $(OBJ_DEBUG) $(DEP_DEBUG)
	$(LD) $(LIBDIR_DEBUG) -o $(OUT_DEBUG) $(OBJ_DEBUG)  $(LDFLAGS_DEBUG) $(LIB_DEBUG)

$(OBJDIR_DEBUG)/epoll.o: epoll.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c epoll.cpp -o $(OBJDIR_DEBUG)/epoll.o

$(OBJDIR_DEBUG)/g_lib.o: g_lib.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c g_lib.cpp -o $(OBJDIR_DEBUG)/g_lib.o

$(OBJDIR_DEBUG)/server_master.o: server_master.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c server_master.cpp -o $(OBJDIR_DEBUG)/server_master.o

$(OBJDIR_DEBUG)/signal_handle.o: signal_handle.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c signal_handle.cpp -o $(OBJDIR_DEBUG)/signal_handle.o

$(OBJDIR_DEBUG)/user_manager.o: user_manager.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c user_manager.cpp -o $(OBJDIR_DEBUG)/user_manager.o

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

$(OBJDIR_RELEASE)/epoll.o: epoll.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c epoll.cpp -o $(OBJDIR_RELEASE)/epoll.o

$(OBJDIR_RELEASE)/g_lib.o: g_lib.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c g_lib.cpp -o $(OBJDIR_RELEASE)/g_lib.o

$(OBJDIR_RELEASE)/server_master.o: server_master.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c server_master.cpp -o $(OBJDIR_RELEASE)/server_master.o

$(OBJDIR_RELEASE)/signal_handle.o: signal_handle.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c signal_handle.cpp -o $(OBJDIR_RELEASE)/signal_handle.o

$(OBJDIR_RELEASE)/user_manager.o: user_manager.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c user_manager.cpp -o $(OBJDIR_RELEASE)/user_manager.o

clean_release: 
	- rm -f $(OBJ_RELEASE) $(OUT_RELEASE)
	- rm -rf $(OBJDIR_RELEASE)
	rmdir -v ./obj

.PHONY: before_debug after_debug clean_debug before_release after_release clean_release

