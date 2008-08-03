/*
 * Kexi Report Plugin
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
#ifndef KEXIREPORTVIEW_H
#define KEXIREPORTVIEW_H

#include <kexi_export.h>
#include <core/KexiView.h>
#include <kexidb/connection.h>
#include <qdom.h>
#include "kexireportpart.h"
class ORPreRender;
class ORODocument;
class QScrollArea;
class KexiReportPage;
class KexiReportViewPageSelect;
class KexiRecordNavigator;

/**
 @author Adam Pigg <adam@piggz.co.uk>
*/
class KexiReportView : public KexiView
{
    Q_OBJECT
public:
    KexiReportView(QWidget *parent);

    ~KexiReportView();

    virtual tristate afterSwitchFrom(Kexi::ViewMode mode);
    virtual tristate beforeSwitchTo(Kexi::ViewMode mode, bool &dontStore);

private:
    ORPreRender *rpt;
    ORODocument *doc;
    QScrollArea *scr;
    KexiReportPage *rptwid;
    KexiRecordNavigator *pageSelector;
    int curPage;
    int pageCount;
    KexiReportPart::TempData* tempData() const;

private slots:
    void nextPage();
    void prevPage();
    void firstPage();
    void lastPage();
    void slotPrintReport();
    void slotRenderKSpread();
    void slotExportHTML();

};

#endif
