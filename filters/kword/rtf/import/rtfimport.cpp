/* This file is part of the KDE project
   Copyright (C) 1999 David Faure <faure@kde.org>

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

#include <rtfimport.h>

#include <qfile.h>
#include <kdebug.h>
#include <koStore.h>

#include <KRTFTokenizer.h>
#include <KRTFFileParser.h>



RTFImport::RTFImport(KoFilter *parent, const char *name) : KoFilter(parent, name) {
}

bool RTFImport::filter(const QString &fileIn, const QString &fileOut,
                       const QString &from, const QString &to, const QString &/*config*/) {

    if(to!="application/x-kword" || from!="text/rtf")
        return false;

    QFile in(fileIn);
    if(!in.open(IO_ReadOnly)) {
        kdError() << "Unable to open input file!" << endl;
        in.close();
        return false;
    }

    KoStore out=KoStore(QString(fileOut), KoStore::Write);
    if(!out.open("root")) {
        kdError() << "Unable to open output file!" << endl;
        in.close();
        out.close();
        return false;
    }

    KRTFTokenizer tokenizer( &in );
    KRTFFileParser parser( &tokenizer, &out );

    bool okay=parser.parse();
    if(!okay)
        kdWarning() << "Error in RTF file" << endl;

    in.close();
    out.close();
    return okay;
}

#include <rtfimport.moc>
