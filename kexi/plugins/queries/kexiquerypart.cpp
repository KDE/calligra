/* This file is part of the KDE project
   Copyright (C) 2004 Lucijan Busch <lucijan@kde.org>
   Copyright (C) 2004,2006 Jarosław Staniek <staniek@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "kexiquerypart.h"

#include <KDebug>
#include <KGenericFactory>
#include <KToggleAction>

#include <KexiMainWindowIface.h>
#include <KexiWindow.h>
#include <kexiproject.h>
#include <kexipartinfo.h>

#include <kexidb/cursor.h>
#include <kexidb/parser/parser.h>

#include "kexiqueryview.h"
#include "kexiquerydesignerguieditor.h"
#include "kexiquerydesignersql.h"

//------------------------------------------------

KexiQueryPart::KexiQueryPart(QObject *parent, const QStringList &l)
        : KexiPart::Part(parent, l)
{
    setInternalPropertyValue("instanceName",
                             i18nc("Translate this word using only lowercase alphanumeric characters (a..z, 0..9). "
                                   "Use '_' character instead of spaces. First character should be a..z character. "
                                   "If you cannot use latin characters in your language, use english word.",
                                   "query"));
    setInternalPropertyValue("instanceCaption", i18n("Query"));
    setInternalPropertyValue("instanceToolTip", i18nc("tooltip", "Create new query"));
    setInternalPropertyValue("instanceWhatsThis", i18nc("what's this", "Creates new query."));
// setInternalPropertyValue("textViewModeCaption", i18n("&SQL View"));
    setInternalPropertyValue("textViewModeCaption", i18n("SQL"));
    setSupportedViewModes(Kexi::DataViewMode | Kexi::DesignViewMode | Kexi::TextViewMode);
}

KexiQueryPart::~KexiQueryPart()
{
}

KexiWindowData* KexiQueryPart::createWindowData(KexiWindow* window)
{
    KexiQueryPart::TempData *data = new KexiQueryPart::TempData(
        window, KexiMainWindowIface::global()->project()->dbConnection());
    data->listenerInfoString = window->part()->instanceCaption() + " \""
                               + window->partItem()->name() + "\"";
    return data;
}

KexiView* KexiQueryPart::createView(QWidget *parent, KexiWindow* window, KexiPart::Item &item,
                                    Kexi::ViewMode viewMode, QMap<QString, QVariant>*)
{
    Q_UNUSED(item);
    Q_UNUSED(window);

    kDebug() << "KexiQueryPart::createView()";

    KexiView* view = 0;
    if (viewMode == Kexi::DataViewMode) {
        view = new KexiQueryView(parent);
        view->setObjectName("dataview");
    } else if (viewMode == Kexi::DesignViewMode) {
        view = new KexiQueryDesignerGuiEditor(parent);
        view->setObjectName("guieditor");
        //needed for updating tables combo box:
        KexiProject *prj = KexiMainWindowIface::global()->project();
        connect(prj, SIGNAL(newItemStored(KexiPart::Item&)),
                view, SLOT(slotNewItemStored(KexiPart::Item&)));
        connect(prj, SIGNAL(itemRemoved(const KexiPart::Item&)),
                view, SLOT(slotItemRemoved(const KexiPart::Item&)));
        connect(prj, SIGNAL(itemRenamed(const KexiPart::Item&, const QString&)),
                view, SLOT(slotItemRenamed(const KexiPart::Item&, const QString&)));

//  connect(KexiMainWindowIface::global()->project(), SIGNAL(tableCreated(KexiDB::TableSchema&)),
//   view, SLOT(slotTableCreated(KexiDB::TableSchema&)));
    } else if (viewMode == Kexi::TextViewMode) {
        view = new KexiQueryDesignerSQLView(parent);
        view->setObjectName("sqldesigner");
    }
    return view;
}

bool KexiQueryPart::remove(KexiPart::Item &item)
{
    if (!KexiMainWindowIface::global()->project()
            || !KexiMainWindowIface::global()->project()->dbConnection())
        return false;
    KexiDB::Connection *conn = KexiMainWindowIface::global()->project()->dbConnection();
    KexiDB::QuerySchema *sch = conn->querySchema(item.identifier());
    if (sch)
        return conn->dropQuery(sch);
    //last chance: just remove item
    return conn->removeObject(item.identifier());
}

#if 0
KexiPart::DataSource *
KexiQueryPart::dataSource()
{
    return new KexiQueryDataSource(this);
}

void KexiQueryPart::initPartActions(KActionCollection *col)
{
}

void KexiQueryPart::initInstanceActions(int mode, KActionCollection *col)
{
    if (mode == Kexi::DataViewMode) {
    } else if (mode == Kexi::DesignViewMode) {
    } else if (mode == Kexi::TextViewMode) {
//  new KAction(i18n("Check Query"), "test_it", 0, this, SLOT(slotCheckQuery()), col, "querypart_check_query");

//TODO  new KAction(i18n("Execute Query"), "?????", 0, this, SLOT(checkQuery()), col, "querypart_execute_query");
    }
}
#endif

void KexiQueryPart::initPartActions()
{
}

void KexiQueryPart::initInstanceActions()
{
// new KAction(i18n("Check Query"), "test_it", 0, this, SLOT(slotCheckQuery()),
//  m_instanceGuiClients[Kexi::DesignViewMode]->actionCollection(), "querypart_check_query");

    /*2.0: moved to KexiView::createViewActions()
      KAction *a = createSharedAction(Kexi::TextViewMode, i18n("Check Query"), "test_it",
        KShortcut(Qt::Key_F9), "querypart_check_query");
      a->setToolTip(i18n("Check Query"));
      a->setWhatsThis(i18n("Checks query for validity."));

      a = createSharedToggleAction(
    //! @todo other icon
        Kexi::TextViewMode, i18n("Show SQL History"), "view_top_bottom",
        KShortcut(), "querypart_view_toggle_history");
      a->setWhatsThis(i18n("Shows or hides SQL editor's history."));
    */
