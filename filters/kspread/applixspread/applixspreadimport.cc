/* This file is part of the KDE project
   Copyright (C) 2001 David Faure <david@mandrakesoft.com>

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
#include <applixspreadimport.h>
#include <applixspreadimport.moc>
#include <kdebug.h>

APPLIXSPREADImport::APPLIXSPREADImport(KoFilter *parent, const char *name) :
                     KoFilter(parent, name) 
{
}

const bool APPLIXSPREADImport::filter(const QString &fileIn, const QString &fileOut,
                               const QString& from, const QString& to,
                               const QString &) 
{

    if(to!="application/x-kspread" || from!="application/x-applixspread")
        return false;

    QFile in(fileIn);
    if(!in.open(IO_ReadOnly)) {
        kdError(30502) << "Unable to open input file!" << endl;
        in.close();
        return false;
    }

    QString str;

    str += "<?xml version=\"1.0\"?>\n";
    str += "<!DOCTYPE doc >\n";
    str += "<doc editor=\"KSpread\" mime=\"application/x-kspread\">\n";
    str += "<paper format=\"A4\" orientation=\"Portrait\">\n";
    // <borders>, <head> and <foot> skipped
    str += "</paper>\n";
    str += "<map>\n";

    str += "<table name=\"test\">\n";
   
    QTextStream stream(&in);
    QString s = stream.readLine(); // *BEGIN SPREADSHEETS VERSION=.../... ENCODING=7BIT
    if ( !s.startsWith("*BEGIN SPREADSHEETS") )
    {
      kdError(30502) << "Not a valid ApplixSpreads file" << endl;
      in.close();
      return false;
    }
    s += stream.readLine(); // Num ExtLinks: 0
    s += stream.readLine(); // Spreadsheet Dump Rev 4.30 Line Length 80
    int step = (in.size() - s.length())/50;
    int progress = 0;
    int i = 0;
    while (!stream.atEnd())
    {
        QString s = stream.readLine();
        i += s.length();

#if 0 // the ApplixSpread encoding for special chars is not the same as the ApplixWord one :(
          if      ( ( c1 == QChar ('n') )  &&  ( c2 == QChar ('p') ) ) str += "ß"; 
          else if ( ( c1 == QChar ('n') )  &&  ( c2 == QChar ('c') ) ) str += "Ò"; 
          else if ( ( c1 == QChar ('p') )  &&  ( c2 == QChar ('c') ) ) str += "ò"; 
          else if ( ( c1 == QChar ('n') )  &&  ( c2 == QChar ('d') ) ) str += "Ó"; 
          else if ( ( c1 == QChar ('p') )  &&  ( c2 == QChar ('d') ) ) str += "ó"; 
          else if ( ( c1 == QChar ('n') )  &&  ( c2 == QChar ('e') ) ) str += "Ô"; 
          else if ( ( c1 == QChar ('p') )  &&  ( c2 == QChar ('e') ) ) str += "ô"; 
          else if ( ( c1 == QChar ('n') )  &&  ( c2 == QChar ('f') ) ) str += "Õ"; 
          else if ( ( c1 == QChar ('p') )  &&  ( c2 == QChar ('f') ) ) str += "õ"; 
          else if ( ( c1 == QChar ('p') )  &&  ( c2 == QChar ('g') ) ) str += "ö"; 
          else if ( ( c1 == QChar ('n') )  &&  ( c2 == QChar ('g') ) ) str += "Ö"; 
          else if ( ( c1 == QChar ('n') )  &&  ( c2 == QChar ('j') ) ) str += "Ù"; 
          else if ( ( c1 == QChar ('p') )  &&  ( c2 == QChar ('j') ) ) str += "ù"; 
          else if ( ( c1 == QChar ('n') )  &&  ( c2 == QChar ('k') ) ) str += "Ú"; 
          else if ( ( c1 == QChar ('p') )  &&  ( c2 == QChar ('k') ) ) str += "ú"; 
          else if ( ( c1 == QChar ('n') )  &&  ( c2 == QChar ('l') ) ) str += "Û"; 
          else if ( ( c1 == QChar ('p') )  &&  ( c2 == QChar ('l') ) ) str += "û"; 
          else if ( ( c1 == QChar ('p') )  &&  ( c2 == QChar ('m') ) ) str += "ü"; 
          else if ( ( c1 == QChar ('n') )  &&  ( c2 == QChar ('m') ) ) str += "Ü";
          else if ( ( c1 == QChar ('m') )  &&  ( c2 == QChar ('a') ) ) str += "À"; 
          else if ( ( c1 == QChar ('o') )  &&  ( c2 == QChar ('a') ) ) str += "à"; 
          else if ( ( c1 == QChar ('m') )  &&  ( c2 == QChar ('b') ) ) str += "Á"; 
          else if ( ( c1 == QChar ('o') )  &&  ( c2 == QChar ('b') ) ) str += "á"; 
          else if ( ( c1 == QChar ('m') )  &&  ( c2 == QChar ('c') ) ) str += "Â"; 
          else if ( ( c1 == QChar ('o') )  &&  ( c2 == QChar ('c') ) ) str += "â"; 
          else if ( ( c1 == QChar ('m') )  &&  ( c2 == QChar ('d') ) ) str += "Ã"; 
          else if ( ( c1 == QChar ('o') )  &&  ( c2 == QChar ('d') ) ) str += "ã"; 
          else if ( ( c1 == QChar ('m') )  &&  ( c2 == QChar ('e') ) ) str += "Ä"; 
          else if ( ( c1 == QChar ('o') )  &&  ( c2 == QChar ('e') ) ) str += "ä"; 
          else if ( ( c1 == QChar ('m') )  &&  ( c2 == QChar ('f') ) ) str += "Å"; 
          else if ( ( c1 == QChar ('o') )  &&  ( c2 == QChar ('f') ) ) str += "å"; 
          else if ( ( c1 == QChar ('m') )  &&  ( c2 == QChar ('g') ) ) str += "Æ"; 
          else if ( ( c1 == QChar ('o') )  &&  ( c2 == QChar ('g') ) ) str += "æ"; 
          else if ( ( c1 == QChar ('m') )  &&  ( c2 == QChar ('i') ) ) str += "È"; 
          else if ( ( c1 == QChar ('o') )  &&  ( c2 == QChar ('i') ) ) str += "è"; 
          else if ( ( c1 == QChar ('m') )  &&  ( c2 == QChar ('j') ) ) str += "É"; 
          else if ( ( c1 == QChar ('o') )  &&  ( c2 == QChar ('j') ) ) str += "é"; 
          else if ( ( c1 == QChar ('m') )  &&  ( c2 == QChar ('k') ) ) str += "Ê"; 
          else if ( ( c1 == QChar ('o') )  &&  ( c2 == QChar ('k') ) ) str += "ê";
          else if ( ( c1 == QChar ('m') )  &&  ( c2 == QChar ('l') ) ) str += "Ë"; 
          else if ( ( c1 == QChar ('o') )  &&  ( c2 == QChar ('l') ) ) str += "ë"; 
          else if ( ( c1 == QChar ('m') )  &&  ( c2 == QChar ('m') ) ) str += "Ì"; 
          else if ( ( c1 == QChar ('o') )  &&  ( c2 == QChar ('m') ) ) str += "ì"; 
          else if ( ( c1 == QChar ('m') )  &&  ( c2 == QChar ('n') ) ) str += "Í"; 
          else if ( ( c1 == QChar ('o') )  &&  ( c2 == QChar ('n') ) ) str += "í"; 
          else if ( ( c1 == QChar ('m') )  &&  ( c2 == QChar ('o') ) ) str += "Î"; 
          else if ( ( c1 == QChar ('o') )  &&  ( c2 == QChar ('o') ) ) str += "î"; 
          else if ( ( c1 == QChar ('m') )  &&  ( c2 == QChar ('p') ) ) str += "Ï"; 
          else if ( ( c1 == QChar ('o') )  &&  ( c2 == QChar ('p') ) ) str += "ï"; 
          else if ( ( c1 == QChar ('n') )  &&  ( c2 == QChar ('b') ) ) str += "Ñ"; 
          else if ( ( c1 == QChar ('p') )  &&  ( c2 == QChar ('b') ) ) str += "ñ"; 
          else if ( ( c1 == QChar ('k') )  &&  ( c2 == QChar ('c') ) ) str += "¢"; 
          else if ( ( c1 == QChar ('k') )  &&  ( c2 == QChar ('j') ) ) str += "©"; 
          else if ( ( c1 == QChar ('l') )  &&  ( c2 == QChar ('f') ) ) str += "µ"; 
          else if ( ( c1 == QChar ('n') )  &&  ( c2 == QChar ('i') ) ) str += "Ø"; 
          else if ( ( c1 == QChar ('p') )  &&  ( c2 == QChar ('i') ) ) str += "ø"; 
          else if ( ( c1 == QChar ('l') )  &&  ( c2 == QChar ('j') ) ) str += "¹"; 
          else if ( ( c1 == QChar ('l') )  &&  ( c2 == QChar ('c') ) ) str += "²"; 
          else if ( ( c1 == QChar ('l') )  &&  ( c2 == QChar ('d') ) ) str += "³"; 
          else if ( ( c1 == QChar ('l') )  &&  ( c2 == QChar ('m') ) ) str += "¼"; 
          else if ( ( c1 == QChar ('l') )  &&  ( c2 == QChar ('n') ) ) str += "½"; 
          else if ( ( c1 == QChar ('l') )  &&  ( c2 == QChar ('o') ) ) str += "¾"; 
          else if ( ( c1 == QChar ('l') )  &&  ( c2 == QChar ('a') ) ) str += "°"; 
          else if ( ( c1 == QChar ('k') )  &&  ( c2 == QChar ('o') ) ) str += "®"; 
          else if ( ( c1 == QChar ('k') )  &&  ( c2 == QChar ('h') ) ) str += "§"; 
          else if ( ( c1 == QChar ('k') )  &&  ( c2 == QChar ('d') ) ) str += "£"; 
          else if ( ( c1 == QChar ('p') )  &&  ( c2 == QChar ('a') ) ) str += "ð"; 
          else if ( ( c1 == QChar ('n') )  &&  ( c2 == QChar ('a') ) ) str += "Ð"; 
          else if ( ( c1 == QChar ('l') )  &&  ( c2 == QChar ('l') ) ) str += "»"; 
          else if ( ( c1 == QChar ('k') )  &&  ( c2 == QChar ('l') ) ) str += "«"; 
          else if ( ( c1 == QChar ('l') )  &&  ( c2 == QChar ('k') ) ) str += "º"; 
          else if ( ( c1 == QChar ('l') )  &&  ( c2 == QChar ('h') ) ) str += "·"; 
          else if ( ( c1 == QChar ('k') )  &&  ( c2 == QChar ('b') ) ) str += "¡"; 
          else if ( ( c1 == QChar ('k') )  &&  ( c2 == QChar ('e') ) ) str += "¤"; 
          else if ( ( c1 == QChar ('l') )  &&  ( c2 == QChar ('b') ) ) str += "±"; 
          else if ( ( c1 == QChar ('l') )  &&  ( c2 == QChar ('p') ) ) str += "¿"; 
          else if ( ( c1 == QChar ('k') )  &&  ( c2 == QChar ('f') ) ) str += "¥"; 
          else if ( ( c1 == QChar ('p') )  &&  ( c2 == QChar ('o') ) ) str += "þ";
          else if ( ( c1 == QChar ('n') )  &&  ( c2 == QChar ('o') ) ) str += "Þ";
          else if ( ( c1 == QChar ('n') )  &&  ( c2 == QChar ('n') ) ) str += "Ý"; 
          else if ( ( c1 == QChar ('p') )  &&  ( c2 == QChar ('n') ) ) str += "ý"; 
          else if ( ( c1 == QChar ('p') )  &&  ( c2 == QChar ('p') ) ) str += "ÿ"; 
          else if ( ( c1 == QChar ('k') )  &&  ( c2 == QChar ('k') ) ) str += "ª"; 
          else if ( ( c1 == QChar ('k') )  &&  ( c2 == QChar ('m') ) ) str += "¬"; 
          else if ( ( c1 == QChar ('p') )  &&  ( c2 == QChar ('h') ) ) str += "÷";
          else if ( ( c1 == QChar ('k') )  &&  ( c2 == QChar ('g') ) ) str += "|"; 
          else if ( ( c1 == QChar ('l') )  &&  ( c2 == QChar ('e') ) ) str += "'"; 
          else if ( ( c1 == QChar ('k') )  &&  ( c2 == QChar ('i') ) ) str += "¨"; 
          else if ( ( c1 == QChar ('k') )  &&  ( c2 == QChar ('n') ) ) str += "­"; 
          else if ( ( c1 == QChar ('k') )  &&  ( c2 == QChar ('p') ) ) str += "¯"; 
          else if ( ( c1 == QChar ('l') )  &&  ( c2 == QChar ('g') ) ) str += "¶"; 
          else if ( ( c1 == QChar ('l') )  &&  ( c2 == QChar ('i') ) ) str += "¸"; 
          else if ( ( c1 == QChar ('m') )  &&  ( c2 == QChar ('h') ) ) str += "Ç"; 
          else if ( ( c1 == QChar ('o') )  &&  ( c2 == QChar ('h') ) ) str += "ç"; 
          else if ( ( c1 == QChar ('n') )  &&  ( c2 == QChar ('h') ) ) str += "×"; 
          else if ( ( c1 == QChar ('k') )  &&  ( c2 == QChar ('a') ) ) str += " "; 
          else if ( ( c1 == QChar ('a') )  &&  ( c2 == QChar ('j') ) ) str += "  &lt;TAB&gt;  "; 
          else  str = str + "&lt; unknown_applixspread_special_character: ^" + c1 + c2 + " &gt;";
#endif

        if (i>step) 
        {
            i = 0;
            progress += 2;
            emit sigProgress(progress );
        }
    }
    emit sigProgress(100);

    str += "</table>\n";
    str += "</map>\n";
    str += "</doc>\n";
    printf ("%s\n", (const char *) str.utf8());
    KoStore out = KoStore (QString(fileOut), KoStore::Write);
    if (!out.open ("root")) 
    {
        kdError(30502) << "Unable to open output file!" << endl;
        in.close  ();
        out.close ();
        return false;
    }

    QCString cstring = str.utf8 ();

    out.write (cstring.data(), cstring.length());

    out.close ();
    in.close ();
    return true;
}

