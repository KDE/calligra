/*
 * Kexi Report Plugin
 * Copyright (C) 2007-2008 by Adam Pigg <adam@piggz.co.uk>
 * Copyright (C) 2011-2015 Jarosław Staniek <staniek@kde.org>
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

#include <QTabWidget>
#include <QDebug>

#include <KLocalizedString>

#include <KexiIcon.h>
#include <core/KexiWindow.h>
#include "kexireportview.h"
#include "kexireportdesignview.h"
#include <core/KexiMainWindowIface.h>
#include "kexisourceselector.h"

//! @internal
class KexiReportPart::Private
{
public:
    Private() : toolboxActionGroup(0)
    {
        sourceSelector = 0;
    }
    ~Private() {
    }
    KexiSourceSelector *sourceSelector;
    QActionGroup toolboxActionGroup;
    QMap<QString, QAction*> toolboxActionsByName;
};

KexiReportPart::KexiReportPart(QObject *parent, const QVariantList &l)
  : KexiPart::Part(parent,
        xi18nc("Translate this word using only lowercase alphanumeric characters (a..z, 0..9). "
              "Use '_' character instead of spaces. First character should be a..z character. "
              "If you cannot use latin characters in your language, use english word.",
              "report"),
        xi18nc("tooltip", "Create new report"),
        xi18nc("what's this", "Creates new report."),
        l)
  , d(new Private)
{
    setInternalPropertyValue("newObjectsAreDirty", true);
}

KexiReportPart::~KexiReportPart()
{
    delete d;
}

KLocalizedString KexiReportPart::i18nMessage(
    const QString& englishMessage, KexiWindow* window) const
{
    Q_UNUSED(window);
    if (englishMessage == "Design of object <resource>%1</resource> has been modified.")
        return kxi18nc(I18NC_NOOP("@info", "Design of report <resource>%1</resource> has been modified."));
    if (englishMessage == "Object <resource>%1</resource> already exists.")
        return kxi18nc(I18NC_NOOP("@info", "Report <resource>%1</resource> already exists."));

    return Part::i18nMessage(englishMessage, window);
}

KexiView* KexiReportPart::createView(QWidget *parent, KexiWindow* window,
                                     KexiPart::Item *item, Kexi::ViewMode viewMode, QMap<QString, QVariant>*)
{
    Q_ASSERT(item);
    Q_UNUSED(window);
    Q_UNUSED(item);

    KexiView* view = 0;

    if (viewMode == Kexi::DataViewMode) {
        view = new KexiReportView(parent);

    } else if (viewMode == Kexi::DesignViewMode) {
        view = new KexiReportDesignView(parent, d->sourceSelector);
        connect(d->sourceSelector, SIGNAL(setData(KoReportData*)), view, SLOT(slotSetData(KoReportData*)));
        connect(view, SIGNAL(itemInserted(QString)), this, SLOT(slotItemInserted(QString)));
    }
    return view;
}

void KexiReportPart::initPartActions()
{
    KexiMainWindowIface *win = KexiMainWindowIface::global();
    QList<QAction*> reportActions = KoReportDesigner::itemActions(&d->toolboxActionGroup);

    foreach(QAction* action, reportActions) {
        connect(action, SIGNAL(triggered(bool)), this, SLOT(slotToolboxActionTriggered(bool)));
        win->addToolBarAction("report", action);
        d->toolboxActionsByName.insert(action->objectName(), action);
    }

}

KDbObject* KexiReportPart::loadSchemaObject(
    KexiWindow *window, const KDbObject& object, Kexi::ViewMode viewMode,
    bool *ownedByWindow)
{
    QString layout;
    if (   !loadDataBlock(window, &layout, "layout") == true
        && !loadDataBlock(window, &layout, "pgzreport_layout") == true /* compat */)
    {
        return 0;
    }

    QDomDocument doc;
    if (!doc.setContent(layout)) {
        return 0;
    }
    qDebug() << doc.toString();

    KexiReportPart::TempData * temp = static_cast<KexiReportPart::TempData*>(window->data());
    const QDomElement root = doc.documentElement();
    temp->reportDefinition = root.firstChildElement("report:content");
    if (temp->reportDefinition.isNull()) {
        qWarning() << "no report report:content element found in report" << window->partItem()->name();
        return 0;
    }
    temp->connectionDefinition = root.firstChildElement("connection");
    if (temp->connectionDefinition.isNull()) {
        qWarning() << "no report report:content element found in report" << window->partItem()->name();
        return 0;
    }
    return KexiPart::Part::loadSchemaObject(window, object, viewMode, ownedByWindow);
}

KexiWindowData* KexiReportPart::createWindowData(KexiWindow* window)
{
    return new KexiReportPart::TempData(window);
}

KexiReportPart::TempData::TempData(QObject* parent)
        : KexiWindowData(parent)
        , reportSchemaChangedInPreviousView(true /*to force reloading on startup*/)
{
}

void KexiReportPart::setupCustomPropertyPanelTabs(QTabWidget *tab)
{
    if (!d->sourceSelector) {
        d->sourceSelector = new KexiSourceSelector(KexiMainWindowIface::global()->project(), tab);
    }
    tab->addTab(d->sourceSelector, koIcon("server-database"), QString());
    tab->setTabToolTip(tab->indexOf(d->sourceSelector), xi18n("Data Source"));
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

