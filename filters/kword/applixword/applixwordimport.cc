/* This file is part of the KDE project
   Copyright (C) 2000 Enno Bartels <ebartels@nwn.de>

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
#include <applixwordimport.h>
#include <applixwordimport.moc>
#include <kdebug.h>

APPLIXWORDImport::APPLIXWORDImport(KoFilter *parent, const char *name) :
                     KoFilter(parent, name) 
{
}

const bool APPLIXWORDImport::filter(const QString &fileIn, const QString &fileOut,
                               const QString& from, const QString& to,
                               const QString &) 
{

    if(to!="application/x-kword" || from!="application/x-applixword")
        return false;

    QFile in(fileIn);
    if(!in.open(IO_ReadOnly)) {
        kdError(30502) << "Unable to open input file!" << endl;
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
   
    int merker=0;
    QTextStream stream(&in);
    int step = in.size()/50;
    int value= 0;
    int i = 0;
    while (!stream.atEnd())
    {
        ++i;
        QChar c, c1, c2;
        stream >> c;

        if ( (merker == 4) && (c == QChar ('\n')) ) 
        {
          merker = 3;
        }

        if ( (merker == 4) && (c == QChar ('\\')) ) 
        {
	  merker = 3;
        }
        else if ((merker == 0) && c == QChar ('<'))  
        {
          stream >> c1; 
          // search for text
          if (c1 == QChar ('T'))
	  {
            merker = 3;
            stream >> c2;
            stream >> c2;
	  }
	}
        else if ((merker == 4) && c == QChar ('<'))  
        {
          str = str + "&lt; ";
          merker = 3;
	}
        else if ((merker == 4) && c == QChar ('>'))  
        {
          str = str + "&gt; ";
          merker = 3;
	}
        else if ( (merker == 4) && (c == QChar ('"') ) ) 
        {
          merker = 0;            
          str += "</TEXT>\n";
          str += "</PARAGRAPH>\n";
          str += "<PARAGRAPH>\n";
          str += "<TEXT>";
	}
        else if ( c == QChar ('&') ) str += "&amp;";
        else if ( c == QChar ('^') )
	{
          stream >> c1;
          stream >> c2;

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

          else  str = str + "&lt; unknown_applixword_special_character: ^" + c1 + c2 + " &gt;";
              
          merker = 3;
	}
        
        if (merker == 4) 
          str += c;

        if (merker == 3) merker = 4;

        if (i>step) 
        {
            i=0;
            value+=2;
            emit sigProgress(value);
        }
    }
    emit sigProgress(100);

    str += "</TEXT>\n";
    str += "</PARAGRAPH>\n";
    str += "</FRAMESET>\n";
    str += "</FRAMESETS>\n";
    str += "</DOC>\n";
    printf ("Text %s\n", (const char *) str.utf8());
    KoStore out = KoStore (QString(fileOut), KoStore::Write);
    if (!out.open ("root")) 
    {
        kdError(30502) << "Unable to open output file!" << endl;
        in.close  ();
        out.close ();
        return false;
    }

    QCString cstring = str.utf8 ();

    out.write ((const char*)cstring, cstring.length());

    out.close ();
    in.close ();
    return true;
}

