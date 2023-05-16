
#include <iostream>
#include <unistd.h>
#include <fstream>
#include <algorithm>
#include <bits/stdc++.h>
#include <chrono>

#include "const.hpp"

#define NUM_THREADS 16
pthread_t threads[NUM_THREADS];
int threads_start_end_row[NUM_THREADS][2];
int threads_start_end_col[NUM_THREADS][2];

using std::cout;
using std::endl;
using std::ifstream;
using std::ofstream;
using namespace std;

#pragma pack(1)
#pragma once

typedef int LONG;
typedef unsigned short WORD;
typedef unsigned int DWORD;

typedef struct tagBITMAPFILEHEADER
{
  WORD bfType;
  DWORD bfSize;
  WORD bfReserved1;
  WORD bfReserved2;
  DWORD bfOffBits;
} BITMAPFILEHEADER, *PBITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER
{
  DWORD biSize;
  LONG biWidth;
  LONG biHeight;
  WORD biPlanes;
  WORD biBitCount;
  DWORD biCompression;
  DWORD biSizeImage;
  LONG biXPelsPerMeter;
  LONG biYPelsPerMeter;
  DWORD biClrUsed;
  DWORD biClrImportant;
} BITMAPINFOHEADER, *PBITMAPINFOHEADER;

int rows;
int cols;
uint8_t*** input_pic;
//uint8_t*** result_pic;

bool fillAndAllocate(char *&buffer, const char *fileName, int &rows, int &cols, int &bufferSize)
{
  std::ifstream file(fileName);

  if (file)
  {
    file.seekg(0, std::ios::end);
    std::streampos length = file.tellg();
    file.seekg(0, std::ios::beg);

    buffer = new char[length];
    file.read(&buffer[0], length);

    PBITMAPFILEHEADER file_header;
    PBITMAPINFOHEADER info_header;

    file_header = (PBITMAPFILEHEADER)(&buffer[0]);
    info_header = (PBITMAPINFOHEADER)(&buffer[0] + sizeof(BITMAPFILEHEADER));
    rows = info_header->biHeight;
    cols = info_header->biWidth;
    bufferSize = file_header->bfSize;
    return 1;
  }
  else
  {
    cout << "File" << fileName << " doesn't exist!" << endl;
    return 0;
  }
}

void getpixelsFromBMP24(int end, int rows, int cols, char *fileReadBuffer)
{
  int count = 1;
  int extra = cols % 4;
  for (int i = 0; i < rows; i++)
  {
    count += extra;
    for (int j = cols - 1; j >= 0; j--)
      for (int k = 0; k < 3; k++)
      {
        switch (k)
        {
        case 0:
          // fileReadBuffer[end - count] is the red value
          input_pic[i][j][RED] = fileReadBuffer[end - count];
          break;
        case 1:
          // fileReadBuffer[end - count] is the green value
          input_pic[i][j][GREEN] = fileReadBuffer[end - count];
          break;
        case 2:
          // fileReadBuffer[end - count] is the blue value
          input_pic[i][j][BLUE] = fileReadBuffer[end - count];
          break;
        // go to the next position in the buffer
        }
        count++;
      }
  }
}

void writeOutBmp24(char *fileBuffer, const char *nameOfFileToCreate, int bufferSize)
{
  std::ofstream write(nameOfFileToCreate);
  if (!write)
  {
    cout << "Failed to write " << nameOfFileToCreate << endl;
    return;
  }
  int count = 1;
  int extra = cols % 4;
  for (int i = 0; i < rows; i++)
  {
    count += extra;
    for (int j = cols - 1; j >= 0; j--)
      for (int k = 0; k < 3; k++)
      {
        switch (k)
        {
        case 0:
          // write red value in fileBuffer[bufferSize - count]
          fileBuffer[bufferSize - count] = input_pic[i][j][RED];
          break;
        case 1:
          // write green value in fileBuffer[bufferSize - count]
          fileBuffer[bufferSize - count] = input_pic[i][j][GREEN];
          break;
        case 2:
          // write blue value in fileBuffer[bufferSize - count]
          fileBuffer[bufferSize - count] = input_pic[i][j][BLUE];
          break;
        // go to the next position in the buffer
        }
        count++;
      }
  }
  write.write(fileBuffer, bufferSize);
}
void make_pixel_matrix() {
  input_pic = new uint8_t**[rows];

  for(int i = 0; i < rows; i++) {
    input_pic[i] = new uint8_t*[cols];

    for(int j = 0; j < cols; j++) {
      input_pic[i][j] = new uint8_t[3];
    }
  }
}
void* make_pixel_matrix2(void* arg) {
  uint8_t ****pic_ = (uint8_t ****)arg;
  *pic_ = new uint8_t**[rows];
  uint8_t ***pic = *pic_;
  for(int i = 0; i < rows; i++) {
    pic[i] = new uint8_t*[cols];

    for(int j = 0; j < cols; j++) {
      pic[i][j] = new uint8_t[3];
    }
  }
  pthread_exit(NULL);
}

