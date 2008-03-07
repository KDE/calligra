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

static LabelSizeInfo __labels[] = {
//"LABEL_NAME","Paper Size","COLUMNS","ROWS","WIDTH","HEIGHT","STARTXOFFSET","STARTYOFFSET","HORIZONTALGAP","VERTICALGAP"
  LabelSizeInfo("Avery 5263", "Letter", 2, 5, 400, 200, 25, 50, 0, 0),
  LabelSizeInfo("Avery 5264", "Letter", 2, 3, 400, 333, 25, 75, 0, 0),
  LabelSizeInfo("Avery 8460", "Letter", 3, 10, 262, 100, 32, 50, 0, 0),
  LabelSizeInfo("CILS ALP1-9200-1", "Letter", 3, 7, 200, 100, 62, 62, 81, 50),
  LabelSizeInfo()               // Null Label Size
};

const LabelSizeInfo & LabelSizeInfo::getByName(const QString & name)
{
  int i = 0;
  while (!__labels[i].isNull() && __labels[i]._name != name)
    i++;
  return __labels[i];
}

QStringList LabelSizeInfo::getLabelNames()
{
  QStringList l;
  for (int i = 0; !__labels[i].isNull(); i++)
    l.append(__labels[i]._name);
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
  _name = QString::null;
  _paper = QString::null;

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

const QString & LabelSizeInfo::name()
{
  return _name;
}

const QString & LabelSizeInfo::paper()
{
  return _paper;
}

const int LabelSizeInfo::columns()
{
  return _columns;
}
const int LabelSizeInfo::rows()
{
  return _rows;
}

const int LabelSizeInfo::width()
{
  return _width;
}
const int LabelSizeInfo::height()
{
  return _height;
}

const int LabelSizeInfo::startX()
{
  return _startx;
}
const int LabelSizeInfo::startY()
{
  return _starty;
}

const int LabelSizeInfo::xGap()
{
  return _xgap;
}
const int LabelSizeInfo::yGap()
{
  return _ygap;
}

bool LabelSizeInfo::isNull()
{
  return _null;
}
