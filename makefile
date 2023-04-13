CC = g++
CLFAGS = -std=c++17
OBJECTS = fmt_tool.o fmt_type.o

%.o: %.cpp %.h
	$(CC) -c -o $@ $< $(CFLAGS)

main: main.o $(OBJECTS)
	$(CC) -o main main.o $(OBJECTS)

.PHONY: clean

clean:
	rm -f *.o *~ core main
