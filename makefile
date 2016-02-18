# -- pwclip makefile for windows --

# See pwclip.mk.sample
include local/path.mk

FLAGS = -static-libgcc -static-libstdc++ -O3

default: pre win clean

bin/pwclip.o: src/win/pwclip.cpp
	g++ -c src/win/pwclip.cpp -o bin/pwclip.o $(INC) $(FLAGS)

win: bin/pwclip.o
	windres --input src/win/resources/pwclip.rc --output bin/pwclip.res --output-format=coff
	g++ -mwindows bin/pwclip.o -o bin/pwclip.exe $(FLAGS) $(OBJEX) $(LIB) -lcomctl32 bin/pwclip.res

pre:
	cls

clean:
	rm -f bin/pwclip.o
	rm -f bin/pwclip.res
