CC=g++
STD=-std=c++11
CF=$(STD)

ImageFilters.out: main.cpp const.hpp
	$(CC) $(CF) $(LINK_FLAGS) main.cpp -o ImageFilters.out

.PHONY: clean
clean:
	rm -rf *.o *.out 
	rm -rf output.bmp