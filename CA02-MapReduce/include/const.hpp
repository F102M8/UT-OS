// libraries:
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <filesystem>
#include <sstream>
#include <fstream>
#include <cstdarg>

#include <regex>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
using namespace std;

struct player {
    string name;
    string position;
    int age;    
};

const string ALL_POSITIONS_FILE = "positions.csv";

const string EXECUTABLE_FILE_COUNTRY = "./country";

const int MASSAGE_SIZE = 1024;
