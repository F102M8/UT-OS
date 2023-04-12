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

void read_clubs(const string &country_folder, vector<string> &clubs_name) {
    for (const auto &entry: filesystem::directory_iterator(country_folder)) {
        if(entry.path().filename() != ALL_POSITIONS_FILE) {
            clubs_name.push_back(entry.path().filename());
        }
    }
}


int main(int argc, char *argv[]) {
    string path = string(argv[1]);
    cout << "country  from path: " << path << " opend\n";

    int fd_unnamed_pipe_from_main = stoi(argv[2]);
    
    char buffer[MASSAGE_SIZE];
    memset(buffer, 0, MASSAGE_SIZE);

    //read from pipe -> get selected positions
    read(fd_unnamed_pipe_from_main, buffer, sizeof(buffer));
    vector<string> selected_pos = split(string(buffer), ',');
    int num_of_selected_pos = selected_pos.size();
    close(fd_unnamed_pipe_from_main);

    //get clubs CSV_files
    vector<string> clubs_names;
    read_clubs(path, clubs_names);
    int num_of_clubs = clubs_names.size();
    //for (string club : clubs_names) { cout << club << "\n";}

    //create pipes 
    int fd_unnamed_pipes_country_to_club[num_of_clubs][2];
    for (int i = 0; i < num_of_clubs; i++) {
        pipe(fd_unnamed_pipes_country_to_club[i]);
        //cout << fd_unnamed_pipes_country_to_club[i][0] << "-" << fd_unnamed_pipes_main_to_country[i][1] << "\n";
    }

    for(int i = 0; i < num_of_clubs; i++) {
        int pid = fork();
        
        if (pid > 0) {
            string buffer;
            buffer = "";
            buffer += selected_pos[0];
            for (int i = 1; i < num_of_selected_pos; i++) {
                buffer += "," + selected_pos[i];
            }
            write(fd_unnamed_pipes_country_to_club[i][1], buffer.c_str(), buffer.length());
            close(fd_unnamed_pipes_country_to_club[i][1]);
        }
        else if (pid == 0) {
            string exec_file = EXECUTABLE_FILE_CLUB;
            string fd_pipe = to_string(fd_unnamed_pipes_country_to_club[i][0]);
            char* arguments[] = {(char*)exec_file.c_str(), (char*)path.c_str(), (char*)clubs_names[i].c_str(), (char*)fd_pipe.c_str(), NULL};
            execv(exec_file.c_str(), arguments);
            return EXIT_SUCCESS;
        }
        else {
            cerr << "Fork Failed\n";
            return EXIT_FAILURE;
        }
    }

    while(wait(NULL) > 0); 
    return EXIT_SUCCESS;
}