make: clean parser.o libparser.so mv
	sudo ldconfig

mv: 
	sudo mv libparser.so /usr/lib/

libparser.so:
	g++ -shared -o libparser.so parser.o

parser.o:
	g++ -fPIC -c parser.cpp

clean:
	rm -rf *.o

