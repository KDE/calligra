/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>
 
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

#ifndef __ko_stream_h__
#define __ko_stream_h__

#include <unistd.h>
#include <config.h>
#include <iostream.h>
#ifdef HAVE_MINI_STL
#include <ministl/vector.h>
#else
#include <vector.h>
#endif

#include <komlParser.h>
#include <komlWriter.h>

#include <qrect.h>
#include <qcolor.h>
#include <qpen.h>
#include <qfont.h>

class QImage;

#include <iostream.h>

ostream& operator<< ( ostream& outs, const QRect &_rect );
QRect tagToRect( vector<KOMLAttrib>& _attribs );

ostream& operator<< ( ostream& outs, const QColor &_rect );
istream& operator>> ( istream& outs, QColor &_rect );
QColor strToColor( const char *_buffer );

ostream& operator<< ( ostream& outs, const QPen &_pen );
QPen tagToPen( vector<KOMLAttrib>& _attribs );

ostream& operator<< ( ostream& outs, const QFont &_font );
QFont tagToFont( vector<KOMLAttrib>& _attribs );

ostream& operator<< ( ostream& outs, const QImage &_img );
istream& operator>> ( istream& ins, QImage &_img );

#endif



