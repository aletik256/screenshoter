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
#include <memory>

constexpr float RED = 0.0;
constexpr float GREEN = 1.0;
constexpr float BLUE = 0.0;

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

void fillBuffer(const char *data1, const char *data2, char *buffer,
                const char *pixelBuffer, int bytesPerLine, int bytesPerPixel) {
  for (int i = 0; i < bytesPerLine;) {
    for (; i < bytesPerLine && data1[i] == data2[i]; i++) {
      buffer[i] = data1[i];
    }
    if (i < bytesPerLine) {
      const auto offset = i - i % bytesPerPixel;
      for (; i < bytesPerLine && data1[i] != data2[i]; i++)
        ;
      if (i < bytesPerLine && i % bytesPerPixel != 0) {
        i += bytesPerPixel - i % bytesPerPixel;
      }
      for (int j = offset; j < i; j += bytesPerPixel) {
        std::memcpy(buffer + j, pixelBuffer, bytesPerPixel);
      }
    }
  }
}

std::unique_ptr<char[]> getPixelBuffer(unsigned long redMask,
                                       unsigned long greenMask,
                                       unsigned long blueMask,
                                       int bytesPerPixel) {
  auto pixelBuffer = std::make_unique<char[]>(bytesPerPixel);
  const auto mask = std::numeric_limits<unsigned long>::max();
  const auto red = static_cast<unsigned long>((mask & redMask) * RED) & redMask;
  const auto green =
      static_cast<unsigned long>((mask & greenMask) * GREEN) & greenMask;
  const auto blue =
      static_cast<unsigned long>((mask & blueMask) * BLUE) & blueMask;
  auto color = red + green + blue;
  for (int i = 0; i < bytesPerPixel; i++) {
    pixelBuffer[i] = static_cast<char>(color);
    color = color >> 8;
  }
  return pixelBuffer;
}

bool writeDiffBitmapFile(const XImage *image1, const XImage *image2,
                         const std::string &path) {
  assert(image1 && image2);

  const auto width = image1->width;
  const auto height = image1->height;
  const auto bits = image1->bits_per_pixel;

  if (width != image2->width || height != image2->height ||
      bits != image2->bits_per_pixel) {
    return false;
  }

  std::ofstream stream(path, std::ios::out);
  stream.exceptions(std::ofstream::badbit | std::ofstream::failbit);
  if (!stream.is_open()) {
    return false;
  }

  writeBitmapHeader(stream, width, height, bits);

  const auto *data1 = image1->data;
  const auto *data2 = image2->data;
  assert(data1 && data2);

  const auto bytesPerPixel = bits / 8;
  const auto bytesPerLine = image1->bytes_per_line;
  const auto paddingSize = (4 - bytesPerLine % 4) % 4;
  const auto bufferSize = bytesPerLine + paddingSize;

  const auto buffer = std::make_unique<char[]>(bufferSize);
  std::memset(buffer.get() + bytesPerLine, 0, paddingSize);

  const auto redMask = image1->red_mask;
  const auto greenMask = image1->green_mask;
  const auto blueMask = image1->blue_mask;
  const auto pixelBuffer =
      getPixelBuffer(redMask, greenMask, blueMask, bytesPerPixel);

  for (int y = height - 1; y >= 0; y--) {
    const auto offset = y * bytesPerLine;
    fillBuffer(data1 + offset, data2 + offset, buffer.get(), pixelBuffer.get(),
               bytesPerLine, bytesPerPixel);
    stream.write(buffer.get(), bufferSize);
  }

  return true;
}