// setActionAvailable("querypart_check_query", true);
}

KexiDB::SchemaData* KexiQueryPart::loadSchemaData(
    KexiWindow *window, const KexiDB::SchemaData& sdata, Kexi::ViewMode viewMode)
{
    KexiQueryPart::TempData * temp = static_cast<KexiQueryPart::TempData*>(window->data());
    QString sqlText;
    if (!loadDataBlock(window, sqlText, "sql")) {
        return 0;
    }
    KexiDB::Parser *parser = KexiMainWindowIface::global()->project()->sqlParser();
    parser->parse(sqlText);
    KexiDB::QuerySchema *query = parser->query();
    //error?
    if (!query) {
        if (viewMode == Kexi::TextViewMode) {
            //for SQL view, no parsing is initially needed:
            //-just make a copy:
            return KexiPart::Part::loadSchemaData(window, sdata, viewMode);
        }
        /* Set this to true on data loading loadSchemaData() to indicate that TextView mode
         could be used instead of DataView or DesignView, because there are problems
         with opening object. */
        temp->proposeOpeningInTextViewModeBecauseOfProblems = true;
        //todo
        return 0;
    }
    query->debug();
    (KexiDB::SchemaData&)*query = sdata; //copy main attributes

    temp->registerTableSchemaChanges(query);

    query->debug();
    return query;
}

KLocalizedString KexiQueryPart::i18nMessage(const QString& englishMessage, KexiWindow* window) const
{
    if (englishMessage == "Design of object \"%1\" has been modified.")
        return ki18n(I18N_NOOP("Design of query \"%1\" has been modified."));
    if (englishMessage == "Object \"%1\" already exists.")
        return ki18n(I18N_NOOP("Query \"%1\" already exists."));

    return Part::i18nMessage(englishMessage, window);
}

tristate KexiQueryPart::rename(KexiPart::Item &item, const QString& newName)
{
    Q_UNUSED(newName);
    if (!KexiMainWindowIface::global()->project()->dbConnection())
        return false;
    KexiMainWindowIface::global()->project()->dbConnection()
    ->setQuerySchemaObsolete(item.name());
    return true;
}

//----------------

KexiQueryPart::TempData::TempData(KexiWindow* window, KexiDB::Connection *conn)
        : KexiWindowData(window)
        , KexiDB::Connection::TableSchemaChangeListenerInterface()
        , queryChangedInPreviousView(false)
        , m_query(0)
{
    this->conn = conn;
}

KexiQueryPart::TempData::~TempData()
{
    conn->unregisterForTablesSchemaChanges(*this);
}

void KexiQueryPart::TempData::clearQuery()
{
    if (!m_query)
        return;
    unregisterForTablesSchemaChanges();
    m_query->clear();
}

void KexiQueryPart::TempData::unregisterForTablesSchemaChanges()
{
    conn->unregisterForTablesSchemaChanges(*this);
}

void KexiQueryPart::TempData::registerTableSchemaChanges(KexiDB::QuerySchema *q)
{
    if (!q)
        return;
    foreach(KexiDB::TableSchema* table, *q->tables()) {
        conn->registerForTableSchemaChanges(*this, *table);
    }
}

tristate KexiQueryPart::TempData::closeListener()
{
    KexiWindow* window = static_cast<KexiWindow*>(parent());
    return KexiMainWindowIface::global()->closeWindow(window);
}

KexiDB::QuerySchema *KexiQueryPart::TempData::takeQuery()
{
    KexiDB::QuerySchema *query = m_query;
    m_query = 0;
    return query;
}

void KexiQueryPart::TempData::setQuery(KexiDB::QuerySchema *query)
{
    if (m_query && m_query == query)
        return;
    if (m_query
            /* query not owned by window */
            && (static_cast<KexiWindow*>(parent())->schemaData() != static_cast<KexiDB::SchemaData*>(m_query))) {
        delete m_query;
    }
    m_query = query;
}

//----------------

#if 0
KexiQueryDataSource::KexiQueryDataSource(KexiPart::Part *part)
        : KexiPart::DataSource(part)
{
}

KexiQueryDataSource::~KexiQueryDataSource()
{
}

KexiDB::FieldList *
KexiQueryDataSource::fields(KexiProject *, const KexiPart::Item &)
{
    return 0;
}

KexiDB::Cursor *
KexiQueryDataSource::cursor(KexiProject *, const KexiPart::Item &, bool)
{
    return 0;
}
#endif

//----------------

K_EXPORT_COMPONENT_FACTORY(kexihandler_query, KGenericFactory<KexiQueryPart>("kexihandler_query"))

#include "kexiquerypart.moc"
