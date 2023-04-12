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

vector<Positions_Data> init_position_data(const vector<string> &selected_pos) {
    vector<Positions_Data> positions_datas;
    Positions_Data new_pos;
    for (int i = 0; i < selected_pos.size(); i++) {    
        new_pos.name = selected_pos[i];
        new_pos.count = 0;
        new_pos.max_age = -1;
        new_pos.min_age = 1000;
        new_pos.sum_age = 0;
        positions_datas.push_back(new_pos);
    }
    return positions_datas;
}

void pasre_csv_file(const string &path, const string &csv_file_name, vector<Positions_Data> &pos_data) {
    string file_name = path + "/" + csv_file_name;
    ifstream file(file_name);
    if (!file.is_open()) {
        cerr <<  "Failed to open file " << file_name.c_str() << "\n";
        exit(EXIT_FAILURE);
    }
    
    string line;
    vector<string> out ;
    while(getline(file, line)) {
        out = split(line, ',');
        for(int i = 0; i < pos_data.size(); i++) {
            if (pos_data[i].name == out[1]) {
                int age = stoi(out[2]);
                pos_data[i].max_age = max(pos_data[i].max_age, age);
                pos_data[i].min_age = min(pos_data[i].min_age, age);
                pos_data[i].sum_age += age;
                pos_data[i].count ++;
            }
        }
    }
    file.close();
}

void send_data_to_named_pipe(const string &myfifo, const Positions_Data &pos_data) {
    
    //string path = NAMED_PIPES_FOLDER + myfifo;
    //cout << path;
    mkfifo(myfifo.c_str(), 0666);
    int fd_named_pipe = open(myfifo.c_str(), O_WRONLY);
    string all_data = "";
    all_data += to_string(pos_data.min_age) + ',' + to_string(pos_data.max_age);
    all_data += "," + to_string(pos_data.sum_age) + "," + to_string(pos_data.count);
    write(fd_named_pipe, all_data.c_str(), all_data.length());
    close(fd_named_pipe);
}

int main(int argc, char *argv[]) {
    int fd_unnamed_pipe_from_country = stoi(argv[3]);
    string path = string(argv[1]);
    string club_name = string(argv[2]);

    char buffer[MESSAGE_SIZE];
    memset(buffer, 0, MESSAGE_SIZE);
    
    //read from pipe -> get selected positions
    read(fd_unnamed_pipe_from_country, buffer, sizeof(buffer));
    vector<string> selected_pos = split(string(buffer), ',');
    int num_of_selected_pos = selected_pos.size();
    close(fd_unnamed_pipe_from_country);

    //parse CSV file
    vector<Positions_Data> pos_data = init_position_data(selected_pos);
    pasre_csv_file(path, club_name, pos_data);
    
    //send data of this club to position procs:
    for (int i = 0; i < num_of_selected_pos; i ++) {
        string myfifo = club_name + "_to_" + selected_pos[i];
        for(int j = 0; j < pos_data.size(); j++) {
            if (pos_data[j].name == selected_pos[i]) {
                send_data_to_named_pipe(myfifo, pos_data[j]);
                break;
            }
        }
        
    }

    while(wait(NULL) > 0); 
    return EXIT_SUCCESS;
}