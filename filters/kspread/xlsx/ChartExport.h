/*
 * This file is part of Office 2007 Filters for KOffice
 *
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
 *
 * Contact: Sebastian Sauer sebsauer@kdab.com
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * version 2.1 as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */
#ifndef CHARTEXPORT_H
#define CHARTEXPORT_H

#include <Charting.h>

//#include <QtCore/QString>
//#include <QtCore/QStringList>
//#include <QtCore/QRect>
//#include <QtCore/QMap>
//#include <QtGui/QColor>

class KoStore;
class KoXmlWriter;

class ChartExport
{
public:
    explicit ChartExport(Charting::Chart* chart);
    ~ChartExport();
    Charting::Chart* chart() const { return m_chart; }


    QString m_href;
    QString m_cellRangeAddress;
    QString m_endCellAddress;
    QString m_notifyOnUpdateOfRanges;
#if 0
    /// anchored to sheet
    QString m_sheetName;
    /// anchored to cell
    //unsigned long m_colL, m_rwT;
#endif
    QString m_x, m_y, m_width, m_height;

    bool saveIndex(KoXmlWriter* xmlWriter);
    bool saveContent(KoStore* store, KoXmlWriter* manifestWriter);

private:
    Charting::Chart* m_chart;
};

#endif
