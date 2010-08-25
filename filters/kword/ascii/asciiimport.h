/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>
   Copyright (C) 2000 Michael Johnson <mikej@xnet.com>
   Copyright (C) 2001, 2002 Nicolas GOUTTE <goutte@kde.org>

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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef ASCIIIMPORT_H
#define ASCIIIMPORT_H

#include <KoFilter.h>
//Added by qt3to4:
#include <QTextStream>
#include <QByteArray>

class QString;
class QTextStream;
class QDomDocument;
class QDomElement;

#define MAXLINES  1000
#define MAXCOLUMNS 15

const double ptsperline = 15.0;
const double ptsperchar = 6.0;
const int spacespertab = 6;
const double leftmargin = 56.0;

const int shortline = 40;  // max length of a "short" line

struct Tabs {
    int columns;
    int indent;
    QString field[MAXCOLUMNS];
    int width[MAXCOLUMNS];
};

struct Position {
    double top;
    double left;
    double bottom;
    double right;
};

class ASCIIImport : public KoFilter
{

    Q_OBJECT

public:
    ASCIIImport(QObject *parent, const QStringList &);
    ASCIIImport();
    virtual ~ASCIIImport() {}

    virtual KoFilter::ConversionStatus convert(const QByteArray& from, const QByteArray& to);
private: // not yet changed
#if 0
    void WriteOutTableCell(int table_no, int row, int col, Position *pos,
                           QString &str);

    bool Table(QString *Line, int *linecount, int no_lines,
               int table_no, QString &tbl, QString &str);



    bool ListItem(QString *Line, int no_lines,
                  QString &str);
#endif
    bool IsListItem(QString FirstLine, QChar mark);

private:
    void prepareDocument(QDomDocument& mainDocument, QDomElement& mainFramesetElement);
    void processParagraph(QDomDocument& mainDocument,
                          QDomElement& mainFramesetElement, const QStringList& paragraph);
    void writeOutParagraph(QDomDocument& mainDocument,
                           QDomElement& mainFramesetElement,  const QString& name,
                           const QString& text, const int firstindent, const int secondindent);
    int Indent(const QString& line) const;
    int MultSpaces(const QString& text, const int index) const;
    bool IsWhiteSpace(const QChar& c) const;
    QString readLine(QTextStream& textstream, bool& lastCharWasCr);
private: // converting functions
    void oldWayConvert(QTextStream& stream, QDomDocument& mainDocument,
                       QDomElement& mainFramesetElement);
    void asIsConvert(QTextStream& stream, QDomDocument& mainDocument,
                     QDomElement& mainFramesetElement);
    void sentenceConvert(QTextStream& stream, QDomDocument& mainDocument,
                         QDomElement& mainFramesetElement);
};
#endif // ASCIIIMPORT_H
