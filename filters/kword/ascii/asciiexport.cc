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

#include <asciiexport.h>
#include <asciiexport.moc>
#include <kdebug.h>

ASCIIExport::ASCIIExport(KoFilter *parent, QString name) :
                     KoFilter(parent, name) {
}

const bool ASCIIExport::filter(const QCString &fileIn, const QCString &fileOut,
                               const QCString& from, const QCString& to,
                               const QString &) {

    if(to!="text/plain" || from!="application/x-kword")
        return false;

    KoTarStore in=KoTarStore(QString(fileIn), KoStore::Read);
    if(!in.open("root", "")) {
        kDebugError( 31502, "Unable to open input file!");
        in.close();
        return false;
    }
    // read the whole file - at least I hope it does :)
    QByteArray array=in.read(0xffffffff);
    QCString buf((const char*)array, array.size());

    QString str;

    int i = buf.find( "<TEXT>" );
    while ( i != -1 )
    {
        int j = buf.find( "</TEXT>", i );
        if ( j - ( i + 6 ) > 0 )
        {
            str += buf.mid( i + 6, j - ( i + 6 ) );
            str += "\n";
        }
        i = buf.find( "<TEXT>", j );
    }

    QCString cstr=QCString(str.utf8());

    QFile out(fileOut);
    if(!out.open(IO_WriteOnly)) {
        kDebugError( 31502, "Unable to open output file!");
        in.close();
        out.close();
        return false;
    }
    out.writeBlock((const char*)cstr, cstr.length());

    in.close();
    out.close();
    return true;
}
