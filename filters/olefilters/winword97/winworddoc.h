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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef WINWORDDOC_H
#define WINWORDDOC_H

#include <document.h>
#include <properties.h>
#include <qmemarray.h>
#include <qobject.h>
#include <qstring.h>
#include <qdom.h>
#include <qvaluelist.h>
#include <qptrvector.h>
#include <qstringlist.h>

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
    virtual ~WinWordDoc();

    bool isOk() const { return m_success; }

    bool convert();

public slots:
    void internalCommDelayStream( const char* delay );
    void internalCommShapeID( unsigned int& shapeID );

signals:
    // See olefilter.h for information
    void signalSaveDocumentInformation(
        const QString &fullName,
        const QString &title,
        const QString &company,
        const QString &email,
        const QString &telephone,
        const QString &fax,
        const QString &postalCode,
        const QString &country,
        const QString &city,
        const QString &street,
	const QString &docTitle,
	const QString &docAbstract);

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
        unsigned int length,
        const char *data);

    void signalPart(
        const QString& nameIN,
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

    // Color for number.

    QColor colorForNumber(QString number, int defaultcolor, bool defaultWhite = false);

    QString justification(unsigned jc) const;      // Justification codes.
    const char *list(unsigned nfc) const;           // Number format codes.
    char numbering(unsigned nfc) const;
    char borderStyle(unsigned btc) const;           // Border type codes.

    // Convert from Word character format to our own format.
    QString generateBorder(
        const char *borderName,
        const BRC &brc);
    QString generateColour(
        const char *colourName,
        const QColor &colour);
    QString generateFormat(
        const CHP *chp);
    QString generateFormats(
        Attributes &attributes);

    void gotError(
        const QString &text);
    void gotDocumentInformation(
	const QString &title,
	const QString &subject,
	const QString &author,
	const QString &lastRevisedBy);
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
        const QValueList<Attributes *> styles,
        MsWordGenerated::TAP &row);

    // This is where the result will end up!

    QCString &m_result;

    // Word has a very flexible concept of columns: each row can vary the
    // edges of each column. We must map this onto a set of fixed-width columns
    // by defining columns on each edge, and then using joined cells to model
    // the original Word cells. We accumulate all the known edges for a given
    // table in an array, and store the per-table arrays in a vector.

    QPtrVector< QMemArray<unsigned> > m_cellEdges;
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
            const QValueList<Attributes *> styles,
            MsWordGenerated::TAP &row);
        ~TableRow();
        QValueList<Attributes *> m_styles;
        QStringList m_texts;
        MsWordGenerated::TAP m_row;
    };
    QPtrVector<TableRow> m_table;

    // Since there is no way to fill m_part incrementally with XML content,
    // we will fill m_body instead.

    QString m_body;
    QString m_tables;
    QString m_pixmaps;
    unsigned m_pictureCount;
    QString m_embedded;
    unsigned m_embeddedCount;
    QString m_extraFrameSets;

    // hacky: These variables hold the delay and the shapeID
    const char* m_delay;
    unsigned int m_shapeID;

    // Page sizes, margins etc. all in points.

    static const unsigned s_height;  // Height.
    static const unsigned s_width;   // Width.
    static const unsigned s_hMargin; // Horizontal margin.
    static const unsigned s_vMargin; // Vertical margin.
};
#endif // WINWORDDOC_H
