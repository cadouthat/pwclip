include local/path.mk

FLAGS = -static-libgcc -static-libstdc++ -O3

default: pre pwcliptray clean

bin/pwcliptray.o: pwcliptray/pwcliptray.cpp
	g++ -c pwcliptray/pwcliptray.cpp -o bin/pwcliptray.o $(INC) $(FLAGS)

pwcliptray: bin/pwcliptray.o
	g++ -mwindows bin/pwcliptray.o -o bin/pwcliptray.exe $(FLAGS) $(OBJEX) $(LIB)

pre:
	cls

clean:
	rm -f bin/pwcliptray.o
