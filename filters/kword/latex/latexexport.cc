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

#include <latexexport.h>
#include <latexexport.moc>
#include <kdebug.h>
#include <kglobal.h>
#include <klocale.h>
#include <qtextcodec.h>
#include "latexexportdia.h"
//#include "xml2latexparser.h"

LATEXExport::LATEXExport(KoFilter *parent, const char *name) :
                     KoFilter(parent, name) {
}

bool LATEXExport::filter(const QString &fileIn, const QString &fileOut,
                         const QString& from, const QString& to,
                         const QString &) {
    QString config;

    if(to != "text/x-tex" || from != "application/x-kword")
        return false;

    KoStore in = KoStore(QString(fileIn), KoStore::Read);
    if(!in.open("root")) {
        kdError(30503) << "Unable to open input file!" << endl;
        in.close();
        return false;
    }
    /* input file Reading */
    QByteArray array=in.read(0xffffffff);
    QString buf = QString::fromUtf8((const char*)array, array.size());
    in.close();

    QString fileIn2 = QString("/tmp/tmp-latexexport.xml");

    QFile tempIn(fileIn2);
    if(tempIn.open(IO_WriteOnly))
    {
        QTextStream tempStream(&tempIn);
	tempStream.setEncoding(QTextStream::UnicodeUTF8);
	tempStream << buf;
    }
    tempIn.close();
    
    LATEXExportDia* dialog = new LATEXExportDia();
    dialog->setInputFile(fileIn2);
    dialog->setOutputFile(fileOut);

    dialog->exec();
    //config = dialog->state();
    delete dialog;
    /*kdDebug() << "config : " << config << endl;
    kdDebug() << "LATEX FILTER --> BEGIN" << endl;
    Xml2LatexParser LATEXParser(fileIn2, fileOut, config);
    LATEXParser.analyse();
    kdDebug() << "---------- generate file -------------" << endl;
    LATEXParser.generate();
    kdDebug() << "LATEX FILTER --> END" << endl;*/

    return true;
}
