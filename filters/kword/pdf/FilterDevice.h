/*
 * Copyright (c) 2002 Nicolas HADACEK (hadacek@kde.org)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this program; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef FILTERDEVICE_H
#define FILTERDEVICE_H

#include <qdom.h>

class Object;
class Stream;

#include "gtypes.h"
#include "OutputDev.h"

class FilterPage;


class FilterDevice : public OutputDev
{
 public:
    FilterDevice(QDomDocument &, QDomElement &mainFrameset);
    ~FilterDevice();

    virtual GBool upsideDown() { return gTrue; }
    virtual GBool useDrawChar() { return gTrue; }
    virtual GBool interpretType3Chars() { return gFalse; }
    virtual GBool needNonText() { return gFalse; }

    virtual void startPage(int pageNum, GfxState *state);
    virtual void endPage();

    virtual void updateFont(GfxState *state);

    virtual void beginString(GfxState *state, GString *s);
    virtual void endString(GfxState *state);
    virtual void drawChar(GfxState *state, double x, double y,
                          double dx, double dy,
                          double originX, double originY,
                          CharCode c, Unicode *u, int uLen);

    virtual void drawLink(Link* link, Catalog *cat);

 private:
    QDomDocument  _document;
    QDomElement   _mainFrameset;
    FilterPage   *_page;
};

#endif
