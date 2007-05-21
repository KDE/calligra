/* libwpg
 * Copyright (C) 2007 Ariya Hidayat (ariya@kde.org)
 * Copyright (C) 2006 Ariya Hidayat (ariya@kde.org)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA  02111-1301 USA
 *
 * For further information visit http://libwpg.sourceforge.net
 */

/* "This product is not manufactured, approved, or supported by
 * Corel Corporation or Corel Corporation Limited."
 */

#include "WPGBitmap.h"

class libwpg::WPGBitmap::Private
{
public:
	int width;
	int height;
	WPGColor* pixels;

	Private(int w, int h): width(w), height(h), pixels(0) {}
};

libwpg::WPGBitmap::WPGBitmap(int width, int height): rect(), d(new Private(width, height))
{
	d->pixels = new WPGColor[width*height];
}

libwpg::WPGBitmap::~WPGBitmap()
{
	delete d->pixels;
	delete d;
}

libwpg::WPGBitmap::WPGBitmap(const WPGBitmap& bitmap): d(new Private(0,0))
{
	copyFrom(bitmap);
}

libwpg::WPGBitmap& libwpg::WPGBitmap::operator=(const WPGBitmap& bitmap)
{
	copyFrom(bitmap);
	return *this;
}

void libwpg::WPGBitmap::copyFrom(const WPGBitmap& bitmap)
{
	rect = bitmap.rect;
	d->width = bitmap.d->width;
	d->height = bitmap.d->height;
	delete d->pixels;
	d->pixels = new WPGColor[d->width*d->height];
	for(int i=0; i < d->width*d->height; i++)
		d->pixels[i] = bitmap.d->pixels[i];
}

const int libwpg::WPGBitmap::width() const
{
	return d->width;
}

const int libwpg::WPGBitmap::height() const
{
	return d->height;
}

libwpg::WPGColor libwpg::WPGBitmap::pixel(int x, int y) const
{
  if((x < 0) || (y <0) || (x >= d->width) || (y >= d->height))
		return libwpg::WPGColor();

	return d->pixels[y*d->width + x];
}

void libwpg::WPGBitmap::setPixel(int x, int y, const libwpg::WPGColor& color)
{
  if((x < 0) || (y <0) || (x >= d->width) || (y >= d->height))
		return;

	d->pixels[y*d->width + x] = color;
}
