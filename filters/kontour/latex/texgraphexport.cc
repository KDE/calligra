/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>

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

#include <texgraphexport.h>
#include <texgraphexport.moc>
#include <kdebug.h>
#include <kglobal.h>
#include <klocale.h>
//#include <qtextcodec.h>

TEXGRAPHExport::TEXGRAPHExport(KoFilter *parent, const char *name) :
                     KoFilter(parent, name) {
}

bool TEXGRAPHExport::filter(const QString &fileIn, const QString &fileOut,
                         const QString& from, const QString& to,
                         const QString &) {

    if(to != "text/x-latex" || from != "application/x-killustrator")
        return false;

    KoStore in = KoStore(QString(fileIn), KoStore::Read);
    if(!in.open("root")) {
        kdError(30503) << "Unable to open input file!" << endl;
        in.close();
        return false;
    }
    // input file Reading
    QByteArray array=in.read(0xffffffff);
    QString buf = QString::fromUtf8((const char*)array, array.size());
    in.close();

    QString fileIn2 = QString("/tmp/tmp-latexexport.xml");

    QFile tempIn(fileIn2);
    if(tempIn.open(IO_WriteOnly))
    {
        QTextStream tempStream(&tempIn);
	tempStream.setEncoding(QTextStream::Unicode);
	tempStream << buf;
    }
    tempIn.close();

    kdDebug() << "TEXGRAPH FILTER --> BEGIN" << endl;
    Document TEXGRAPHParser(fileIn2, fileOut);
    TEXGRAPHParser.analyse();
    kdDebug() << "---------- generate file -------------" << endl;
    TEXGRAPHParser.generate();
    kdDebug() << "TEXGRAPH FILTER --> END" << endl;

    return true;
}
