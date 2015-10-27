include local/path.mk

FLAGS = -static-libgcc -static-libstdc++ -O3

default: pre win clean

bin/pwclip-win.o: src/win/pwclip.cpp
	g++ -c src/win/pwclip.cpp -o bin/pwclip-win.o $(INC) $(FLAGS)

win: bin/pwclip-win.o
	g++ -mwindows bin/pwclip-win.o -o bin/pwclip-win.exe $(FLAGS) $(OBJEX) $(LIB)

pre:
	cls

clean:
	rm -f bin/pwclip-win.o
