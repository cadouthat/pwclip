include local/path.mk

FLAGS = -static-libgcc -static-libstdc++ -O3

default: pre pwclip pwclipdump pwcliptray clean

bin/pwclip.o: pwclip/pwclip.cpp
	g++ -c pwclip/pwclip.cpp -o bin/pwclip.o $(INC) $(FLAGS)

pwclip: bin/pwclip.o
	g++ bin/pwclip.o -o bin/pwclip.exe $(FLAGS) $(OBJEX) $(LIB)

bin/pwclipdump.o: pwclipdump/pwclipdump.cpp
	g++ -c pwclipdump/pwclipdump.cpp -o bin/pwclipdump.o $(INC) $(FLAGS)

pwclipdump: bin/pwclipdump.o
	g++ bin/pwclipdump.o -o bin/pwclipdump.exe $(FLAGS) $(OBJEX) $(LIB)

bin/pwcliptray.o: pwcliptray/pwcliptray.cpp
	g++ -c pwcliptray/pwcliptray.cpp -o bin/pwcliptray.o $(INC) $(FLAGS)

pwcliptray: bin/pwcliptray.o
	g++ -mwindows bin/pwcliptray.o -o bin/pwcliptray.exe $(FLAGS) $(OBJEX) $(LIB)

pre:
	cls

clean:
	rm -f bin/pwclip.o
	rm -f bin/pwclipdump.o
	rm -f bin/pwcliptray.o
