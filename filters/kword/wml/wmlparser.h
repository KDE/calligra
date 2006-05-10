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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef __WMLPARSER_H
#define __WMLPARSER_H

#include <q3valuelist.h>
#include <QString>

class WMLFormat
{
  public:
    int pos, len;
    bool bold, italic, underline;
    enum { Normal, Big, Small } fontsize;
    QString link, href;
    WMLFormat();
    WMLFormat( const WMLFormat& );
    WMLFormat& operator= ( const WMLFormat& );
    void assign( const WMLFormat& );
};

class WMLLayout
{
  public:
    enum { Left, Center, Right } align;
    WMLLayout();
    WMLLayout( const WMLLayout& );
    WMLLayout& operator=( const WMLLayout& );
    void assign( const WMLLayout& );
};

typedef Q3ValueList<WMLFormat> WMLFormatList;

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
    virtual bool doParagraph( QString, WMLFormatList, WMLLayout );
    virtual bool doBeginTable();
    virtual bool doTableCell( unsigned row, unsigned col );
    virtual bool doEndTable();
};

#endif
