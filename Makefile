# Copyright (c) 1999 Apple Computer, Inc.  All rights reserved.
#  

NAME = libLogManagerLib
STATICLIB = libLogManagerLib.a
SHAREDLIB = libLogManager.so
C++ = gcc #$(CPLUS)
CC = gcc #$(CCOMP)
LINK = $(LINKER)

COMPILER_FLAGS = -D_REENTRANT -D__USE_POSIX -D__linux__ -pipe
INCLUDE_FLAG = -include
CCFLAGS += $(COMPILER_FLAGS) -fPIC -g -Wall $(INCLUDE_FLAG) ./PlatformHeader.h #$(INCLUDE_FLAG) ./SSPlatformHeader.h

# OPTIMIZATION
#CCFLAGS += -O2

# EACH DIRECTORY WITH HEADERS MUST BE APPENDED IN THIS MANNER TO THE CCFLAGS

CCFLAGS += -I.
CCFLAGS += -I..
CCFLAGS += -I../../include/

C++FLAGS = $(CCFLAGS)

CFILES =

CPPFILES =	Synchronize.cpp \
            StdioFile.cpp \
            LogProcess.cpp \
			LogThread.cpp \
            MsgThread.cpp \
			SSS_LogManagerU.cpp \
			xhLogManager.cpp \
			LogManager.cpp \
			SSS_LogManagerC.cpp 
            

all: $(SHAREDLIB) $(STATICLIB)

$(SHAREDLIB): $(CFILES:.c=.o) $(CPPFILES:.cpp=.o)
	g++ -shared -fPIC -o $(SHAREDLIB) $(CFILES:.c=.o) $(CPPFILES:.cpp=.o)
	cp -f $(SHAREDLIB) ../../centosx64/binr/

$(STATICLIB): $(CFILES:.c=.o) $(CPPFILES:.cpp=.o)
	ar -r $(STATICLIB) $(CFILES:.c=.o) $(CPPFILES:.cpp=.o)
	ranlib $(STATICLIB)
	cp -f $(STATICLIB) ../../centosx64/libr/
	cp -f SSS_LogManagerU.h ../../include/
	cp -f SSS_LogManagerC.h ../../include/

install: $(STATICLIB)
clean:
	rm -f $(STATICLIB) $(SHAREDLIB) $(CFILES:.c=.o) $(CPPFILES:.cpp=.o)

.SUFFIXES: .cpp .c .o

.cpp.o:
	$(C++) -c -o $*.o $(DEFINES) $(C++FLAGS) $*.cpp

.c.o:
	$(CC) -c -o $*.o $(DEFINES) $(CCFLAGS) $*.c
	

