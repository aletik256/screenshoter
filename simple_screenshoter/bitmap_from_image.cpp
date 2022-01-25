/*
 * Screenshoter project
 *
 * Author: Aleksei Tikhomirov <aleksei.tikhomirov@mail.ru>
 *
 */

#include "bitmap_from_image.h"
#include <cassert>
#include <cstring>
#include <fstream>

void writeBitmapHeader(std::ofstream &stream, int width, int height, int bits) {
  char fileHeader[14];
  char bitmapHeader[40];

  const auto fileSize =
      sizeof(fileHeader) + sizeof(bitmapHeader) + width * height * bits / 8;

  std::memset(fileHeader, 0, sizeof(fileHeader));
  fileHeader[0] = 'B';
  fileHeader[1] = 'M';
  fileHeader[2] = static_cast<char>(fileSize);
  fileHeader[3] = static_cast<char>(fileSize >> 8);
  fileHeader[4] = static_cast<char>(fileSize >> 16);
  fileHeader[5] = static_cast<char>(fileSize >> 24);
  fileHeader[10] = static_cast<char>(sizeof(fileHeader) + sizeof(bitmapHeader));

  std::memset(bitmapHeader, 0, sizeof(bitmapHeader));
  bitmapHeader[0] = static_cast<char>(sizeof(bitmapHeader));
  bitmapHeader[4] = static_cast<char>(width);
  bitmapHeader[5] = static_cast<char>(width >> 8);
  bitmapHeader[6] = static_cast<char>(width >> 16);
  bitmapHeader[7] = static_cast<char>(width >> 24);
  bitmapHeader[8] = static_cast<char>(height);
  bitmapHeader[9] = static_cast<char>(height >> 8);
  bitmapHeader[10] = static_cast<char>(height >> 16);
  bitmapHeader[11] = static_cast<char>(height >> 24);
  bitmapHeader[12] = 1;
  bitmapHeader[14] = static_cast<char>(bits);

  stream.write(fileHeader, sizeof(fileHeader));
  stream.write(bitmapHeader, sizeof(bitmapHeader));
}

bool writeBitmapFile(const XImage *image, const std::string &path) {
  assert(image);

  const auto width = image->width;
  const auto height = image->height;
  const auto bits = image->bits_per_pixel;

  std::ofstream stream(path, std::ios::out);
  stream.exceptions(std::ofstream::badbit | std::ofstream::failbit);
  if (!stream.is_open()) {
    return false;
  }

  writeBitmapHeader(stream, width, height, bits);

  const auto *data = image->data;
  assert(data);

  const auto bytesPerLine = image->bytes_per_line;
  const char padding[3] = {0, 0, 0};
  const auto paddingSize = (4 - bytesPerLine % 4) % 4;

  for (int y = height - 1; y >= 0; y--) {
    stream.write(data + y * bytesPerLine, bytesPerLine);
    stream.write(padding, paddingSize);
  }

  return true;
}
