.PHONY: run clean

run: project3
	./project3

project3: project3_008.o
	cc -o project3 project3_008.o -lpthread
project3.o: project3.c
	cc -c project3_008.c -lpthread
clean:
	rm *.o project3
