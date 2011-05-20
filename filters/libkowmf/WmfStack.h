/* This file is part of the KDE libraries
   Copyright (c) 1998 Stefan Taferner
                    2001/2003 thierry lorthiois (lorthioist@wanadoo.fr)

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/
#ifndef _WMFSTACK_H_
#define _WMFSTACK_H_

#include <QPen>
#include <QColor>
#include <QFont>
#include <QBrush>
#include <QPixmap>

class WmfAbstractBackend;

/**
 * WMF file allows manipulation on a stack of object.
 * It's possible to create, delete or select an object.
 */
class KoWmfHandle
{
public:
    virtual ~KoWmfHandle() {}
    virtual void apply(WmfAbstractBackend *) = 0;
};

class KoWmfBrushHandle: public KoWmfHandle
{
public:
    virtual void apply(WmfAbstractBackend *);
    QBrush brush;
};

class KoWmfPenHandle: public KoWmfHandle
{
public:
    virtual void apply(WmfAbstractBackend *);
    QPen pen;
};

class KoWmfPatternBrushHandle: public KoWmfHandle
{
public:
    virtual void apply(WmfAbstractBackend *);
    QBrush brush;
    QPixmap image;
};

class KoWmfFontHandle: public KoWmfHandle
{
public:
    virtual void apply(WmfAbstractBackend *);
    QFont font;
    int rotation;
    int height;                 // Can be negative. In 'font' above, we store the absolute value.
};

#endif
