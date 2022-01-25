/*
 * Screenshoter project
 *
 * Author: Aleksei Tikhomirov <aleksei.tikhomirov@mail.ru>
 *
 */

#pragma once

#include <X11/Xlib.h>
#include <string>

bool writeBitmapFile(const XImage *image, const std::string &path);

bool writeDiffBitmapFile(const XImage *image1, const XImage *image2,
                         const std::string &path);
