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

#include <htmlexport.h>
#include <htmlexport.moc>
#include <kdebug.h>
#include <kglobal.h>
#include <klocale.h>

void mainFunc(const char*,const char*);

HTMLExport::HTMLExport(KoFilter *parent, const char *name) :
                     KoFilter(parent, name) {
}

const bool HTMLExport::filter(const QString &fileIn, const QString &fileOut,
                              const QString& from, const QString& to,
                              const QString &) {

    if(to!="text/html" || from!="application/x-kword")
        return false;

    KoStore in=KoStore(QString(fileIn), KoStore::Read);
    if(!in.open("root")) {
        kdError(30503) << "Unable to open input file!" << endl;
        in.close();
        return false;
    }
    // read the whole file
    QCString buf( in.size() );
    int count = in.read( buf.data(), in.size() );
    if ( count != in.size() ) {
      kdError(30503) << "Error reading input file!" << endl;
      in.close();
      return false;
    }

    //kdDebug(30503) << buf << endl;
    // Note: I have no idea if I should use KLocale::charset or KGlocal::charset....
    //QCString charset = KGlobal::locale()->charset().ascii();
    // Problem: the charset of the document may not be the one of the user
    // (for instance, someone sent me a non-latin 1 doc :-)
    QCString charset( "utf-8" );

    int begin = buf.find( "<DOC" ); // skip <?...?>
    mainFunc( (const char*)buf + begin, charset );

    QFile f( "/tmp/kword2html" );
    if ( !f.open( IO_ReadOnly ) ) {
        in.close();
        return false;
    }

    QTextStream s( &f );
    QString str = s.read();
    f.close();

    QCString cstr(str.utf8());

    QFile out(fileOut);
    if(!out.open(IO_WriteOnly)) {
        kdError(30503) << "Unable to open output file!" << endl;
        in.close();
        out.close();
        return false;
    }
    out.writeBlock((const char*)cstr, cstr.length());

    in.close();
    out.close();
    return true;
}
