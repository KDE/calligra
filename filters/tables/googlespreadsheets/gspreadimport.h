/* This file is part of the KDE project
   Copyright (C) 2010-2011 Christoph Goerlich <chgoerlich@gmx.de>

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
   Boston, MA 02110-1301, USA.
*/

#ifndef GSPREADFILTER_H
#define GSPREADFILTER_H

#include <KoFilter.h>

#include "kspread/Sheet.h"
#include "part/Doc.h"

#include <QVariantList>
#include <QXmlStreamAttributes>

namespace KSpread
{
    class Cell;
}


class GSpreadImport : public KoFilter
{
    Q_OBJECT;
    
public:
    explicit GSpreadImport(QObject* parent, const QVariantList&);
    virtual ~GSpreadImport() {}

    virtual KoFilter::ConversionStatus convert(const QByteArray& from, const QByteArray& to);

private:
    bool readFeed(QByteArray &input);
    void readEntry();
    QString convertFormula(QString &formula, KSpread::Cell &cell);
    
    QString errorString();
    
    QXmlStreamReader m_xmlReader;
    
    int m_rows;
    int m_cols;
    
    KSpread::Sheet *m_sheet;
    KSpread::Doc *m_doc;
};

#endif //GSPREADFILTER_H
