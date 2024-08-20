.POSIX:
CC     := clang
CFLAGS := -ggdb -Wall -Wextra
# CPPFLAGS := ?

BIN  := prog
SRCS := main.c
OBJS := ${SRCS:c=o}

${BIN}: ${OBJS}
	${CC} ${OBJS} -o $@

clean:
	rm -f ${BIN} ${OBJS}

.PHONY: clean
