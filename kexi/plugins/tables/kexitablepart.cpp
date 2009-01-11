/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@kde.org>
   Copyright (C) 2002, 2003 Joseph Wenninger <jowenn@kde.org>
   Copyright (C) 2004 Jarosław Staniek <staniek@kde.org>

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
 * Boston, MA 02110-1301, USA.
*/

#include "kexitablepart.h"

#include <kdebug.h>
#include <kgenericfactory.h>
#include <kmessagebox.h>
#include <ktabwidget.h>
#include <kiconloader.h>

#include <KexiMainWindowIface.h>
#include "kexiproject.h"
#include "kexipartinfo.h"
#include "widget/kexidatatable.h"
#include "widget/tableview/kexidatatableview.h"
#include "kexitabledesignerview.h"
#include "kexitabledesigner_dataview.h"
#include "kexilookupcolumnpage.h"

#include <kexidb/connection.h>
#include <kexidb/cursor.h>
#include <KexiWindow.h>
#include <Q3CString>

//! @internal
class KexiTablePart::Private
{
public:
    Private() {
    }
    ~Private() {
        delete static_cast<KexiLookupColumnPage*>(lookupColumnPage);
    }
    QPointer<KexiLookupColumnPage> lookupColumnPage;
};

KexiTablePart::KexiTablePart(QObject *parent, const QStringList &l)
        : KexiPart::Part(parent, l)
        , d(new Private())
{
    kDebug();
    setInternalPropertyValue("instanceName",
                             i18nc("Translate this word using only lowercase alphanumeric characters (a..z, 0..9). "
                                   "Use '_' character instead of spaces. First character should be a..z character. "
                                   "If you cannot use latin characters in your language, use english word.",
                                   "table"));
    setInternalPropertyValue("instanceCaption", i18n("Table"));
    setInternalPropertyValue("instanceToolTip", i18nc("tooltip", "Create new table"));
    setInternalPropertyValue("instanceWhatsThis", i18nc("what's this", "Creates new table."));
    setSupportedViewModes(Kexi::DataViewMode | Kexi::DesignViewMode);
//! @todo js: also add Kexi::TextViewMode when we'll have SQL ALTER TABLE EDITOR!!!
}

KexiTablePart::~KexiTablePart()
{
    delete d;
}

void KexiTablePart::initPartActions()
{
}

void KexiTablePart::initInstanceActions()
{
//moved to main window createSharedAction(Kexi::DataViewMode, i18n("Filter"), "view-filter", 0, "tablepart_filter");
    /*2.0 moved to createViewActions()
      KAction *a = createSharedToggleAction(
        Kexi::DesignViewMode, i18n("Primary Key"), "key", KShortcut(),
        "tablepart_toggle_pkey");
    //  Kexi::DesignViewMode, i18n("Toggle Primary Key"), "key", KShortcut(), "tablepart_toggle_pkey");
      a->setWhatsThis(i18n("Sets or removes primary key for currently selected field.")); */
}

KexiWindowData* KexiTablePart::createWindowData(KexiWindow* window)
{
    return new KexiTablePart::TempData(window);
}

KexiView* KexiTablePart::createView(QWidget *parent, KexiWindow* window,
                                    KexiPart::Item &item, Kexi::ViewMode viewMode, QMap<QString, QVariant>*)
{
    KexiMainWindowIface *win = KexiMainWindowIface::global();
    if (!win || !win->project() || !win->project()->dbConnection())
        return 0;


    KexiTablePart::TempData *temp
    = static_cast<KexiTablePart::TempData*>(window->data());
    if (!temp->table) {
        temp->table = win->project()->dbConnection()->tableSchema(item.name());
        kDebug() << "schema is " << temp->table;
    }

    if (viewMode == Kexi::DesignViewMode) {
        KexiTableDesignerView *t = new KexiTableDesignerView(parent);
        return t;
    } else if (viewMode == Kexi::DataViewMode) {
        if (!temp->table)
            return 0; //todo: message
        //we're not setting table schema here -it will be forced to set
        // in KexiTableDesigner_DataView::afterSwitchFrom()
        KexiTableDesigner_DataView *t = new KexiTableDesigner_DataView(parent);
        return t;
    }
    return 0;
}

bool KexiTablePart::remove(KexiPart::Item &item)
{
    KexiProject *project = KexiMainWindowIface::global()->project();
    if (!project || !project->dbConnection())
        return false;

    KexiDB::Connection *conn = project->dbConnection();
    KexiDB::TableSchema *sch = conn->tableSchema(item.identifier());

    if (sch) {
        tristate res = KexiTablePart::askForClosingObjectsUsingTableSchema(
                           KexiMainWindowIface::global()->thisWidget(), *conn, *sch,
                           i18n(
                               "You are about to remove table \"%1\" but following objects using this table are opened:",
                               sch->name()));
        return true == conn->dropTable(sch);
    }
    //last chance: just remove item
    return conn->removeObject(item.identifier());
}

tristate KexiTablePart::rename(KexiPart::Item & item,
                               const QString& newName)
{
//TODO: what about objects (queries/forms) that use old name?
    KexiDB::Connection *conn = KexiMainWindowIface::global()->project()->dbConnection();
    KexiDB::TableSchema *sch = conn->tableSchema(item.identifier());
    if (!sch)
        return false;
    return conn->alterTableName(*sch, newName);
}

KexiDB::SchemaData*
KexiTablePart::loadSchemaData(KexiWindow *window, const KexiDB::SchemaData& sdata,
                              Kexi::ViewMode viewMode)
{
    Q_UNUSED(viewMode);
    return KexiMainWindowIface::global()->project()->dbConnection()->tableSchema(sdata.name());
}

