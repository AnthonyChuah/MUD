TARGET = oomud
OBJFILES = main3.o ChatBuffer.o UserConnection.o Server.o

LINKERFLAGS = -g -pthread
COMPILERFLAGS = -Wall -std=c++11 -pthread -pipe -g

LINKER = g++
COMPILER = g++

$(TARGET): $(OBJFILES)
	$(LINKER) $(LINKERFLAGS) $(OBJFILES) -o $(TARGET)

ChatBuffer.o: src/ChatBuffer.cpp src/ChatBuffer.h
	$(COMPILER) $(COMPILERFLAGS) -c src/ChatBuffer.cpp

UserConnection.o: src/UserConnection.cpp src/UserConnection.h src/ChatBuffer.cpp src/ChatBuffer.h
	$(COMPILER) $(COMPILERFLAGS) -c src/UserConnection.cpp

Server.o: src/Server.cpp src/Server.h src/UserConnection.h src/UserConnection.cpp src/ChatBuffer.h src/ChatBuffer.cpp
	$(COMPILER) $(COMPILERFLAGS) -c src/Server.cpp

main3.o: main3.cpp src/Server.h src/Server.cpp
	$(COMPILER) $(COMPILERFLAGS) -c main3.cpp

.PHONY: clean
clean:
	rm -f $(OBJFILES)
	rm -f $(TARGET)
