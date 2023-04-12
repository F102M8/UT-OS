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

void read_positions_file(const string &posintions_file_folder, vector<string> &positions) {
    string posintions_file_path = posintions_file_folder + "/" + ALL_POSITIONS_FILE;
    ifstream file(posintions_file_path);
    if (!file.is_open()) {
        cerr <<  "Failed to open file " << posintions_file_path.c_str() << "\n";
        abort();
    }
    string line;
    getline(file, line);
    positions = split(line, ',');
    file.close();
}

void read_countries(const string &countries_file_folder, vector<string> &countries_name) {
    for (const auto &entry: filesystem::directory_iterator(countries_file_folder)) {
        if(entry.path().filename() != ALL_POSITIONS_FILE) {
            countries_name.push_back(entry.path().filename());
        }
    }
}

bool valid_clubs_path(const string &path) {
    return true;
}

void print_all_positions(const vector<string> &positions) {
    cout << "All positions :\n";
    cout << positions[0];
    for (int i = 1; i < positions.size(); i++) {
        cout << " - " << positions[i];
    }
    cout << "\n";
}

void get_selected_pos(vector<string> &selected_pos) {
    cout << "Enter positions to get stats :\n";
    string line;
    getline(cin, line);
    selected_pos = split(line, ' ');
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <clubs folder name>\n";
        exit(EXIT_FAILURE);
    }
    string path(argv[1]);
    
    //check path for clubs path

    //get all positions:
    vector<string> positions;
    read_positions_file(path, positions);
    int num_of_positions = positions.size();
    
    //print all positions:
    print_all_positions(positions);

    //get selected positions from user
    vector<string> selected_pos;
    get_selected_pos(selected_pos);
    int num_of_selected_pos = selected_pos.size();
    //for (string pos: selected_pos) { cout << pos << "\n";}

    //get countries:
    vector<string> countries_name;
    //vector<filesystem::directory_entry> countries;
    read_countries(path, countries_name);
    int num_of_countries = countries_name.size();

    //create pipes 
    int fd_unnamed_pipes_main_to_country[num_of_countries][2];
    for (int i = 0; i < num_of_countries; i++) {
        pipe(fd_unnamed_pipes_main_to_country[i]);
        //cout << fd_unnamed_pipes_main_to_country[i][0] << "-" << fd_unnamed_pipes_main_to_country[i][1] << "\n";
    }

    
    for(int i = 0; i < num_of_countries; i++) {
        int pid = fork();
        
        if (pid > 0) {
            string buffer;
            buffer = "";
            buffer += selected_pos[0];
            for (int i = 1; i < num_of_selected_pos; i++) {
                buffer += "," + selected_pos[i];
            }
            write(fd_unnamed_pipes_main_to_country[i][1], buffer.c_str(), buffer.length());
            close(fd_unnamed_pipes_main_to_country[i][1]);
        }
        else if (pid == 0) {
            string exec_file = EXECUTABLE_FILE_COUNTRY;
            string fd_pipe = to_string(fd_unnamed_pipes_main_to_country[i][0]);
            string country_folder_path = path + '/' + countries_name[i];
            char* arguments[] = {(char*)exec_file.c_str(),(char*)country_folder_path.c_str(), (char*)fd_pipe.c_str(), NULL};
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