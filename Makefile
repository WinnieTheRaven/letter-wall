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

LDFLAGS = ${LIBS}
CFLAGS = -Wall -Wextra -O0 ${INCS}

all: ${PROG}

${PROG}: ${OBJ}
	${CC} -o $@ ${OBJ} ${LDFLAGS}

%.o: %.c
	${CC} -c $< ${CFLAGS}

clean:
	-rm ${OBJ} ${PROG}

.PHONY: all clean
