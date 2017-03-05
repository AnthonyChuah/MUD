TARGET = mud
OBJFILES = main4.o UserConnection.o ConnectionManager.o Engine.o

LINKERFLAGS = -g -pthread
COMPILERFLAGS = -Wall -std=c++11 -pthread -pipe -g

LINKER = g++
COMPILER = g++

$(TARGET): $(OBJFILES)
	$(LINKER) $(LINKERFLAGS) $(OBJFILES) -o $(TARGET)

UserConnection.o: src/UserConnection.cpp src/UserConnection.h
	$(COMPILER) $(COMPILERFLAGS) -c src/UserConnection.cpp

ConnectionManager.o: src/ConnectionManager.cpp src/ConnectionManager.h src/UserConnection.h src/Engine.h
	$(COMPILER) $(COMPILERFLAGS) -c src/ConnectionManager.cpp

Engine.o: src/Engine.cpp src/Engine.h src/UserConnection.h src/ConnectionManager.h
	$(COMPILER) $(COMPILERFLAGS) -c src/Engine.cpp

main4.o: main4.cpp src/UserConnection.h src/ConnectionManager.h src/Engine.h
	$(COMPILER) $(COMPILERFLAGS) -c main4.cpp

.PHONY: clean
clean:
	rm -f $(OBJFILES)
	rm -f $(TARGET)
