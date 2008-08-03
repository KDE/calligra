/*
 * OpenRPT report writer and rendering engine
 * Copyright (C) 2001-2007 by OpenMFG, LLC
 * Copyright (C) 2007-2008 by Adam Pigg (adam@piggz.co.uk)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 * Please contact info@openmfg.com with any questions on this license.
 */

/*
 *     This is a collection of various functions used to parse the
 * different xml structures used in the Report definitions.  Most
 * of these are structures that are common to several or the complex
 * structures.
 */

#ifndef __PARSEXMLUTILS_H__
#define __PARSEXMLUTILS_H__

// forward declarations
class QDomElement;
class KRObjectData;
class KRLineData;
class KRLabelData;
class KRFieldData;
class KRTextData;
class KRBarcodeData;
class KRImageData;

#include "krsectiondata.h"
#include "reportpageoptions.h"

#include <qstring.h>
#include <QRectF>
#include <qfont.h>
#include <qlist.h>
#include <qmap.h>

#include <QList>
#include <QPair>

//
// Data Structures
//
class ORDataData
{
public:
    ORDataData() {
        query = QString::null;
        column = QString::null;
    }
    ORDataData(const QString & q, const QString & c) {
        query = q;
        column = c;
    }
    ORDataData(const ORDataData & d) {
        query = d.query;
        column = d.column;
    }

    ORDataData & operator= (const ORDataData & d) {
        query = d.query;
        column = d.column;
        return *this;
    }

    bool operator== (const ORDataData & d) const {
        return ((query == d.query) && (column == d.column));
    }

    bool operator< (const ORDataData & d) const {
        if ((query < d.query) || (query == d.query && column < d.column))
            return true;
        return false;
    }

    QString query;
    QString column;
};

class ORTextStyleData
{
public:
    QFont font;
    Qt::Alignment textFlags;
    QColor bgColor;
    QColor fgColor;
    int bgOpacity;

};

class ORLineStyleData
{
public:
    int weight;
    QColor lnColor;
    Qt::PenStyle style;
};


class ORDetailGroupSectionData
{
public:
    ORDetailGroupSectionData();

    enum PageBreak {
        BreakNone = 0,
        BreakAfterGroupFoot = 1
    };

    //QString name;
    QString column;
    int pagebreak;

    QMap<ORDataData, qreal> _subtotCheckPoints;

    KRSectionData * head;
    KRSectionData * foot;
};



bool parseReportRect(const QDomElement &, QRectF &);
bool parseReportFont(const QDomElement &, QFont &);

bool parseReportTextStyleData(const QDomElement &, ORTextStyleData &);
bool parseReportLineStyleData(const QDomElement &, ORLineStyleData &);

#endif
