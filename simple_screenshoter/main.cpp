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

int main() {
  auto *display = XOpenDisplay(nullptr);
  assert(display);

  const auto root = DefaultRootWindow(display);
  XMapRaised(display, root);

  const auto width = DisplayWidth(display, 0);
  const auto height = DisplayHeight(display, 0);

  auto *image =
      XGetImage(display, root, 0, 0, width, height, AllPlanes, ZPixmap);
  if (!image || !writeBitmapFile(image, "screen.bmp")) {
    return -1;
  }

  return 0;
}
