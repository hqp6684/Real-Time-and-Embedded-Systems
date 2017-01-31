HEADERS = c_language_test.h unit_tests.h

default: test

test:	main.o c_language_test.o unit_tests.o
	gcc -g -o test main.o c_language_test.o unit_tests.o
	
c_language_test.o: c_language_test.c $(HEADERS)
	gcc -g -c c_language_test.c

main.o: main.c $(HEADERS)
	gcc -g -c main.c
	
unit_tests.o: unit_tests.c $(HEADERS)
	gcc -g -c unit_tests.c

clean:
	-rm -f c_language_test.o
	-rm -f main.o
	-rm -f unit_tests.o
	-rm -f test
	