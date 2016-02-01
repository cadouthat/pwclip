include local/path.mk

FLAGS = -static-libgcc -static-libstdc++ -O3

default: pre win clean

bin/pwclip-win.o: src/win/pwclip.cpp
	g++ -c src/win/pwclip.cpp -o bin/pwclip-win.o $(INC) $(FLAGS)

win: bin/pwclip-win.o
	windres --input src/win/resources/pwclip.rc --output bin/pwclip.res --output-format=coff
	g++ -mwindows bin/pwclip-win.o -o bin/pwclip-win.exe $(FLAGS) $(OBJEX) $(LIB) -lcomctl32 bin/pwclip.res

pre:
	cls

clean:
	rm -f bin/pwclip-win.o
	rm -f bin/pwclip.res
