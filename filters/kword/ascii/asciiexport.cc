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

    KoStore in=KoStore(QString(fileIn), KoStore::Read);
    if(!in.open("root")) {
        kdError(30502) << "Unable to open input file!" << endl;
        in.close();
        return false;
    }
    // read the whole file - at least I hope it does :)
    QByteArray array=in.read(0xffffffff);
    QString buf=QString::fromUtf8((const char*)array, array.size());

    int step=buf.length()/50;
    int value=0;
    int k=0;
    emit sigProgress(0);

    QString str;
    QRegExp amp("&amp;");
    QRegExp lt("&lt;");
    QRegExp gt("&gt;");

    int i = buf.find( "<TEXT>" );
    while ( i != -1 )
    {
	k+=i;
        int j = buf.find( "</TEXT>", i );
        if ( j - ( i + 6 ) > 0 )
        {
            str += buf.mid( i + 6, j - ( i + 6 ) ).replace( amp, "&" )
                                                  .replace( lt, "<" )
                                                  .replace( gt, ">" )
                                                  ;
            str += "\n";
        }
        i = buf.find( "<TEXT>", j );
	if(k>step) {
	    k=0;
	    value+=2;
	    emit sigProgress(value);
	}
    }

    QFile out(fileOut);
    if(!out.open(IO_WriteOnly)) {
        kdError(30502) << "Unable to open output file!" << endl;
        in.close();
        out.close();
        return false;
    }
    out.writeBlock((const char*)str.local8Bit(), str.length());

    in.close();
    out.close();
    return true;
}
