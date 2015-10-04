include local/path.mk

FLAGS = -static-libgcc -static-libstdc++ -O3

default: pre core clean

bin/core.o: core/core.cpp
	g++ -c core/core.cpp -o bin/core.o $(INC) $(FLAGS)

core: bin/core.o
	g++ bin/core.o -o bin/pwclip.exe $(FLAGS) $(OBJEX) $(LIB)

pre:
	cls

clean:
	rm -f bin/core.o
