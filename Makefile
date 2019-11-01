all : sender recv

sender : sender.o
	g++ -o sender sender.o

recv : recv.o
	g++ -o recv recv.o

sender.o : sender.cpp msg.h
	g++ -c sender.cpp

recv.o : recv.cpp msg.h
	g++ -c recv.cpp

clean :
	rm sender recv sender.o recv.o
