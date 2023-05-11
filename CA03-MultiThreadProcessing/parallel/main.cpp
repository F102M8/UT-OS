// use class for img and filters?!

#include <iostream>
#include <unistd.h>
#include <fstream>
#include <algorithm>
#include <bits/stdc++.h>


#include "const.hpp"

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
uint8_t*** pixel_matrix;

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

void getPixlesFromBMP24(int end, int rows, int cols, char *fileReadBuffer)
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
          pixel_matrix[i][j][RED] = fileReadBuffer[end - count];
          break;
        case 1:
          // fileReadBuffer[end - count] is the green value
          pixel_matrix[i][j][GREEN] = fileReadBuffer[end - count];
          break;
        case 2:
          // fileReadBuffer[end - count] is the blue value
          pixel_matrix[i][j][BLUE] = fileReadBuffer[end - count];
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
          fileBuffer[bufferSize - count] = pixels[i][j][RED];
          break;
        case 1:
          // write green value in fileBuffer[bufferSize - count]
          fileBuffer[bufferSize - count] = pixels[i][j][GREEN];
          break;
        case 2:
          // write blue value in fileBuffer[bufferSize - count]
          fileBuffer[bufferSize - count] = pixels[i][j][BLUE];
          break;
        // go to the next position in the buffer
        }
      }
  }
  write.write(fileBuffer, bufferSize);
}

void make_pixel_matrix() {
  pixel_matrix = new uint8_t**[rows];

  for(int i = 0; i < rows; i++) {
    pixel_matrix[i] = new uint8_t*[cols];

    for(int j = 0; j < cols; j++) {
      pixel_matrix[i][j] = new uint8_t[3];
    }
  }
}

void horizontal_mirror() {
  for (int r = 0; r < rows; r++) {
    for (int c = 0; c < cols / 2; c++) {
      swap(pixel_matrix[r][c], pixel_matrix[rows - r - 1][c]);
    }
  }
}

void apply_filters() {
  horizontal_mirror();
}

int main(int argc, char *argv[])
{
  char *fileBuffer;
  int bufferSize;
  char *fileName = argv[1];
  if (!fillAndAllocate(fileBuffer, fileName, rows, cols, bufferSize))
  {
    cout << "File read error" << endl;
    return 1;
  }

  time_t start, end, end_read, end_apply_filters, end_generate_output;
  time(&start);
  make_pixel_matrix();
  
  // read input file
  
  getPixlesFromBMP24(bufferSize, rows, cols, fileBuffer);


  // apply filters
  apply_filters();

  // write output file
  writeOutBmp24(fileBuffer, "output.bmp", bufferSize);

  // Calculate total time
  time(&end);
  double time_taken = double(end - start);
    cout << "Total time taken by program is : " << fixed << time_taken << setprecision(5);
    cout << " sec " << endl;
  return 0;
}