make: clean parser.o libparser.so

install: clean parser.o libparser.so mv
	sudo ldconfig

mv:
	sudo mv libparser.so /usr/lib/

libparser.so:
	g++ -shared -o libparser.so parser.o

parser.o:
	g++ -fPIC -c parser.cpp

clean:
	rm -rf *.o test.out libparser.so

test: clean test.o parser.test
	g++ -o test.out test.o parser.o

test.o:
	g++ -g -c test.cpp

parser.test:
	g++ -g -c parser.cpp

