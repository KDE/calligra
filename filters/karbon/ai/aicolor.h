/***************************************************************************
                          aicolor.h  -  description
                             -------------------
    begin                : Thu Feb 21 2002
    copyright            : (C) 2002 by 
    email                : 
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef AICOLOR_H
#define AICOLOR_H

#include <qstring.h>

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
