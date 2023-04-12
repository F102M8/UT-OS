#include "../include/const.hpp"
vector<string> split(const string &str, char delim) {
    vector<string> elements;
    istringstream ss(str);
    string item;
    while (getline(ss, item, delim)) {
        item.erase(0, item.find_first_not_of(' '));
        item.erase(item.find_last_not_of(' ') + 1);
        if (!item.empty())
            elements.push_back(item);
    }
    return elements;
}



int main(int argc, char *argv[]) {
    //cout << "country opend pipe_id: " << argv[1] << "\n";

    int fd_unnamed_pipe_from_main = stoi(argv[1]);
    
    char buffer[MASSAGE_SIZE];
    memset(buffer, 0, MASSAGE_SIZE);

    //read from pipe -> get selected positions
    read(fd_unnamed_pipe_from_main, buffer, sizeof(buffer));
    vector<string> selected_pos = split(string(buffer), ',');
    int num_selected_pos = selected_pos.size();
    close(fd_unnamed_pipe_from_main);

    //get clubs CSV_files
    vector<string> clubs_names;

    int num_of_clubs = clubs_names.size();
}