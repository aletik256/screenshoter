/*
 * Screenshoter project
 *
 * Author: Aleksei Tikhomirov <aleksei.tikhomirov@mail.ru>
 *
 */

#include "bitmap_from_image.h"
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <cassert>
#include <chrono>
#include <thread>

constexpr int SCREENSHOTS_NUMBER = 5;

XImage *getImageCopy(Display *display, Visual *visual, XImage *image) {
  assert(image);
  return XCreateImage(display, visual, image->depth, image->format,
                      image->xoffset, image->data, image->width, image->height,
                      image->bitmap_pad, image->bytes_per_line);
}

int main() {
  auto *display = XOpenDisplay(nullptr);
  assert(display);

  const auto root = DefaultRootWindow(display);
  const auto visual = DefaultVisual(display, 0);
  XMapRaised(display, root);

  const auto width = DisplayWidth(display, 0);
  const auto height = DisplayHeight(display, 0);

  XImage *prevImage = nullptr;
  for (int i = 0; i < SCREENSHOTS_NUMBER; i++) {
    auto *currentImage =
        XGetImage(display, root, 0, 0, width, height, AllPlanes, ZPixmap);
    const auto currentPath =
        std::string("screen_") + std::to_string(i) + ".bmp";
    if (!currentImage || !writeBitmapFile(currentImage, currentPath)) {
      return -1;
    }
    if (prevImage) {
      const auto currentDiffPath = std::string("diff_") +
                                   std::to_string(i - 1) + "_" +
                                   std::to_string(i) + ".bmp";
      if (!writeDiffBitmapFile(prevImage, currentImage, currentDiffPath)) {
        return -1;
      }
      XDestroyImage(prevImage);
    }
    prevImage = getImageCopy(display, visual, currentImage);
    std::this_thread::sleep_for(std::chrono::seconds(10));
  }

  if (prevImage) {
    XDestroyImage(prevImage);
  }
  return 0;
}
