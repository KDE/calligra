/*
 *  kis_global.h - part of KImageShop
 *
 *  Copyright (c) 2000 Matthias Elter <elter@kde.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef __kis_global_h__
#define __kis_global_h__

// size for graphic blocks - must be a power of 2
const int TILE_SIZE = 64;

enum ActiveColor { NONE, FOREGROUND, BACKGROUND};

// color spaces
enum cSpace { INDEXED, RGB, HSV, CMYK, LAB };

// color modes
enum cMode { M_INDEXED, M_GREYSCALE, M_RGB, M_RGBA, M_CMYK, M_CMYKA, M_LAB, M_LABA };

#endif
