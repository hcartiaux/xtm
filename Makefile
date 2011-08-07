CC=gcc
EXEC=xtm

INSTALL_DIRECTORY=/usr/bin
MAN_DIRECTORY=/usr/share/man/fr/man1

all: $(EXEC)

xtm: xtm.o xtmfile.o
	$(CC) -o xtm xtm.o xtmfile.o $(LDFLAGS)

xtmfile.o: src/xtmfile.c src/xtmfile.h
	$(CC) -o xtmfile.o -c src/xtmfile.c $(CFLAGS)

xtm.o: src/xtm.c src/xtmfile.h
	$(CC) -o xtm.o -c src/xtm.c $(CFLAGS)

clean:
	rm -rf *.o

mrproper: clean
	rm -f xtm

install:
	cp xtm $(INSTALL_DIRECTORY)

uninstall:
	rm -f $(INSTALL_DIRECTORY)/xtm

install-man:
	cp xtm.1.gz $(MAN_DIRECTORY)

uninstall-man:
	rm -f $(MAN_DIRECTORY)/xtm.1.gz
