CAENDIR=      /home/dali/CAENHVWrapper-6.0/

PROGRAM=	HVRead

CC=		gcc

FLAGS=		-DUNIX -DLINUX -g

LFLAGS=

LIBS=		-lcaenhvwrapper -lncurses -lpthread -ldl -lm

INCLUDEDIR=	-I./ -I$(CAENDIR)

SOURCES=	HVRead.c HVFunctions.c 

OBJECTS=	HVRead.o HVFunctions.o 

INCLUDES=	HVFunction.h CAENHVWrapper.h 

########################################################################

ARFLAGS=		r

CFLAGS=			$(FLAGS)

all:			$(PROGRAM)

$(PROGRAM):		$(OBJECTS)
			$(CC) $(CFLAGS) $(LFLAGS) -o $(PROGRAM) $(OBJECTS)\
			$(LIBS)

$(OBJECTS):		$(SOURCES)

%.o:			%.c
			$(CC) $(CFLAGS) $(INCLUDEDIR) -o $@ -c $<

clean:
			rm -f $(OBJECTS) $(PROGRAM)
