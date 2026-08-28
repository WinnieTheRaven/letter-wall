PROG = letter-wall
SRC = ${PROG}.c
OBJ = ${SRC:.c=.o}

CC = cc

# uncomment if linux
#INCS = -I/usr/include/X11
#LIBS = -lX11

# uncomment if OpenBSD
INCS = -I/usr/X11R6/include
LIBS = -L/usr/X11R6/lib -lX11

CPPFlags = ${INCS}
CFLAGS = -std=c17 \
	-Wall \
	-Wextra \
	-Wpedantic \
	-Wconversion \
	-Wsign-conversion \
	-Wshadow \
	-Wstrict-prototypes \
	-Wmissing-prototypes \
	-Wold-style-definition\
	-Werror \
	-O0 \
	-g \

LDFLAGS = ${LIBS}

all: ${PROG}

${PROG}: ${OBJ}
	${CC} ${LDFLAGS} -o $@ ${OBJ}

.c.o:
	${CC} ${CPPFlags} ${CFLAGS} -c $< -o $@

clean:
	-rm ${OBJ} ${PROG}

.PHONY: all clean
