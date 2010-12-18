/* This file is part of the KOffice project
   Copyright (C) 2002 Werner Trobin <trobin@kde.org>
   Copyright (C) 2002 David Faure <faure@kde.org>
   Copyright (C) 2009 Inge Wallin   <inge@lysator.liu.se>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the Library GNU General Public
   version 2 of the License, or (at your option) version 3 or,
   at the discretion of KDE e.V (which shall act as a proxy as in
   section 14 of the GPLv3), any later version..

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef TABLEHANDLER_H
#define TABLEHANDLER_H

#include <wv2/src/functor.h>
#include <wv2/src/word97_generated.h> // for TAP
#include <wv2/src/handlers.h>
#include <QString>
#include <QObject>
#include <QList>
#include <deque>
#include <QRectF>

#include <KoXmlWriter.h>
#include <KoGenStyles.h>

class Document;

namespace wvWare
{
namespace Word97 {
class TC;
class SHD;
}
}
class KoRect;

namespace KWord
{
typedef const wvWare::TableRowFunctor* TableRowFunctorPtr;
typedef wvWare::SharedPtr<const wvWare::Word97::TAP> TAPptr;

// Data for a given row table. This struct is used by the Table struct.
struct Row {
    Row() : functorPtr(0L), tap(0L)  {}     // QValueList wants that one
    Row(TableRowFunctorPtr ptr, TAPptr _tap) : functorPtr(ptr), tap(_tap) {}
    ~Row() {}

    // Each row has: a functor to call to parse it and a TAP (table row properties)
    TableRowFunctorPtr functorPtr;
    TAPptr tap;
};

// Data for a given table, stored between the 'tableRowFound' callback
// during text parsing and the final generation of table cells.
struct Table {
    QString name; // kword's grpMgr attribute
    QList<Row> rows; // need to use QValueList to benefit from implicit sharing

    // Word has a very flexible concept of columns: each row can vary the
    // edges of each column. We must map this onto a set of fixed-width columns
    // by defining columns on each edge, and then using joined cells to model
    // the original Word cells. We accumulate all the known edges for a given
    // table in an array.
    // Important: don't use unsigned int. Value can be negative (relative to margin...).
    QList<int> m_cellEdges;

    // table properties
    TAPptr tap;

    void cacheCellEdge(int cellEdge);
    int columnNumber(int cellEdge) const;
};
}

class KWordTableHandler : public QObject, public wvWare::TableHandler
{
    Q_OBJECT
public:
    KWordTableHandler(KoXmlWriter* bodyWriter, KoGenStyles* mainStyles);

    //////// TableHandler interface
    virtual void tableRowStart(wvWare::SharedPtr<const wvWare::Word97::TAP> tap);
    virtual void tableRowEnd();
    virtual void tableCellStart();
    virtual void tableCellEnd();

    ///////// Our own interface
    Document* document() const {
        return m_document;
    }
    void setDocument(Document * document) {
        m_document = document;
    }

    void tableStart(KWord::Table* table);
    void tableEnd();

protected:
    double rowHeight() const;

private:
    // Table can be present in {header, footer, footnote, endnote, body}.
    // @return the correct writer.
    KoXmlWriter* currentWriter() const;

    // The document owning this table handler.
    Document*     m_document;

    KWord::Table* m_currentTable;

    KoXmlWriter*  m_bodyWriter;
    KoGenStyles*  m_mainStyles;

    int m_row;
    int m_column;
    double m_currentY;
    wvWare::SharedPtr<const wvWare::Word97::TAP> m_tap;
    bool m_cellOpen;
    int m_colSpan;

    QString m_borderStyle[6];
    QString m_margin[6];

    bool m_floatingTable; //true - table is floatin table; false - table is not floating table
};

#endif // TABLEHANDLER_H