void* horizontial_mirror(void* arg) {
  int thread_first_rows = ((int*)arg)[0];
  int thread_last_rows = ((int*)arg)[1];
  for (int r =thread_first_rows ; r <= thread_last_rows; r++) {
		for (int c = 0; c < cols/2; c++){
       swap(input_pic[r][c],input_pic[r][cols - c -1]);
    }
  }
  pthread_exit(NULL);
}
void* vertical_mirror(void* arg) {
  int thread_first_cols = ((int*)arg)[0];
  int thread_last_cols = ((int*)arg)[1];
    for (int r = 0; r < rows / 2; r++) {
		  for (int c = thread_first_cols; c <= thread_last_cols; c++){
        swap(input_pic[r][c], input_pic[rows - 1 -r][c]); 
      }
  }
  pthread_exit(NULL);
}
void* sharpen(void* arg) {
    int thread_first_rows = ((int*)arg)[0];
    if (thread_first_rows == 0)
        thread_first_rows = 1;

    int thread_last_rows = ((int*)arg)[1];
    if (thread_last_rows == rows - 1)
        thread_last_rows = rows - 2;

 uint8_t out_img[rows][cols][3];
  for (int i = thread_first_rows; i <= thread_last_rows ; i++)
  {
    for (int j = 1; j < cols - 1; j++)
    {
      if (i - 1 < 0 || j - 1 < 0 || i + 1 >= rows || j + 1 >= cols)
        continue;
      int temp = 0;
      temp -= input_pic[i - 1][j][RED];
      temp -= input_pic[i][j - 1][RED];
      temp +=(5 * input_pic[i][j][RED]);
      temp -= input_pic[i][j + 1][RED];
      temp -= input_pic[i + 1][j][RED];

      if (temp > 255)
        out_img[i][j][RED] = 255;
      else if (temp < 0)
        out_img[i][j][RED] = 0;
      else
        out_img[i][j][RED] = temp;

      temp = 0;
      temp -= input_pic[i - 1][j][BLUE];
      temp -= input_pic[i][j - 1][BLUE];
      temp += (5 * input_pic[i][j][BLUE]);
      temp -= input_pic[i][j + 1][BLUE];
      temp -= input_pic[i + 1][j][BLUE];

      if (temp > 255)
        out_img[i][j][BLUE] = 255;
      else if (temp < 0)
        out_img[i][j][BLUE] = 0;
      else
        out_img[i][j][BLUE] = temp;

      temp = 0;
      temp -= input_pic[i - 1][j][GREEN];
      temp -= input_pic[i][j - 1][GREEN];
      temp += (5 * input_pic[i][j][GREEN]);
      temp -= input_pic[i][j + 1][GREEN];
      temp -= input_pic[i + 1][j][GREEN];

      if (temp > 255)
        out_img[i][j][GREEN] = 255;
      else if (temp < 0)
        out_img[i][j][GREEN] = 0;
      else
        out_img[i][j][GREEN] = temp;
    }
    
  }
 for (int i = thread_first_rows; i < thread_last_rows; i++){
    for (int j = 1; j < cols - 1; j++){
      input_pic[i][j] = out_img[i][j];
    }
  }
 

  pthread_exit(NULL);
}
void* sepia(void* arg) {
    int thread_first_cols = ((int*)arg)[0];
    int thread_last_cols = ((int*)arg)[1];
    for (int r = thread_first_cols; r <= thread_last_cols; r++) {
		  for (int c = 0; c < cols; c++) { 
        int red = input_pic[r][c][RED], green = input_pic[r][c][GREEN], blue = input_pic[r][c][BLUE];
        input_pic[r][c][RED]= min(255, (int) (red * T[RED][RED] + green * T[RED][GREEN] + blue * T[RED][BLUE]));
        input_pic[r][c][GREEN] = min(255, (int) (red * T[GREEN][RED] + green * T[GREEN][GREEN] + blue * T[GREEN][BLUE]));
        input_pic[r][c][BLUE] = min(255, (int) (red * T[BLUE][RED] + green * T[BLUE][GREEN] + blue * T[BLUE][BLUE]));
      }
  }
  pthread_exit(NULL);
}
void draw_line(int x1, int y1, int x2, int y2) {
    int dx = x2 - x1;
    int dy = y2 - y1;
    int steps = max(abs(dx), abs(dy));
    float xIncrement = static_cast<float>(dx) / (float) steps;
    float yIncrement = static_cast<float>(dy) / (float) steps;
    auto x = static_cast<float>(x1);
    auto y = static_cast<float>(y1);
       for (int i = 0; i <= steps; ++i) {
          input_pic[(int)y][(int)x][RED] = 255;
          input_pic[(int)y][(int)x][GREEN] = 255;
          input_pic[(int)y][(int)x][BLUE] = 255;

          x += xIncrement;
          y += yIncrement;
       }
    pthread_exit(NULL);   
}
void* draw_X_shape(void* arg) {
  draw_line(0, rows - 1, cols - 1, 0);
  draw_line(cols - 1, rows - 1, 0, 0);
  pthread_exit(NULL);
}

