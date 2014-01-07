/*
* Kexi QuickForm Plugin
* Copyright 2013  Adam Pigg <adam@piggz.co.uk>
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License as
* published by the Free Software Foundation; either version 2 of
* the License or (at your option) version 3 or any later version
* accepted by the membership of KDE e.V. (or its successor approved
* by the membership of KDE e.V.), which shall act as a proxy
* defined in Section 14 of version 3 of the license.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
*/

#include "quickformpart.h"

#include <kmainwindow.h>
#include <klocale.h>
#include <kdebug.h>
#include <KoIcon.h>
#include <core/KexiWindow.h>
#include <core/KexiMainWindowIface.h>
#include <ktabwidget.h>
#include <db/connection.h>

//#include "kexireportview.h"
#include "quickformdesignview.h"
#include "quickformdataview.h"

//! @internal
class QuickFormPart::Private
{
public:
Private() : toolboxActionGroup(0)
{
    //!TODO ksrc = 0;
}
~Private() {
}
//!TODO KexiSourceSelector *ksrc;
QActionGroup toolboxActionGroup;
QMap<QString, QAction*> toolboxActionsByName;
};

QuickFormPart::QuickFormPart(QObject *parent, const QVariantList &l)
: KexiPart::Part(parent,
                i18nc("Translate this word using only lowercase alphanumeric characters (a..z, 0..9). "
                "Use '_' character instead of spaces. First character should be a..z character. "
                "If you cannot use latin characters in your language, use english word.",
                "quick form"),
                i18nc("tooltip", "Create new quick form"),
                i18nc("what's this", "Creates new quick form."),
                l)
, d(new Private)
{
setInternalPropertyValue("newObjectsAreDirty", true);
}

QuickFormPart::~QuickFormPart()
{
delete d;
}

KLocalizedString QuickFormPart::i18nMessage(
const QString& englishMessage, KexiWindow* window) const
{
    Q_UNUSED(window);
    if (englishMessage == "Design of object \"%1\" has been modified.")
        return ki18n(I18N_NOOP("Design of report \"%1\" has been modified."));
    if (englishMessage == "Object \"%1\" already exists.")
        return ki18n(I18N_NOOP("Report \"%1\" already exists."));
    
    return Part::i18nMessage(englishMessage, window);
}

KexiView* QuickFormPart::createView(QWidget *parent, KexiWindow* window,
                                        KexiPart::Item &item, Kexi::ViewMode viewMode, QMap<QString, QVariant>*)
{
    Q_UNUSED(window);
    Q_UNUSED(item);
    
    KexiView* view = 0;
 
    if (viewMode == Kexi::DataViewMode) {
        QuickFormDataView *dview = new QuickFormDataView(parent);
        dview->setDefinition(loadQuickForm(item.name()));
        view = dview;
    } else if (viewMode == Kexi::DesignViewMode) {
        view = new QuickFormDesignView(parent);
    }
    return view;
}

void QuickFormPart::initPartActions()
{
#if 0
    KexiMainWindowIface *win = KexiMainWindowIface::global();
    QList<QAction*> reportActions = KoReportDesigner::actions(&d->toolboxActionGroup);
    
    foreach(QAction* action, reportActions) {
        connect(action, SIGNAL(triggered(bool)), this, SLOT(slotToolboxActionTriggered(bool)));
        win->addToolBarAction("report", action);
        d->toolboxActionsByName.insert(action->objectName(), action);
    }
#endif  
}

QString QuickFormPart::loadQuickForm(const QString& name)
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
    
    if (conn->loadObjectSchemaData(win->project()->idForClass("org.kexi-project.quickform"), name, sd) != true)
    {
        kWarning() << "failed to load schema data";
        return QString();
    }
    
    kDebug() << "***Object ID:" << sd.id();
    
    if ( win->project()->dbConnection()->loadDataBlock(sd.id(), src) == true)
    {
     kDebug() << src;
        return src;
    }
    
    kWarning() << "Unable to load document";
    return QString();
}

KexiWindowData* QuickFormPart::createWindowData(KexiWindow* window)
{
    kDebug();
    const QString document(loadQuickForm(window->partItem()->name()));
    QuickFormPart::TempData *td = new QuickFormPart::TempData(window);
    
    QDomDocument doc;
    doc.setContent(document);
    
    kDebug() << doc.toString();
    
    QDomElement root = doc.documentElement();
    QDomElement qf = root.firstChildElement("quickform:content");
    QDomElement conn = root.firstChildElement("connection");
    
    td->quickFormDefinition = qf;
    td->connectionDefinition = conn;
    
    td->name = window->partItem()->name();
    return td;
}

QuickFormPart::TempData::TempData(QObject* parent)
: KexiWindowData(parent)
, quickformSchemaChangedInPreviousView(true /*to force reloading on startup*/)
{
}

void QuickFormPart::setupCustomPropertyPanelTabs(KTabWidget *tab)
{
#if 0
    if (!d->ksrc)
        d->ksrc = new KexiSourceSelector(tab, KexiMainWindowIface::global()->project()->dbConnection());
    tab->addTab(d->ksrc, koIcon("server-database"), QString());
    tab->setTabToolTip(tab->indexOf(d->ksrc), i18n("Data Source"));
#endif
}

void QuickFormPart::slotToolboxActionTriggered(bool checked)
{
#if 0
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
#endif
}

void QuickFormPart::slotItemInserted(const QString& entity)
{
    Q_UNUSED(entity);
    // uncheck toolbox action after it is used
    QAction * a = d->toolboxActionGroup.checkedAction();
    if (a) {
        a->setChecked(false);
    }
}
                
            