#if 0
KexiPart::DataSource *
KexiTablePart::dataSource()
{
    return new KexiTableDataSource(this);
}
#endif

tristate KexiTablePart::askForClosingObjectsUsingTableSchema(
    QWidget *parent, KexiDB::Connection& conn,
    KexiDB::TableSchema& table, const QString& msg)
{
    QSet<KexiDB::Connection::TableSchemaChangeListenerInterface*>* listeners
    = conn.tableSchemaChangeListeners(table);
    if (!listeners || listeners->isEmpty())
        return true;

    QString openedObjectsStr = "<ul>";
    foreach(KexiDB::Connection::TableSchemaChangeListenerInterface* iface, *listeners) {
        openedObjectsStr += QString("<li>%1</li>").arg(iface->listenerInfoString);
    }
    openedObjectsStr += "</ul>";
    int r = KMessageBox::questionYesNo(parent,
                                       "<p>" + msg + "</p><p>" + openedObjectsStr + "</p><p>"
                                       + i18n("Do you want to close all windows for these objects?"),
                                       QString(), KGuiItem(i18n("Close windows"), "window-close"), KStandardGuiItem::cancel());
    tristate res;
    if (r == KMessageBox::Yes) {
        //try to close every window
        res = conn.closeAllTableSchemaChangeListeners(table);
        if (res != true) //do not expose closing errors twice; just cancel
            res = cancelled;
    } else
        res = cancelled;

    return res;
}

KLocalizedString KexiTablePart::i18nMessage(
    const QString& englishMessage, KexiWindow* window) const
{
    Q_UNUSED(window);
    if (englishMessage == "Design of object \"%1\" has been modified.")
        return ki18n(I18N_NOOP("Design of table \"%1\" has been modified."));

    if (englishMessage == "Object \"%1\" already exists.")
        return ki18n(I18N_NOOP("Table \"%1\" already exists."));

    if (window->currentViewMode() == Kexi::DesignViewMode && !window->neverSaved()
            && englishMessage == ":additional message before saving design")
        return ki18n(I18N_NOOP("Warning! Any data in this table will be removed upon design's saving!"));

    return Part::i18nMessage(englishMessage, window);
}

void KexiTablePart::setupCustomPropertyPanelTabs(KTabWidget *tab)
{
    if (!d->lookupColumnPage) {
        d->lookupColumnPage = new KexiLookupColumnPage(0);
        connect(d->lookupColumnPage,
                SIGNAL(jumpToObjectRequested(const QString&, const QString&)),
                KexiMainWindowIface::global()->thisWidget(),
                SLOT(highlightObject(const QString&, const QString&)));

//! @todo add "Table" tab

        /*
          connect(d->dataSourcePage, SIGNAL(formDataSourceChanged(const QCString&, const QCString&)),
            KFormDesigner::FormManager::self(), SLOT(setFormDataSource(const QCString&, const QCString&)));
          connect(d->dataSourcePage, SIGNAL(dataSourceFieldOrExpressionChanged(const QString&, const QString&, KexiDB::Field::Type)),
            KFormDesigner::FormManager::self(), SLOT(setDataSourceFieldOrExpression(const QString&, const QString&, KexiDB::Field::Type)));
          connect(d->dataSourcePage, SIGNAL(insertAutoFields(const QString&, const QString&, const QStringList&)),
            KFormDesigner::FormManager::self(), SLOT(insertAutoFields(const QString&, const QString&, const QStringList&)));*/
    }

    KexiProject *prj = KexiMainWindowIface::global()->project();
    d->lookupColumnPage->setProject(prj);

//! @todo add lookup field icon
    tab->addTab(d->lookupColumnPage, KIcon("combo"), "");
    tab->setTabToolTip(d->lookupColumnPage, i18n("Lookup column"));
}

KexiLookupColumnPage* KexiTablePart::lookupColumnPage() const
{
    return d->lookupColumnPage;
}

//----------------

#if 0
KexiTableDataSource::KexiTableDataSource(KexiPart::Part *part)
        : KexiPart::DataSource(part)
{
}

KexiTableDataSource::~KexiTableDataSource()
{
}

KexiDB::FieldList *
KexiTableDataSource::fields(KexiProject *project, const KexiPart::Item &it)
{
    kDebug() << it.name();
    return project->dbConnection()->tableSchema(it.name());
}

KexiDB::Cursor *
KexiTableDataSource::cursor(KexiProject * /*project*/,
                            const KexiPart::Item &/*it*/, bool /*buffer*/)
{
    return 0;
}
#endif

//----------------

KexiTablePart::TempData::TempData(QObject* parent)
        : KexiWindowData(parent)
        , table(0)
        , tableSchemaChangedInPreviousView(true /*to force reloading on startup*/)
{
}

//----------------

/**
TODO
*/
/*
AboutData( const char *programName,
  const char *version,
  const char *i18nShortDescription = 0,
  int licenseType = License_Unknown,
  const char *i18nCopyrightStatement = 0,
  const char *i18nText = 0,
  const char *homePageAddress = 0,
  const char *bugsEmailAddress = "submit@bugs.kde.org"
);

#define KEXIPART_EXPORT_FACTORY( libname, partClass, aboutData ) \
  static KexiPart::AboutData * libname ## updateAD(KexiPart::AboutData *ad) \
  { ad->setAppName( #libname ); return ad; } \
  K_EXPORT_COMPONENT_FACTORY( libname, KGenericFactory<partClass>(libname ## updateAD(#libname)) )
*/

K_EXPORT_COMPONENT_FACTORY(kexihandler_table, KGenericFactory<KexiTablePart>("kexihandler_table"))

#include "kexitablepart.moc"

