/* Sidewinder - Portable library for spreadsheet
   Copyright (C) 2003 Ariya Hidayat <ariya@kde.org>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, US
*/

#ifndef SIDEWINDER_DEFS_H
#define SIDEWINDER_DEFS_H

namespace Sidewinder
{

/**
 * @short Provides color based on RGB values.
 *
 * Class Color provides color based on  terms of RGB (red, green and blue) 
 * components.
 * 
 */
class Color
{
public:

  unsigned red, green, blue;
  
  /**
   * Constructs a default color with the RGB value (0, 0, 0), i.e black.
   */   
  Color(){ red = green = blue = 0; };
  
  /**
   * Creates a copy of another color.
   */   
  Color( const Color& c )
    { red = c.red; green = c.green; blue = c.blue; }   
  
  /**
   * Creates a color based on given red, green and blue values.
   */   
  Color( unsigned r, unsigned g, unsigned b )
    { red = r; green = g; blue = b; }
};


}; // namespace Sidewinder

#endif // SIDEWINDER_DEFS_H

