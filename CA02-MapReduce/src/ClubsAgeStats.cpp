#include "../include/const.hpp"
vector<string> split(const string &str, char delim)
{
    vector<string> elements;
    istringstream ss(str);
    string item;
    while (getline(ss, item, delim))
    {
        item.erase(0, item.find_first_not_of(' '));
        item.erase(item.find_last_not_of(' ') + 1);
        if (!item.empty())
            elements.push_back(item);
    }
    return elements;
}

void read_positions(const string &posintions_file_folder, vector<string> &positions) {
    string posintions_file_path = posintions_file_folder + "/" + ALL_POSITIONS_FILE;
    ifstream file(posintions_file_path);
    if (!file.is_open()) {
        cerr <<  "Failed to open file " << posintions_file_path.c_str() << "\n";
        abort();
    }
    string line;
    getline(file, line);
    positions = split(line, ',');
}

void read_countries(const string &countries_file_folder, vector<filesystem::directory_entry> &countries) {
    for (const auto &entry: filesystem::directory_iterator(countries_file_folder)) {
        if(entry.path().filename() != ALL_POSITIONS_FILE) {
            countries.push_back(entry);
        }
    }
}

bool valid_clubs_path(const string &path) {

    return true;
}


int main(int argc, char *argv[]) {
    /*if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <Clubs_Folder>\n";
        abort();
    }
    string path(argv[1]);*/
    string path = "../clubs";
    //check path for clubs path
    
    
    vector<string> positions;
    read_positions(path, positions);
    int num_of_positions = positions.size();
    
    //vector<string> countries;
    vector<filesystem::directory_entry> countries;
    read_countries(path, countries);
    int num_of_countries = countries.size();
}