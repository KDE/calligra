/* This file is part of the KDE project
   Copyright (C) 2002, Dirk Schönberger <dirk.schoenberger@sz-online.de>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "aicolor.h"
#include <stdlib.h>

AIColor::AIColor(){
  ctype = AIColor::CT_CMYK;
  cdata.cmykdata.cvalue = 0;
  cdata.cmykdata.mvalue = 0;
  cdata.cmykdata.yvalue = 0;
  cdata.cmykdata.kvalue = 0;
}

AIColor::AIColor( const AIColor& value ){
  ctype = value.ctype;
  memcpy (&cdata, &value.cdata, sizeof (cdata));
}
AIColor::AIColor( double c, double m, double y, double k ){
  ctype = AIColor::CT_CMYK;
  cdata.cmykdata.cvalue = c;
  cdata.cmykdata.mvalue = m;
  cdata.cmykdata.yvalue = y;
  cdata.cmykdata.kvalue = k;
}
AIColor::AIColor( double c, double m, double y, double k, const char* colorname, double gray ){
  ctype = AIColor::CT_CMYK_Key;
  cdata.cmykdata.cvalue = c;
  cdata.cmykdata.mvalue = m;
  cdata.cmykdata.yvalue = y;
  cdata.cmykdata.kvalue = k;
  cdata.cmykdata.colorname = strdup (colorname);
}
AIColor::AIColor( double gray ){
  ctype = AIColor::CT_Gray;
  cdata.graydata = gray;
}

AIColor::~AIColor(){
}

void AIColor::toRGB (double &r, double &g, double &b)
{
  switch (ctype)
  {
    case CT_CMYK :
    case CT_CMYK_Key :
      r = 1 - cdata.cmykdata.cvalue - cdata.cmykdata.kvalue;
      g = 1 - cdata.cmykdata.mvalue - cdata.cmykdata.kvalue;
      b = 1 - cdata.cmykdata.yvalue - cdata.cmykdata.kvalue;
      break;
    case CT_Gray :
      r = cdata.graydata;
      g = cdata.graydata;
      b = cdata.graydata;
      break;
    default :
      qDebug ("unknown colortype %d", ctype);
  }
}

void AIColor::toCMYK (double &c, double &m, double &y, double &k)
{
  switch (ctype)
  {
    case CT_CMYK :
    case CT_CMYK_Key :
      c = cdata.cmykdata.cvalue;
      m = cdata.cmykdata.mvalue;
      y = cdata.cmykdata.yvalue;
      k = cdata.cmykdata.kvalue;
      break;
    case CT_Gray :
      c = 0;
      m = 0;
      y = 0;
      k = cdata.graydata;
      break;
    default :
      qDebug ("unknown colortype %d", ctype);
  }
}


