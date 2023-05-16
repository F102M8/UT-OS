// use class for img and filters?!
//change time-cal method
#include <iostream>
#include <unistd.h>
#include <fstream>
#include <algorithm>
#include <bits/stdc++.h>
#include <chrono>

#include "const.hpp"

#define NUM_OF_THREADS 8
pthread_t threads[NUM_OF_THREADS];


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

void* make_pixel_matrix(void* arg) {
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

void horizontial_mirror() {
  for (int r = 0; r < rows; r++) {
		for (int c = 0; c < cols/2; c++){
      swap(input_pic[r][c], input_pic[r][cols - 1 - c]);  
    }
  }
  return;
}
void vertical_mirror() {
    for (int r = 0; r < rows / 2; r++) {
		  for (int c = 0; c < cols; c++){
        swap(input_pic[r][c], input_pic[rows - 1 -r][c]); 
      }
  }
  return;
}
void sharpen() {
 uint8_t out_img[rows][cols][3];
  for (int i = 1; i < rows - 1; i++)
  {
    for (int j = 1; j < cols - 1; j++)
    {
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

  for (int i = 1; i < rows - 1; i++){
    for (int j = 1; j < cols - 1; j++){
      input_pic[i][j] = out_img[i][j];
    }
  }
  
}
void sepia() {
    for (int r = 0; r < rows; r++) {
		  for (int c = 0; c < cols; c++) { 
        int red = input_pic[r][c][RED], green = input_pic[r][c][GREEN], blue = input_pic[r][c][BLUE];
        input_pic[r][c][RED]= min(255, (int) (red * T[RED][RED] + green * T[RED][GREEN] + blue * T[RED][BLUE]));
        input_pic[r][c][GREEN] = min(255, (int) (red * T[GREEN][RED] + green * T[GREEN][GREEN] + blue * T[GREEN][BLUE]));
        input_pic[r][c][BLUE] = min(255, (int) (red * T[BLUE][RED] + green * T[BLUE][GREEN] + blue * T[BLUE][BLUE]));
      }
  }
  return;
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
}
void draw_X_shape() {
  draw_line(0, rows - 1, cols - 1, 0);
  draw_line(cols - 1, rows - 1, 0, 0);
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
  
  // read input file
  //create 1.read - 2.apply filter
  pthread_t threads_for_create[2];
  pthread_create(&threads_for_create[0], NULL, &make_pixel_matrix,(void*) &input_pic);
  //join
  pthread_join(threads_for_create[0], NULL);
  getpixelsFromBMP24(bufferSize, rows, cols, fileBuffer);


  // apply filters


  // write output file
  writeOutBmp24(fileBuffer, OUTPUT_FILE, bufferSize);

  // Calculate total time
  auto end = chrono::high_resolution_clock::now();
  
  cout << "Execution time: " << 
    chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
    << " milliseconds" << endl;
    
  return 0;
}