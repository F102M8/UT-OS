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

void init_pos_data(Positions_Data &pos_data, const string &name) {
    pos_data.name = name;
    pos_data.max_age = -1;
    pos_data.min_age = 1000;
    pos_data.count = 0;
    pos_data.sum_age = 0;
}

void print_result(const Positions_Data &pos_data) {
    if (pos_data.count > 0) {
        char float_avg[100];
        sprintf(float_avg, "%.1f", float(pos_data.sum_age) / float(pos_data.count));
        string result = "";
        result += pos_data.name + " min age : " + to_string(pos_data.min_age) + "\n";
        result += pos_data.name + " max age : " + to_string(pos_data.max_age) + "\n"; 
        result += pos_data.name + " avg age : " + float_avg + "\n";
        result +=pos_data.name + " count age : " + to_string(pos_data.count) + "\n";
        cout << result;
        /*cout << pos_data.name << " min age : " << pos_data.min_age <<"\n" <<
         pos_data.name << " max age : " << pos_data.max_age <<"\n" << 
         pos_data.name << " avg age : " << float_avg <<"\n" << 
         pos_data.name << " count age : " << pos_data.count <<"\n" ;*/
    }
    else {
        cout << pos_data.name << " : NOT FOUND...! \n";
    }
}

void update_data(const string &new_data, Positions_Data &pos_data) {
    vector<string> datas = split(new_data, ',');
    
    pos_data.min_age = min(pos_data.min_age, stoi(datas[MIN]));
    pos_data.max_age = max(pos_data.max_age, stoi(datas[MAX]));
    pos_data.sum_age += stoi(datas[SUM]);
    pos_data.count += stoi(datas[COUNT]);
}
void get_data_from_named_pipe(const string &myfifo, Positions_Data &pos_data ) {
    mkfifo(myfifo.c_str(), 0666);

    int fd_named_pipe = open(myfifo.c_str(), O_RDONLY);

    char buffer[MESSAGE_SIZE];
    memset(buffer, 0, sizeof(buffer));

    read(fd_named_pipe, buffer, sizeof(buffer));

    update_data(string(buffer), pos_data);

    close(fd_named_pipe);
}



int main(int argc, char *argv[]) {
    string position = string(argv[1]);
    int fd_unnamed_pipe_from_main= stoi(argv[2]);

    char buffer[MESSAGE_SIZE];
    memset(buffer, 0, MESSAGE_SIZE);
   
    //read from pipe -> get clubs_names
    read(fd_unnamed_pipe_from_main, buffer, sizeof(buffer));
    vector<string> clubs_name = split(string(buffer), ',');

    Positions_Data pos_data;
    init_pos_data(pos_data, position);
    
    for (int i = 0; i < clubs_name.size(); i++) {
        string myfifo = clubs_name[i] + "_to_" + position;
        //string new_data = 
        get_data_from_named_pipe(myfifo, pos_data);
        //update_data(new_data, pos_data);
    }
    

    print_result(pos_data);
     
    return EXIT_SUCCESS;
}