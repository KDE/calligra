/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>
   Copyright (C) 2000 Michael Johnson <mikej@xnet.com>

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

#ifndef RTFEXPORT_H
#define RTFEXPORT_H

#include <qstring.h>
#include <qcstring.h>
#include <qfile.h>
#include <qobject.h>

#include <kwExport.h>
#include <docinfoExport.h>
#include <koFilter.h>
#include <koStore.h>

#define KDEBUG_RTFFILTER 30505
#define O

struct CodeTable
   {
   QString mapId;
   QString page;
   };

#define CODE_PAGE_SIZE 6
// global declaration
static const CodeTable codeTable[CODE_PAGE_SIZE] = {"ISO-8859-1", "1252",   // Latin1
                                                    "ISO-8859-5", "1251",   // Cryllic
                                                    "ISO-8859-6", "1256",   // Arabic
                                                    "ISO-8859-7", "1253",   // Greek
                                                    "ISO-8859-8", "1255",   // Hebrew
                                                    "ISO-10646",  "1200"};   // Unicode

class RTFExport : public KoFilter
{
        Q_OBJECT

    public:
        RTFExport ( KoFilter    *parent,
                        const char  *name    ) : KoFilter (parent, name) {}

        virtual ~RTFExport () {}

        virtual bool filter ( const QString &fileIn,
                              const QString &fileOut,
                              const QString &from,
                              const QString &to,
                              const QString &config = QString::null );
};



/************************************************************************/

// The following define markup types in RTF
enum paraNumberingType{numeric, alpha, ALPHA, rom, ROMAN};
enum FontType{roman, swiss, modern, script, decor, tech};


/************************************************************************/
// FontTable is for holding font information that goes at the beginning
// of an rtf file
class FontTable
   {
   public :
   QString fontName;
   FontType fontType;  // roman, swiss, etc
   };

/***********************************************************************/
class ColorTable
   {
   public:

   ColorTable()  {}
   ColorTable ( int r,
                int g,
                int b ) : red(r), green(g), blue(b) {}
   int red;
   int green;
   int blue;
   };

/************************************************************************/
class Variable
   {
   public :
   Variable() {}
   Variable ( int p, QString m ) : pos(p), markup(m) {}


   int pos;
   QString markup;
   };

/************************************************************************/

QString fontMarkup(QString fontName, QValueList< FontTable > &fontTable,
                   QString &fontHeader);

QString fontTableMarkup(QString fontName, QValueList< FontTable > &fontTable,
                   QString &fontHeader, FontType fontType, int counter);

QString listStart( QString font, int fontSize, QString listMarker);

QString listMarkup( int firstIndent,int listType, int startNumber, int depth,
                    int fontSize, QString font,
                    QString preceedingText, QString followingText,
                    bool section, bool multiLevel);

QString escapeRTFsymbols( QString text);

void paperSize( PaperAttributes &paper, PaperBorders &paperBorders  );

void ProcessParagraphData ( QString                     &paraText,
                            QValueList<FormatData>      &paraFormatDataList,
                            QValueList<AnchoredInsert>  &anchoredInsertList,
                            QString                     &outputText,
                            QValueList<Variable>        *varList          );

QString encodeSevenBit( QString text);

QValueList<FormatData>  combineFormatData(  QValueList<FormatData> &paraFormatData,
                   QValueList<FormatData> &paraFormatDataFormats );

QString ProcessTabData( QValueList < TabularData > &tabData );

QString colorMarkup(int red, int blue, int green,
                 QValueList< ColorTable > &colorTable,
                 QString &colorHeader);

QString borderMarkup (QString borderId, BorderStyle *border );

void processVariables( QValueList<Variable>&varList,
                       QValueList<FormatData>paraFormat );

#endif // RTFEXPORT_H
