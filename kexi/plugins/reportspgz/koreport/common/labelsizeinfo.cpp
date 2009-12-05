/*
 * OpenRPT report writer and rendering engine
 * Copyright (C) 2001-2007 by OpenMFG, LLC
 * Copyright (C) 2007-2008 by Adam Pigg (adam@piggz.co.uk)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 * Please contact info@openmfg.com with any questions on this license.
 */

#include "labelsizeinfo.h"

static LabelSizeInfo s_labels[] = {
//"LABEL_NAME","Paper Size","COLUMNS","ROWS","WIDTH","HEIGHT","STARTXOFFSET","STARTYOFFSET","HORIZONTALGAP","VERTICALGAP"
    LabelSizeInfo("Avery 5263", "Letter", 2, 5, 400, 200, 25, 50, 0, 0),
    LabelSizeInfo("Avery 5264", "Letter", 2, 3, 400, 333, 25, 75, 0, 0),
    LabelSizeInfo("Avery 8460", "Letter", 3, 10, 262, 100, 32, 50, 0, 0),
    LabelSizeInfo("CILS ALP1-9200-1", "Letter", 3, 7, 200, 100, 62, 62, 81, 50),
    LabelSizeInfo()               // Null Label Size
};

LabelSizeInfo LabelSizeInfo::find(const QString & name)
{
    int i = 0;
    while (!s_labels[i].isNull() && s_labels[i]._name != name)
        i++;
    return s_labels[i];
}

QStringList LabelSizeInfo::labelNames()
{
    QStringList l;
    for (int i = 0; !s_labels[i].isNull(); i++)
        l.append(s_labels[i]._name);
    return l;
}

LabelSizeInfo::LabelSizeInfo(const QString & n, const QString & p, int c,
                             int r, int w, int h, int sx, int sy, int xg,
                             int yg)
{
    _name = n;
    _paper = p;

    _columns = c;
    _rows = r;

    _width = w;
    _height = h;

    _startx = sx;
    _starty = sy;

    _xgap = xg;
    _ygap = yg;

    _null = FALSE;
}

LabelSizeInfo::LabelSizeInfo()
{
    _columns = 0;
    _rows = 0;

    _width = 0;
    _height = 0;

    _startx = 0;
    _starty = 0;

    _xgap = 0;
    _ygap = 0;

    _null = TRUE;
}

LabelSizeInfo::~LabelSizeInfo()
{
}

QString LabelSizeInfo::name() const
{
    return _name;
}

QString LabelSizeInfo::paper() const
{
    return _paper;
}

int LabelSizeInfo::columns() const
{
    return _columns;
}
int LabelSizeInfo::rows() const
{
    return _rows;
}

int LabelSizeInfo::width() const
{
    return _width;
}

int LabelSizeInfo::height() const
{
    return _height;
}

int LabelSizeInfo::startX() const
{
    return _startx;
}

int LabelSizeInfo::startY() const
{
    return _starty;
}

int LabelSizeInfo::xGap() const
{
    return _xgap;
}

int LabelSizeInfo::yGap() const
{
    return _ygap;
}

bool LabelSizeInfo::isNull() const
{
    return _null;
}
