/* This file is part of the KDE project
   Copyright (C) 2000 Thomas Zander zander@earthling.net

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

#include <config.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <qtextstream.h>
#include <asciiimport.h>
#include <asciiimport.moc>
#include <kdebug.h>

ASCIIImport::ASCIIImport(KoFilter *parent, const char *name) :
                     KoFilter(parent, name) {
}

const bool ASCIIImport::filter(const QString &fileIn, const QString &fileOut,
                               const QString& from, const QString& to,
                               const QString &) {
kdDebug(30502) << "checkpoint" << endl;

    if(to!="application/x-kspread" || from!="text/plain")
        return false;

    QFile in(fileIn);
    if(!in.open(IO_ReadOnly)) {
        kdError(30502) << "Unable to open input file!" << endl;
        in.close();
        return false;
    }

    kdDebug(30502) << "doing header" << endl;

    QString str;

    str += "<?xml version=\"1.0\" encoding=\"UTF-8\"?><!DOCTYPE spreadsheet ><spreadsheet mime=\"application/x-kspread\" editor=\"KSpread\" >\n";
    str += " <paper format=\"A4\" orientation=\"Portrait\" >\n";
    str += "  <borders right=\"20\" left=\"20\" bottom=\"20\" top=\"20\" />\n";
    str += "  <head/>\n";
    str += "  <foot/>\n";
    str += " </paper>\n";
    str += " <map markerColumn=\"1\" activeTable=\"Table1\" markerRow=\"1\" >\n";
    str += "  <table columnnumber=\"0\" borders=\"0\" hide=\"0\" hidezero=\"0\" firstletterupper=\"0\" grid=\"1\" formular=\"0\" lcmode=\"0\" name=\"Table1\" >\n";

    QTextStream stream(&in);
    int value=0, i=0, step=in.size()/50;
    int row=1;
    int col=1;
    bool newcell=true;
    bool firstcell=true;
    while(!stream.atEnd())
    {
        QChar c;
        stream >> c;
        if ( c == QChar( '\n' ) ) {
            row++;
            col=1;
            newcell=true;
        }
        else if ( c == QChar( '\t' ) ) {
            col++;
            newcell=true;
        } else if ( c == QChar( ';' ) ) {
            col++;
            newcell=true;
        } 

        if(newcell) {
kdDebug(30502) << "newcell" << endl;
            if(firstcell) {
kdDebug(30502) << "firstcell" << endl;
                str+= "</text>\n";
                str+= "   </cell>\n";
                firstcell=false;
            }
kdDebug(30502) << "row: "  << row << ", col: "<< col << endl;
            str+= "   <cell row=\" + row + \" column=\" + col + \" >\n";
            str+= "    <text>";
            newcell=false;
        }

        if ( c == QChar( '<' ) )
            str += "&lt;";
        else if ( c == QChar( '>' ) )
            str += "&gt;";
        else if( c == QChar( '&' ) )
            str+="&amp;";
        else
            str+=c;
            
        if(i>step) {
kdDebug(30502) << "now at "<< value << "%" << endl;
            i=0;
            value+=2;
            emit sigProgress(value);
        }
    }
    emit sigProgress(100);

    str += "  </table>\n";
    str += " </map>\n";
    str += "</spreadsheet>";

    kdDebug(30502) << str << endl;

    KoStore out=KoStore(QString(fileOut), KoStore::Write);
    if(!out.open("root")) {
        kdError(30502) << "Unable to open output file!" << endl;
        in.close();
        out.close();
        return false;
    }
    QCString cstring=str.utf8();
    out.write((const char*)cstring, cstring.length());
    out.close();
    in.close();
    return true;
}
