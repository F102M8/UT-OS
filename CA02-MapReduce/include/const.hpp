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
#include <fcntl.h>
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
const string NAMED_PIPES_FOLDER = "named_pipes/";

const int MESSAGE_SIZE = 1024;

const int MIN = 0;
const int MAX = 1;
const int SUM = 2;
const int COUNT = 3;