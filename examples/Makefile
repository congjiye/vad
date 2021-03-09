all: vad_base vad_class

CFLAGS = -I../include

vad_base: vad_base.o
		g++ -g -O3 $^ -o $@
		rm -f vad_base.o

vad_class: vad_class.o
		g++ -g -O3 $^ -o $@
		rm -f vad_class.o

%.o: %.c
	g++ $(CFLAGS) -c -o $@ $<

%.o: %.cc
	g++ -std=c++17 $(CFLAGS) -c -o $@ $<

clean:
	rm -f *.o *.a