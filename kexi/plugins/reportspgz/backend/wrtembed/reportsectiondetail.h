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

#ifndef REPORTSECTIONDETAIL_H
#define REPORTSECTIONDETAIL_H

#include <qwidget.h>
#include "reportdesigner.h"

class QDomNode;
class ReportSectionDetailGroup;
/**
 @author
*/
class ReportSectionDetail : public QWidget
{
    Q_OBJECT
public:
    ReportSectionDetail(ReportDesigner * rptdes, const char * name = 0);
    virtual ~ReportSectionDetail();

    enum PageBreak {
        BreakNone = 0,
        BreakAtEnd = 1
    };

    void setPageBreak(int);
    int pageBreak() const;

    ReportSection * getDetail();

    void buildXML(QDomDocument & doc, QDomElement & section);
    void initFromXML(QDomNode & node);

    ReportDesigner * reportDesigner();

    int groupSectionCount();
    ReportSectionDetailGroup * getSection(int i);
    void insertSection(int idx, ReportSectionDetailGroup * rsd);
    int findSection(const QString & name);
    void removeSection(int idx, bool del = FALSE);

    virtual QSize sizeHint() const;
protected:
    QString _query;

    QString _name;
    ReportSection * _detail;
    ReportDesigner * _rd;

    QList<ReportSectionDetailGroup*> groupList;

    QVBoxLayout * vboxlayout;

    int _pagebreak;
};

#endif
