all: main

CFLAGS += -I../include

main: main.o
		g++ -g -O3 $^ -o $@ $(LIBS) -lgfortran -lm -lpthread -ldl
		rm -f test_vad.o

%.o: %.c
	g++ $(CFLAGS) -c -o $@ $<

%.o: %.cc
	g++ -std=c++17 $(CFLAGS) -c -o $@ $<

clean:
	rm -f *.o *.a