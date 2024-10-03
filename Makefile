.POSIX:
CC     := clang
CFLAGS := -ggdb -Wall -Wextra
LDLIBS := -lncurses
# CPPFLAGS := ?

BIN  := prog

SRCS := thock.c ui.c 
OBJS := ${SRCS:c=o}

MOD_SRCS := ${SRCS} dictmod.c
MOD_OBJS := ${MOD_SRCS:c=o}

${BIN}: ${OBJS}
	${CC} ${OBJS} -o $@ ${LDLIBS}

${OBJS}: ${SRCS}
	${CC} -c ${SRCS}

mods: ${MOD_OBJS}
	${CC} ${MOD_OBJS} -o ${BIN} ${LDLIBS}

${MOD_OBJS}: ${MOD_SRCS}
	${CC} -c ${MOD_SRCS} -DWITH_MODS

debug: ${OBJS}
	${CC} ${OBJS} -o ${BIN}

clean:
	rm -f ${BIN} *.o

.PHONY: clean
