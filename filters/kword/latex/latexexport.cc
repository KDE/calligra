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

LATEXExport::LATEXExport(KoFilter *parent, const char *name) :
                     KoFilter(parent, name) {
}

const bool LATEXExport::filter(const QString &fileIn, const QString &fileOut,
                              const QString& from, const QString& to,
                              const QString &) {

    if(to != "text/x-tex" || from != "application/x-kword")
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
 
    int begin = buf.find( "<DOC" ); // skip <?...?>
    buf.remove(0, begin);
    kdDebug() << "LATEX FILTER --> BEGIN" << endl;
    Xml2LatexParser LATEXParser(fileOut, buf.latin1());
    LATEXParser.analyse();
    kdDebug() << "---------- generate file -------------" << endl;
    LATEXParser.generate();
    kdDebug() << "LATEX FILTER --> END" << endl;

    // It would certainly be more efficient if the filter was writing
    // into a QString (using fromUtf8 for the stuff coming from the xml file)
    // instead of using a temporary file !
    /*QFile f( "/tmp/kword2latex" );
    if ( !f.open( IO_ReadOnly ) ) {
        return false;
    }

    QTextStream s( &f );
    s.setEncoding( QTextStream::UnicodeUTF8 ); // The contents comes from XML, it will always be utf 8
    // Yes, your eyes are working well: we put everything we just wrote
    // into a file back into a QString !
    QString str = s.read();
    f.close();

    QFile out(fileOut);
    if(!out.open(IO_WriteOnly)) {
        kdError(30503) << "Unable to open output file!" << endl;
        out.close();
        return false;
    }
    QTextStream sout( &out );
    QTextCodec * codec = QTextCodec::codecForName( charset );
    if ( !codec )
    {
        kdError(30503) << "Unable to find codec for " << charset << " !" << endl;
        out.close();
        return false;
    }
    sout.setCodec( codec );
    sout << str;

    out.close();*/
    return true;
}