void  calculate_contribution_for_each_thread_ROW() {
    int contribution = floor((double)rows / NUM_THREADS);
    for (int i = 0; i < (NUM_THREADS - 1); i++)
    {
        threads_start_end_row[i][0] = i * contribution;
        threads_start_end_row[i][1] = (i + 1) * contribution - 1;
    }
    threads_start_end_row[NUM_THREADS - 1][0] = (NUM_THREADS - 1) * contribution;
    threads_start_end_row[NUM_THREADS - 1][1] = rows - 1;
  
}
void  calculate_contribution_for_each_thread_COL()
{
    int contribution = floor((double)cols / NUM_THREADS);
    for (int i = 0; i < (NUM_THREADS - 1); i++)
    {
        threads_start_end_col[i][0] = i * contribution;
        threads_start_end_col[i][1] = (i + 1) * contribution - 1;
    }
    threads_start_end_col[NUM_THREADS - 1][0] = (NUM_THREADS - 1) * contribution;
    threads_start_end_col[NUM_THREADS - 1][1] = cols - 1;
  
}
void multi_thread_pro(void* (*filter)(void*), int thread_args[NUM_THREADS][2]) {
  //create
  for (int i = 0; i < NUM_THREADS; i++) {
    pthread_create(&threads[i], NULL, filter, (void*) thread_args[i]);
  }
  //join 
  for (int i = 0; i < NUM_THREADS; i++) {
    pthread_join(threads[i], NULL);
  }
}

void apply_filters() {
  calculate_contribution_for_each_thread_COL();
  calculate_contribution_for_each_thread_ROW();

  multi_thread_pro(&horizontial_mirror, threads_start_end_row);
  multi_thread_pro(&vertical_mirror, threads_start_end_col);
  multi_thread_pro(&sharpen, threads_start_end_row);
  multi_thread_pro(&sepia, threads_start_end_row);
  //multi_thread_pro(&draw_X_shape);
}

int main(int argc, char *argv[])
{
  auto start = chrono::high_resolution_clock::now();
  
  char *fileBuffer;
  int bufferSize;
  char *fileName = argv[1];
  if (!fillAndAllocate(fileBuffer, fileName, rows, cols, bufferSize))
  {
    cout << "File read error" << endl;
    return 1;
  }

  make_pixel_matrix();
  getpixelsFromBMP24(bufferSize, rows, cols, fileBuffer);

  // apply filters
  apply_filters();

  // write output file
  writeOutBmp24(fileBuffer, OUTPUT_FILE, bufferSize);

  // Calculate total time
  auto end = chrono::high_resolution_clock::now();
  
  cout << "Execution time: " << 
    chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
    << " milliseconds" << endl;
    
  return 0;
}