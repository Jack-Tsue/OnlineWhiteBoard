CC = g++
THIRDPARTY = /opt/local/include
THIRDPARTY2 = /usr/local/lib
CFLAGS = -static -g -I$(THIRDPARTY)/boost/include/boost-1_39 -I$(THIRDPARTY)/gflags/include -I$(THIRDPARTY)/pcre/include
LIBS = -L$(THIRDPARTY)/ -lpcre -lpcrecpp -lpthread -L$(THIRDPARTY2)/ -lgflags -L$(THIRDPARTY)/boost/ -lboost_system-gcc40-mt -lboost_thread-gcc40-mt
.cpp.o:
	@rm -f $@
	$(CC) $(CFLAGS) -c $*.cpp

CSRC := $(shell ls *.cpp)
CHEADER := $(shell ls *.hpp)

OBJ = $(CSRC:.cpp=.o)
ALL = httpd

all: $(ALL)

$(OBJ) : $(CHEADER)
httpd: $(OBJ)
	@rm -f $(@)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(OBJ) $(LIBS) $(NETLIBS)

clean:
	@rm httpd *.o
