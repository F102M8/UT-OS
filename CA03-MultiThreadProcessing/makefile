CC=g++
STD=-std=c++11
CF=$(STD)
LINK_FLAGS = -pthread

ImageFilters.out: main.cpp
	$(CC) $(CF) $(LINK_FLAGS) main.cpp -o ImageFilters.out

.PHONY: clean
clean:
	rm -rf *.o *.out
