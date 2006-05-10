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

#ifndef AICOLOR_H
#define AICOLOR_H

#include <QString>

/**
  *@author 
  */

class AIColor {
  public:
     typedef enum { CT_CMYK, CT_CMYK_Key, CT_Gray } ColorType;

  private:
    ColorType ctype;

    union {
      struct {
        double cvalue, mvalue, yvalue, kvalue;
        char *colorname;
        double graydata;
      } cmykdata;
      double graydata;
    } cdata;
public:
	AIColor();
	~AIColor();
  AIColor( const AIColor& );
  AIColor( double c, double m, double y, double k );
  AIColor( double c, double m, double y, double k, const char *colorname, double gray );
  AIColor( double gray );

  void toRGB (double &r, double &g, double &b);
  void toCMYK (double &c, double &m, double &y, double &k);

};

#endif
