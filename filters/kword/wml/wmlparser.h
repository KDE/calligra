/* This file is part of the KDE project
   Copyright (C) 2002 Ariya Hidayat <ariyahidayat@yahoo.de>

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

#ifndef __WMLPARSER_H
#define __WMLPARSER_H

#include <qvaluelist.h>
#include <qstring.h>

class WMLFormat
{
  public:
    int pos, len;
    bool bold, italic, underline;
    WMLFormat();
    WMLFormat( const WMLFormat& );
    WMLFormat& operator= ( const WMLFormat& );
    void assign( const WMLFormat& );
};

typedef QValueList<WMLFormat> WMLFormatList;

class WMLParser
{
  public:
    WMLParser(){};
    virtual ~WMLParser(){};
    virtual void parse( const char* filename );

    virtual bool doOpenDocument();
    virtual bool doCloseDocument();
    virtual bool doOpenCard( QString id, QString title );
    virtual bool doCloseCard();
    virtual bool doParagraph( QString text, WMLFormatList list );
};

#endif
