TARGET = tmud
OBJFILES = main.o

LINKERFLAGS = -g
COMPILERFLAGS = -Wall -std=c++11 -pipe -g

LINKER = g++
COMPILER = g++

$(TARGET): $(OBJFILES)
	$(LINKER) $(LINKERFLAGS) $(OBJFILES) -o $(TARGET)

main.o: main.cpp
	$(COMPILER) $(COMPILERFLAGS) -c main.cpp

.PHONY: clean
clean:
	rm -f $(OBJFILES)
	rm -f $(TARGET)
