# Easily adaptable makefile for Advanced Programming
# Note: remove comments (#) to activate some features
#
# author Vitor Carreira
# date 2010-09-26


# Libraries to include (if any)
LIBS=#-pthread 

# Compiler flags
CFLAGS=-Wall -W -Wmissing-prototypes

# Indentation flags
IFLAGS=-br -brs -npsl -ce -cli4


# Name of the executable
PROGRAM=palz

# Prefix for the gengetopt file (if gengetopt is used)
PROGRAM_OPT=config

# Object files required to build the executable
PROGRAM_OBJS=main.o debug.o memory.o etapa1.o ${PROGRAM_OPT}.o

# Clean and all are not files
.PHONY: clean all docs indent debugon

all: ${PROGRAM}

# compilar com depuracao
debugon: CFLAGS += -D SHOW_DEBUG -g
debugon: ${PROGRAM}

${PROGRAM}: ${PROGRAM_OBJS}
	${CC} -o $@ ${PROGRAM_OBJS} ${LIBS}

# Dependencies
main.o: main.c debug.h memory.h ${PROGRAM_OPT}.h etapa1.h
${PROGRAM_OPT}.o: ${PROGRAM_OPT}.c ${PROGRAM_OPT}.h

debug.o: debug.c debug.h
memory.o: memory.c memory.h
etapa1.o: etapa1.c etapa1.h

#how to create an object file (.o) from C file (.c)
.c.o:
	${CC} ${CFLAGS} -c $<

# Generates command line arguments code from gengetopt configuration file
${PROGRAM_OPT}.h: ${PROGRAM_OPT}.ggo
	gengetopt < ${PROGRAM_OPT}.ggo --file-name=${PROGRAM_OPT}

clean:
	rm -f *.o core.* *~ ${PROGRAM} *.bak ${PROGRAM_OPT}.h ${PROGRAM_OPT}.c

docs: Doxyfile
	doxygen Doxyfile

Doxyfile:
	doxygen -g Doxyfile

indent:
	indent ${IFLAGS} *.c *.h
