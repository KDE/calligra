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

void mainFunc(const char*);

HTMLExport::HTMLExport(KoFilter *parent, QString name) :
                     KoFilter(parent, name) {
}

const bool HTMLExport::filter(const QCString &fileIn, const QCString &fileOut,
                               const QCString& from, const QCString& to) {

    if(to!="text/html" || from!="application/x-kword")
        return false;

    KoTarStore in=KoTarStore(QString(fileIn), KoStore::Read);
    if(!in.open("root", "")) {
        kdebug(KDEBUG_ERROR, 31000, "Unable to open input file!");
        in.close();
        return false;
    }
    // read the whole file - at least I hope it does :)
    QByteArray array=in.read(0xffffffff);
    QCString buf((const char*)array, array.size());

    int begin = buf.find( "<DOC" );
    buf.remove( 0, begin - 1 );

    mainFunc( (const char*)buf );

    QFile f( "/tmp/kword2html" );
    if ( !f.open( IO_ReadOnly ) ) {
        in.close();
        return false;
    }

    QTextStream s( &f );
    QString str = s.read();
    f.close();

    QCString cstr=QCString(str.utf8());

    QFile out(fileOut);
    if(!out.open(IO_WriteOnly)) {
        kdebug(KDEBUG_ERROR, 31000, "Unable to open output file!");
        in.close();
        out.close();
        return false;
    }
    out.writeBlock((const char*)cstr, cstr.length());

    in.close();
    out.close();
    return true;
}
