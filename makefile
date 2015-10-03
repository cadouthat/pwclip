INC = -I"C:/thirdparty_libs/"
OBJ = "C:/thirdparty_libs/sqlite3/sqlite3.o"
FLAGS = -static-libgcc -static-libstdc++ -O3

default: pre core clean

bin/core.o: core/core.cpp
	g++ -c core/core.cpp -o bin/core.o $(INC) $(FLAGS)

core: bin/core.o
	g++ bin/core.o -o bin/pwclip.exe $(FLAGS) $(OBJ)

pre:
	cls

clean:
	rm -f bin/core.o
