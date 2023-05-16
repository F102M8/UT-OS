using namespace std;
 

#define OUTPUT_FILE  "output.bmp" 

const int RED = 0;
const int GREEN = 1;
const int BLUE = 2;

const float T[3][3] = {{0.393, 0.769, 0.189},
                    {0.349, 0.686, 0.168},
                    {0.272, 0.534, 0.131}} ;

const vector<vector<double>> SHARPEN_KERNEL= 
    {
      {0, -1, 0},
      {-1, 5, -1},
      {0, -1, 0}
    };

