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
 * License along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef KEXIREPORTVIEW_H
#define KEXIREPORTVIEW_H

#include <core/KexiView.h>
#include <kexidb/connection.h>
#include <qdom.h>
#include "kexireportpart.h"
#include <KoReportRendererBase.h>

class KoReportPreRenderer;
class ORODocument;
class QScrollArea;
class KoReportPage;
class KexiReportViewPageSelect;
class KexiRecordNavigator;
class KexiScriptAdaptor;
class KRScriptFunctions;

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
    KoReportPreRenderer *m_preRenderer;
    ORODocument *m_reportDocument;
    QScrollArea *m_scrollArea;
    KoReportPage *m_reportWidget;
    KexiRecordNavigator *m_pageSelector;
    int m_currentPage;
    int m_pageCount;
    KexiReportPart::TempData* tempData() const;
    KoReportData* sourceData(QDomElement e);

    KexiScriptAdaptor *m_kexi;
    KRScriptFunctions *m_functions;
    KoReportRendererFactory m_factory;
    
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
