/*
 *  kis_cursor.cc - part of KImageShop
 *
 *  Copyright (c) 1999 Matthias Elter <elter@kde.org>
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

#include <qbitmap.h>
#include <qcursor.h>

#include "kis_cursor.h"

KImageShopCursor::KImageShopCursor() {}

/*
 * Predefined Qt cursors
 */
QCursor KImageShopCursor::arrowCursor()
{
  return Qt::arrowCursor;
}

QCursor KImageShopCursor::upArrowCursor()
{
  return Qt::upArrowCursor;
}

QCursor KImageShopCursor::crossCursor()
{
  return Qt::crossCursor;
}

QCursor KImageShopCursor::waitCursor()
{
  return Qt::waitCursor;
}

QCursor KImageShopCursor::ibeamCursor()
{
  return Qt::ibeamCursor;
}

QCursor KImageShopCursor::sizeVerCursor()
{
  return Qt::sizeVerCursor;
}

QCursor KImageShopCursor::sizeHorCursor()
{
  return Qt::sizeHorCursor;
}

QCursor KImageShopCursor::sizeBDiagCursor()
{
  return Qt::sizeBDiagCursor;
}

QCursor KImageShopCursor::sizeFDiagCursor()
{
  return Qt::sizeFDiagCursor;
}

QCursor KImageShopCursor::sizeAllCursor()
{
  return Qt::sizeAllCursor;
}

QCursor KImageShopCursor::blankCursor()
{
  return Qt::blankCursor;
}

QCursor KImageShopCursor::splitVCursor()
{
  return Qt::splitVCursor;
}

QCursor KImageShopCursor::splitHCursor()
{
  return Qt::splitHCursor;
}

QCursor KImageShopCursor::pointingHandCursor()
{
  return Qt::pointingHandCursor;
}

/*
 * Custom KImageShop cursors. Use the X utility "bitmap" to create new cursors.
 */

QCursor KImageShopCursor::pickerCursor()
{
  static QCursor *picker_cursor = 0;
  
  unsigned char picker_bits[] = {
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f, 0xfc,
    0xff, 0x3f, 0xf8, 0xff, 0x3f, 0xf4, 0xff, 0x87, 0xf7, 0xff, 0x6f, 0xf7,
    0xff, 0x97, 0xf8, 0xff, 0x8b, 0xfe, 0xff, 0x65, 0xfe, 0xff, 0xb2, 0xfe,
    0x7f, 0xd9, 0xff, 0xbf, 0xec, 0xff, 0x5f, 0xf6, 0xff, 0x2f, 0xfb, 0xff,
    0x97, 0xfd, 0xff, 0xcb, 0xfe, 0xff, 0x6b, 0xff, 0xff, 0xbd, 0xff, 0xff,
    0xc7, 0xff, 0xff, 0xf7, 0xff, 0xff};
  
  unsigned char picker_mask_bits[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x03, 0x00, 0xc0, 0x07,
    0x00, 0xe0, 0x0f, 0x00, 0xf8, 0x1f, 0x00, 0xfc, 0x1f, 0x00, 0xf8, 0x1f,
    0x00, 0xf8, 0x0f, 0x00, 0xfc, 0x07, 0x00, 0xfe, 0x03, 0x00, 0xff, 0x03,
    0x80, 0x3f, 0x00, 0xc0, 0x1f, 0x00, 0xe0, 0x0f, 0x00, 0xf0, 0x07, 0x00,
    0xf8, 0x03, 0x00, 0xfc, 0x01, 0x00, 0xfc, 0x00, 0x00, 0x7e, 0x00, 0x00,
    0x3c, 0x00, 0x00, 0x08, 0x00, 0x00 };
  
  QBitmap picker_bitmap(22, 22, picker_bits, true); 
  QBitmap picker_mask(22, 22, picker_mask_bits, true); 
  picker_cursor = new QCursor(picker_bitmap, picker_mask, 2, 20);
  
  CHECK_PTR(picker_cursor);
  return *picker_cursor;
}

QCursor KImageShopCursor::penCursor()
{
  // TODO
  return Qt::arrowCursor;
}

QCursor KImageShopCursor::brushCursor()
{
  // TODO
  return Qt::arrowCursor;
}

QCursor KImageShopCursor::moveCursor()
{
  // TODO
  return Qt::arrowCursor;
}
