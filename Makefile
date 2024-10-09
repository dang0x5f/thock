.POSIX:
CC     := clang
CFLAGS := -ggdb -Wall -Wextra 
LDLIBS := -lncursesw
# CPPFLAGS :=

BIN  := prog

SRCS := thock.c ui.c 
OBJS := ${SRCS:c=o}

MOD_SRCS := ${SRCS} dictmod.c
MOD_OBJS := ${MOD_SRCS:c=o}

${BIN}: ${OBJS}
	${CC} ${OBJS} -o $@ ${LDLIBS}

${OBJS}: ${SRCS}
	${CC} -c ${SRCS} ${CPPFLAGS} ${CFLAGS}

mods: 
	${MAKE} SRCS="${MOD_SRCS}" OBJS="${MOD_OBJS}" CPPFLAGS="-DWITH_MODS"

# debug: ${OBJS}
# 	${CC} ${OBJS} -o ${BIN}

todo:
	grep -rn TODO

tags:
	uctags *.h *.c

clean:
	rm -f ${BIN} *.o *.d *.core tags

.PHONY: clean
