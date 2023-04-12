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

struct Positions_Data {
    string name;
    int min_age;
    int max_age;
    int sum_age;
    int count;
};

const string ALL_POSITIONS_FILE = "positions.csv";

const string EXECUTABLE_FILE_COUNTRY = "./country";
const string EXECUTABLE_FILE_CLUB = "./club";
const string EXECUTABLE_FILE_POSITION = "./position";


const int MESSAGE_SIZE = 1024;
