CC = g++
CPPFLAGS = -std=c++17
OBJECTS = fmt_tool.o fmt_type.o int_type.o ascii_type.o

%.o: %.cpp %.h %.tpp
	$(CC) -c -o $@ $< $(CPPFLAGS)

fmttool: main.o $(OBJECTS)
	$(CC) -o fmttool main.o $(OBJECTS)

.PHONY: clean

clean:
	rm -f *.o *~ core fmttool
