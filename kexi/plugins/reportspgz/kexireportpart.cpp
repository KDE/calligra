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


#include "kexireportpart.h"

#include <qlabel.h>

#include <kmainwindow.h>
#include <klocale.h>
#include <kdebug.h>
#include <core/KexiWindow.h>
#include "kexireportview.h"
#include "kexireportdesignview.h"
#include <core/KexiMainWindowIface.h>
#include "reportentityselector.h"
#include <ktabwidget.h>

//! @internal
class KexiReportPart::Private
{
public:
    Private() {
        res = 0;
    }
    ~Private() {
    }
    ReportEntitySelector *res;
};

KexiReportPart::KexiReportPart(QObject *parent, const QStringList &l)
        : KexiPart::Part(9, parent, l)
        , d(new Private())
{
    kDebug();
    setInternalPropertyValue("instanceCaption", i18n("Report"));
    setInternalPropertyValue("instanceToolTip", i18nc("tooltip", "Create new report"));
    setInternalPropertyValue("instanceWhatsThis", i18nc("what's this", "Creates new report."));
    setSupportedViewModes(Kexi::DataViewMode | Kexi::DesignViewMode);
    setInternalPropertyValue("newObjectsAreDirty", true);

    //m_registeredPartID = 9; //Do not want to use the already registered report type just yet(int)KexiPart::ReportObjectType;

}

KexiReportPart::~KexiReportPart()
{
}

KexiView* KexiReportPart::createView(QWidget *parent, KexiWindow* window,
                                     KexiPart::Item &item, Kexi::ViewMode viewMode, QMap<QString, QVariant>*)
{
    kDebug();
    KexiView* view;

    if (viewMode == Kexi::DataViewMode) {
        view = new KexiReportView(parent);
    } else if (viewMode == Kexi::DesignViewMode) {
        view = new KexiReportDesignView(parent, d->res);
    }
    return view;
}

QString KexiReportPart::loadReport(const QString& name)
{
    //_internal->_reportName = pReportName;
    KexiMainWindowIface *win = KexiMainWindowIface::global();
    if (!win || !win->project() || !win->project()->dbConnection()) {
        kDebug() << "failed sanity check";
        return "";
    }
    QString src, did;
    KexiDB::SchemaData sd;
    if (win->project()->dbConnection()->loadObjectSchemaData(9, name, sd) != true) {
        kDebug() << "failed to loafd schema data";
        return "";
    }

    kDebug() << "***Object ID:" << sd.id();
    if (win->project()->dbConnection()->loadDataBlock(sd.id(), src, "pgzreport_layout") == true) {
        return src;
    } else {
        kDebug() << "Unable to load document";
        return "";
    }
}

KexiWindowData* KexiReportPart::createWindowData(KexiWindow* window)
{
    kDebug();
    KexiReportPart::TempData *td = new KexiReportPart::TempData(window);
    td->document = loadReport(window->partItem()->name());
    td->name = window->partItem()->name();
    return td;
}

KexiReportPart::TempData::TempData(QObject* parent)
        : KexiWindowData(parent)
        , reportSchemaChangedInPreviousView(true /*to force reloading on startup*/)
{
    document = "";
}

void KexiReportPart::setupCustomPropertyPanelTabs(KTabWidget *tab)
{
    if (!d->res)
        d->res = new ReportEntitySelector(tab);

    tab->addTab(d->res, i18n("Controls"));
}

#include "kexireportpart.moc"
