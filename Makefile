# binpack - lay out windows automatically
# See LICENSE file for copyright and license details.

SRC = main.c    \
	  binpack.c \
	  bin_utils.c

OBJ = ${SRC:.c=.o}

all: binpack

.c.o:
	@echo CC $<
	@${CC} -c ${CFLAGS} $<

binpack: ${OBJ}
	@echo CC -o $@
	@${CC} -o $@ ${OBJ} ${LDFLAGS}

clean:
	@echo cleaning
	@rm -f binpack ${OBJ}

install: all
	@mkdir -p ${DESTDIR}${PREFIX}/bin
	@cp -f binpack ${DESTDIR}${PREFIX}/bin
	@chmod 755 ${DESTDIR}${PREFIX}/bin/binpack
uninstall:
	@rm -f ${DESTDIR}${PREFIX}/bin/binpack

.PHONY: all clean install uninstall
