/* This file is part of the KDE project
   Copyright (C) 2001 Andrea Rizzi <rizzi@kde.org>
	              Ulrich Kuettler <ulrich.kuettler@mailbox.tu-dresden.de>

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
   Boston, MA 02111-1307, USA.
*/

//KDE Include
#include <kconfig.h>

//Qt Include

//Formula include

enum DrawMode {CharSize,FixedHeight,FixedWidth}

/**
 * Contains info on fonts etc..
 */

class ContextStyle
{
public:  

/**
 * Build a default context style
 */
  ContextStyle();

/**
 * Build a context style reading settings from config
 */
  ContextStyle(KConfig *config);

/**
 * return defalt color
 */

QColor getDefaultColor();


private:
 QFont defaultFont;H
 QColor defaultColor;

 
 DrawMode mode;
 bool editing;
 
};
