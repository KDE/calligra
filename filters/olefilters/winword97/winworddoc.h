/* This file is part of the KDE project
   Copyright (C) 1999 Werner Trobin <trobin@kde.org>

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

#ifndef WINWORDDOC_H
#define WINWORDDOC_H

#include <document.h>
#include <properties.h>
#include <qarray.h>
#include <qobject.h>
#include <qstring.h>
#include <qdom.h>
#include <qvector.h>

class myFile;

class WinWordDoc:
    public QObject, private Document
{

    Q_OBJECT

public:
    WinWordDoc(
        QCString &result,
        const myFile &mainStream,
        const myFile &table0Stream,
        const myFile &table1Stream,
        const myFile &dataStream);
    ~WinWordDoc();

    bool isOk() const { return m_success; }

    bool convert();

signals:
    // See olefilter.h for information
    void signalSavePic(
        const QString &nameIN,
        QString &storageId,
        const QString &extension,
        unsigned int length,
        const char *data);

    void signalSavePart(
        const QString &nameIN,
        QString &storageId,
        QString &mimeType,
        const QString &extension,
        const QString &config,
        unsigned int length,
        const char *data);

    void signalPart(
        const char *nameIN,
        QString &storageId,
        QString &mimeType);

private:
    WinWordDoc(const WinWordDoc &);
    const WinWordDoc &operator=(const WinWordDoc &);

    // The conversion is done exactly once. Has it already happened?

    bool m_isConverted;
    bool m_success;

    // Convert from Word text into XML-friendly text.

    void encode(QString &text);

    QString colour(
        unsigned colour,
        const char *red,
        const char *green,
        const char *blue,
        unsigned defaultColour = 1) const;

    // Number format codes.

    const char *list(unsigned nfc) const;
    char numbering(unsigned nfc) const;

    // Justification codes.

    const char *justification(unsigned jc) const;

    // Convert from Word character format to our own format.
    QString generateFormat(
        const CHP *chp);
    QString generateFormats(
        Attributes &attributes);

    void gotError(
        const QString &text);
    void gotParagraph(
        const QString &text,
        Attributes &style);
    virtual void gotHeadingParagraph(
        const QString &text,
        Attributes &style);
    virtual void gotListParagraph(
        const QString &text,
        Attributes &style);
    virtual void gotStyle(
        const QString &name,
        const Properties &style);
    virtual void gotTableBegin(
        unsigned tableNumber);
    virtual void gotTableEnd(
        unsigned tableNumber);
    virtual void gotTableRow(
        unsigned tableNumber,
        const QString texts[],
        const Attributes styles[],
        MsWordGenerated::TAP &row);
    void gotTableRow(
        unsigned tableNumber, unsigned rowNumber, const QString texts[], const MsWordGenerated::PAP styles[],
        MsWordGenerated::TAP &row);

    // This is where the result will end up!

    QCString &m_result;

    // Word has a very flexible concept of columns: each row can vary the
    // edges of each column. We must map this onto a set of fixed-width columns
    // by defining columns on each edge, and then using joined cells to model
    // the original Word cells. We accumulate all the known edges for a given
    // table in an array, and store the per-table arrays in a vector.

    QVector< QArray<unsigned> > m_cellEdges;
    int cacheCellEdge(
        unsigned tableNumber,
        unsigned cellEdge);
    unsigned computeCellEdge(
        MsWord::TAP &row,
        unsigned edge);
    class TableRow
    {
    public:
        TableRow(
            const QString texts[],
            const Attributes styles[],
            MsWordGenerated::TAP &row);
        ~TableRow();
        QValueList<Attributes> m_styles;
        QStringList m_texts;
        MsWordGenerated::TAP m_row;
    };
    QVector<TableRow> m_table;

    // Since there is no way to fill m_part incrementally with XML content,
    // we will fill m_body instead.

    QString m_body;
    QString m_tables;
    QString m_pixmaps;
    QString m_embedded;

    // Page sizes, margins etc. all in points.

    static const unsigned s_height = 841; // Height.
    static const unsigned s_width = 595;  // Width.
    static const unsigned s_hMargin = 28; // Horizontal margin.
    static const unsigned s_vMargin = 42; // Vertical margin.
};
#endif // WINWORDDOC_H
