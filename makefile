CC = g++
CLFAGS = -std=c++17
OBJECTS = fmt_tool.o fmt_type.o int_type.o

%.o: %.cpp %.h
	$(CC) -c -o $@ $< $(CFLAGS)

fmttool: main.o $(OBJECTS)
	$(CC) -o fmttool main.o $(OBJECTS)

.PHONY: clean

clean:
	rm -f *.o *~ core fmttool
