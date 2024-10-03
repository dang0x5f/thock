.POSIX:
CC     := clang
CFLAGS := -ggdb -Wall -Wextra
LDLIBS := -lncurses
# CPPFLAGS := ?

BIN  := prog

SRCS := thock.c ui.c 
OBJS := ${SRCS:c=o}

${BIN}: ${OBJS}
	${CC} ${OBJS} -o $@ ${LDLIBS}

debug: ${OBJS}
	${CC} ${OBJS} -o ${BIN}

clean:
	rm -f ${BIN} ${OBJS}

.PHONY: clean
