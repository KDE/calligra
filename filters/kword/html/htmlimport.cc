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

#include <htmlimport.h>
#include <htmlimport.moc>
#include <kdebug.h>

HTMLImport::HTMLImport(KoFilter *parent, const char*name) :
                     KoFilter(parent, name) {
}

bool HTMLImport::filter(const QString &fileIn, const QString &fileOut,
                        const QString& from, const QString& to,
                        const QString &) {

    if(to!="application/x-kword" || from!="text/html")
        return false;

    QFile in(fileIn);
    if(!in.open(IO_ReadOnly)) {
        kdError(30503) << "Unable to open input file!" << endl;
        in.close();
        return false;
    }

    QString str;

    str += "<?xml version=\"1.0\"?>\n";
    str += "<DOC  author=\"Reginald Stadlbauer and Torben Weis\" email=\"reggie@kde.org and weis@kde.org\" editor=\"KWord\" mime=\"application/x-kword\">\n";
    str += "<PAPER format=\"1\" ptWidth=\"595\" ptHeight=\"841\" mmWidth =\"210\" mmHeight=\"297\" inchWidth =\"8.26772\" inchHeight=\"11.6929\" orientation=\"0\" columns=\"1\" ptColumnspc=\"2\" mmColumnspc=\"1\" inchColumnspc=\"0.0393701\" hType=\"0\" fType=\"0\" ptHeadBody=\"9\" ptFootBody=\"9\" mmHeadBody=\"3.5\" mmFootBody=\"3.5\" inchHeadBody=\"0.137795\" inchFootBody=\"0.137795\">\n";
    str += "<PAPERBORDERS mmLeft=\"10\" mmTop=\"15\" mmRight=\"10\" mmBottom=\"15\" ptLeft=\"28\" ptTop=\"42\" ptRight=\"28\" ptBottom=\"42\" inchLeft=\"0.393701\" inchTop=\"0.590551\" inchRight=\"0.393701\" inchBottom=\"0.590551\"/>\n";
    str += "</PAPER>\n";
    str += "<ATTRIBUTES processing=\"0\" standardpage=\"1\" hasHeader=\"0\" hasFooter=\"0\" unit=\"mm\"/>\n";
    str += "<FRAMESETS>\n";
    str += "<FRAMESET frameType=\"1\" autoCreateNewFrame=\"1\" frameInfo=\"0\" removeable=\"0\">\n";
    str += "<FRAME left=\"28\" top=\"42\" right=\"566\" bottom=\"798\" runaround=\"1\" runaGapPT=\"2\" runaGapMM=\"1\" runaGapINCH=\"0.0393701\"  lWidth=\"1\" lRed=\"255\" lGreen=\"255\" lBlue=\"255\" lStyle=\"0\"  rWidth=\"1\" rRed=\"255\" rGreen=\"255\" rBlue=\"255\" rStyle=\"0\"  tWidth=\"1\" tRed=\"255\" tGreen=\"255\" tBlue=\"255\" tStyle=\"0\"  bWidth=\"1\" bRed=\"255\" bGreen=\"255\" bBlue=\"255\" bStyle=\"0\" bkRed=\"255\" bkGreen=\"255\" bkBlue=\"255\" bleftpt=\"0\" bleftmm=\"0\" bleftinch=\"0\" brightpt=\"0\" brightmm=\"0\" brightinch=\"0\" btoppt=\"0\" btopmm=\"0\" btopinch=\"0\" bbottompt=\"0\" bbottommm=\"0\" bbottominch=\"0\"/>\n";

    str += "<PARAGRAPH>\n";
    str += "<TEXT>";

    int buflen=in.size();
    char * buffer = new char[buflen];
    in.readBlock(buffer, buflen);
    bool add = true;
    int step=buflen/50;
    int value=0;
    int j=0;

    for ( int i = 0 ;i < buflen ; ++i )
    {
        ++j;
        if(j>step) {
            j=0;
            value+=2;
            emit sigProgress(value);
        }
        QChar c = buffer[ i ];
        if ( c == QChar( '\n' ) )
        {
            str += "</TEXT>\n";
            str += "</PARAGRAPH>\n";
            str += "<PARAGRAPH>\n";
            str += "<TEXT>";
        }
        else if ( c == QChar( '<' ) )
            add = false;
        else if ( c == QChar( '>' ) )
            add = true;
        else if ( add )
            str += c;
    }

    str += "</TEXT>\n";
    str += "</PARAGRAPH>\n";

    str += "</FRAMESET>\n";
    str += "</FRAMESETS>\n";
    str += "</DOC>\n";

    QCString cstr=QCString(str.utf8());

    KoStore out=KoStore(QString(fileOut), KoStore::Write);
    if(!out.open("root")) {
        kdError(30503) << "Unable to open output file!" << endl;
        in.close();
        out.close();
        delete [] buffer;
        return false;
    }
    out.write((const char*)cstr, cstr.length());
    out.close();
    in.close();
    delete [] buffer;
    return true;
}
