make: clean parser.o
	g++ -shared -o libparser.so parser.o

parser.o:
	g++ -fPIC -c parser.cpp

clean:
	rm -rf *.o

