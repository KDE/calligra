/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>
   Copyright (C) 2001 Nicolas GOUTTE <nicog@snafu.de>

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

#include <qtextstream.h>
#include <qxml.h>

#include <htmlimport.h>
#include <htmlimport.moc>
#include <kdebug.h>

#include "htmlimportwell.h"
#include "htmlimportsax.h"

HTMLImport::HTMLImport(KoFilter *parent, const char*name) :
                     KoFilter(parent, name) {
}

static bool checkXMLDeclaration(const QString &fileIn)
{
    QFile in(fileIn);
    if(!in.open(IO_ReadOnly)) {
        kdError(30503) << "Unable to open input file!" << endl;
        return false;
    }

    QTextStream inStream(&in);
    inStream.setEncoding( QTextStream::UnicodeUTF8 );

    inStream.skipWhiteSpace(); // skip white space that may be before the XML declaration

    QString strToCompare="<?xml";
    QString strRead;
    QChar ch;

    for (int i=0; i<5; i++)
    {
        inStream >> ch; // read one char
        strRead+=ch;
    }

    return (strRead==strToCompare);
}

static bool TwoPassFilter(const QString &fileIn, const QString &fileOut)
{
    kdDebug(30503) << "Calling first pass (HTML to XHTML)" << endl;
    QFile in(fileIn);
    if(!in.open(IO_ReadOnly)) {
        kdError(30503) << "Unable to open input file!" << endl;
        return false;
    }
    QTextStream inStream(&in);
    QString strXHTML;
    QTextStream streamXHTML(&strXHTML,IO_WriteOnly);
    if (!WellFilter(inStream,streamXHTML))
    {
        kdDebug(30503) << "First pass failed! Aborting!" << endl;
        return false;
    }
#if 1
    kdDebug(30503) << strXHTML;
#endif
    kdDebug(30503) << "Calling second pass (HTML to XHTML)" << endl;
    QXmlInputSource source;
    source.setData(strXHTML);
    return saxfilter(source,fileOut);
}


bool HTMLImport::filter(const QString &fileIn, const QString &fileOut,
                        const QString& from, const QString& to,
                        const QString &)
{
    if(to!="application/x-kword" || from!="text/html")
        return false;

    bool result=false;
    if (checkXMLDeclaration(fileIn)) // Do we have a XML file?
    { // We have a XML file, so we can use the new XHTML filter!
        kdDebug(30503) << "Calling XHTML filter" << endl;
        result=saxfilter(fileIn,fileOut);
    }
    else
    { // We have not a XML file, so we must make two passes
        kdDebug(30503) << "Calling two pass filter" << endl;
        result=TwoPassFilter(fileIn,fileOut);
    }
    return result;
}
