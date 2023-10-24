main:main.o cmd_mng.o myshell.o
	g++ -o main main.o cmd_mng.o myshell.o

main.o: main.cpp
	g++ -c main.cpp -o main.o

cmd_mng.o: ./src/CommandManager.cpp
	g++ -c ./src/CommandManager.cpp -o cmd_mng.o

myshell.o: ./src/MyShell.cpp
	g++ -c ./src/MyShell.cpp -o myshell.o

clear:
	-rm -f *.o main