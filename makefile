include local/path.mk

FLAGS = -static-libgcc -static-libstdc++ -O3

default: pre core pwclipdump clean

bin/core.o: core/core.cpp
	g++ -c core/core.cpp -o bin/core.o $(INC) $(FLAGS)

core: bin/core.o
	g++ bin/core.o -o bin/pwclip.exe $(FLAGS) $(OBJEX) $(LIB)

bin/pwclipdump.o: pwclipdump/pwclipdump.cpp
	g++ -c pwclipdump/pwclipdump.cpp -o bin/pwclipdump.o $(INC) $(FLAGS)

pwclipdump: bin/pwclipdump.o
	g++ bin/pwclipdump.o -o bin/pwclipdump.exe $(FLAGS) $(OBJEX) $(LIB)

pre:
	cls

clean:
	rm -f bin/core.o
	rm -f bin/pwclipdump.o
