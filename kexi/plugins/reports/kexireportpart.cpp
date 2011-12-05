/*
 * Kexi Report Plugin
 * Copyright (C) 2007-2008 by Adam Pigg <adam@piggz.co.uk>
 * Copyright (C) 2011 Jarosław Staniek <staniek@kde.org>
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


#include "kexireportpart.h"

#include <qlabel.h>

#include <kmainwindow.h>
#include <klocale.h>
#include <kdebug.h>
#include <core/KexiWindow.h>
#include "kexireportview.h"
#include "kexireportdesignview.h"
#include <core/KexiMainWindowIface.h>
#include <ktabwidget.h>

#include "kexisourceselector.h"

//! @internal
class KexiReportPart::Private
{
public:
    Private() : toolboxActionGroup(0)
    {
        ksrc = 0;
    }
    ~Private() {
    }
    KexiSourceSelector *ksrc;
    QActionGroup toolboxActionGroup;
    QMap<QString, QAction*> toolboxActionsByName;
};

KexiReportPart::KexiReportPart(QObject *parent, const QVariantList &l)
  : KexiPart::Part(parent,
        i18nc("Translate this word using only lowercase alphanumeric characters (a..z, 0..9). "
              "Use '_' character instead of spaces. First character should be a..z character. "
              "If you cannot use latin characters in your language, use english word.",
              "report"),
        i18nc("tooltip", "Create new report"),
        i18nc("what's this", "Creates new report."),
        l)
  , d(new Private)
{
    setInternalPropertyValue("newObjectsAreDirty", true);
}

KexiReportPart::~KexiReportPart()
{
    delete d;
}

KexiView* KexiReportPart::createView(QWidget *parent, KexiWindow* window,
                                     KexiPart::Item &item, Kexi::ViewMode viewMode, QMap<QString, QVariant>*)
{
    Q_UNUSED(window);
    Q_UNUSED(item);

    KexiView* view = 0;

    if (viewMode == Kexi::DataViewMode) {
        view = new KexiReportView(parent);

    } else if (viewMode == Kexi::DesignViewMode) {
        view = new KexiReportDesignView(parent, d->ksrc);
        connect(d->ksrc, SIGNAL(setData(KoReportData*)), view, SLOT(slotSetData(KoReportData*)));
        connect(view, SIGNAL(itemInserted(QString)), this, SLOT(slotItemInserted(QString)));
    }
    return view;
}

void KexiReportPart::initPartActions()
{
    KexiMainWindowIface *win = KexiMainWindowIface::global();
    QList<QAction*> reportActions = KoReportDesigner::actions(&d->toolboxActionGroup);

    foreach(QAction* action, reportActions) {
        connect(action, SIGNAL(triggered(bool)), this, SLOT(slotToolboxActionTriggered(bool)));
        win->addToolBarAction("report", action);
        d->toolboxActionsByName.insert(action->objectName(), action);
    }
    
}

QString KexiReportPart::loadReport(const QString& name)
{
    //_internal->_reportName = pReportName;

    KexiMainWindowIface *win = KexiMainWindowIface::global();
    KexiDB::Connection *conn;
    if (!win || !win->project() || !((conn = win->project()->dbConnection()))) {
        kDebug() << "failed sanity check: !win || !win->project() || !((conn = win->project()->dbConnection()))";
        return QString();
    }
    QString src, did;
    KexiDB::SchemaData sd;

    if (conn->loadObjectSchemaData(win->project()->idForClass("org.kexi-project.report"), name, sd) != true
        && conn->loadObjectSchemaData(win->project()->idForClass("uk.co.piggz.report"), name, sd) != true /* compat. */)
    {
        kWarning() << "failed to load schema data";
        return QString();
    }

    kDebug() << "***Object ID:" << sd.id();

    if (   win->project()->dbConnection()->loadDataBlock(sd.id(), src, "layout") == true
        || win->project()->dbConnection()->loadDataBlock(sd.id(), src, "pgzreport_layout") == true /* compat */)
    {
        return src;
    }

    kWarning() << "Unable to load document";
    return QString();
}

KexiWindowData* KexiReportPart::createWindowData(KexiWindow* window)
{
    kDebug();
    const QString document(loadReport(window->partItem()->name()));
    KexiReportPart::TempData *td = new KexiReportPart::TempData(window);
    
    QDomDocument doc;
    doc.setContent(document);
    
    kDebug() << doc.toString();
    
    QDomElement root = doc.documentElement();
    QDomElement korep = root.firstChildElement("report:content");
    QDomElement conn = root.firstChildElement("connection");

    td->reportDefinition = korep;
    td->connectionDefinition = conn;

    td->name = window->partItem()->name();
    return td;
}

KexiReportPart::TempData::TempData(QObject* parent)
        : KexiWindowData(parent)
        , reportSchemaChangedInPreviousView(true /*to force reloading on startup*/)
{
}

void KexiReportPart::setupCustomPropertyPanelTabs(KTabWidget *tab)
{
    if (!d->ksrc)
        d->ksrc = new KexiSourceSelector(tab, KexiMainWindowIface::global()->project()->dbConnection());
    tab->addTab(d->ksrc, KIcon("server-database"), QString());
    tab->setTabToolTip(tab->indexOf(d->ksrc), i18n("Data Source"));
}

void KexiReportPart::slotToolboxActionTriggered(bool checked)
{
    if (!checked)
        return;
    QObject *theSender = sender();
    if (!theSender)
        return;

    QString senderName = sender()->objectName();
    KexiMainWindowIface *mainwin = KexiMainWindowIface::global();

    KexiWindow *win = mainwin->currentWindow();

    if (!win)
        return;

    KexiView *designView = win->viewForMode(Kexi::DesignViewMode);

    if (designView) {
        KexiReportDesignView *dv = dynamic_cast<KexiReportDesignView*>(designView);
        if (!dv)
            return;
        dv->triggerAction(senderName);
    }
}

void KexiReportPart::slotItemInserted(const QString& entity)
{
    Q_UNUSED(entity);
    // uncheck toolbox action after it is used
    QAction * a = d->toolboxActionGroup.checkedAction();
    if (a) {
        a->setChecked(false);
    }
}

#include "kexireportpart.moc"
