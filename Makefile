CFLAGS=-Wall -g -pedantic -O2
LDFLAGS=-O2
LDLIBS=-lm

all: sarc sarx libsar.a
zip: Makefile $(SOURCES) $(HEADERS)
	zip -r mypackage.zip $^

libsar.a : sar.o md5.o
	ar rcu libsar.a $^

sarc: sarc.o libsar.a
sarx: sarx.o libsar.a

clean:
	rm -f sarc sarx *.o lib*.a

.PHONY: all clean
