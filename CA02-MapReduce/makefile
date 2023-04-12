# flags:
CC = g++ -std=c++17 -Wall -pedantic


# paths:
PATH_SRC := src
PATH_INC := include

OUT = ClubsAgeStats.out

all : $(OUT) country club
# club position

#---------------------------------------------------------
$(OUT): $(PATH_SRC)/ClubsAgeStats.cpp $(PATH_INC)/const.hpp
	$(CC) $(PATH_SRC)/ClubsAgeStats.cpp -o $(OUT)
country: $(PATH_SRC)/country.cpp $(PATH_INC)/const.hpp
		$(CC) $(PATH_SRC)/country.cpp -o country

club: $(PATH_SRC)/club.cpp $(PATH_INC)/const.hpp
		$(CC) $(PATH_SRC)/club.cpp -o club

#position: $(PATH_SRC)/position.cpp $(PATH_INC)/const.hpp
#		$(CC) $(PATH_SRC)/position.cpp -o position

#---------------------------------------------------------
PHONY: all clean
clean:
	rm -rf ClubsAgeStats.out
	rm -rf country
	rm -rf club