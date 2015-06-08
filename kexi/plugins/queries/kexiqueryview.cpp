/* This file is part of the KDE project
   Copyright (C) 2004 Lucijan Busch <lucijan@kde.org>
   Copyright (C) 2004, 2006 Jarosław Staniek <staniek@kde.org>

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

#include "kexiqueryview.h"
#include "kexiquerydesignersql.h"
#include "kexiquerydesignerguieditor.h"
#include "kexiquerypart.h"
#include <widget/tableview/KexiTableScrollArea.h>
#include <widget/kexiqueryparameters.h>
#include <kexiproject.h>
#include <KexiMainWindowIface.h>
#include <kexiutils/utils.h>
#include <KexiWindow.h>

#include <KDbConnection>
#include <KDbParser>
#include <KDbCursor>

//! @internal
class KexiQueryView::Private
{
public:
    Private()
            : cursor(0),
              currentParams()
    {}
    ~Private() {}
    KDbCursor *cursor;
    QList<QVariant> currentParams;
    /*! Used in storeNewData(), storeData() to decide whether
     we should ask other view to save changes.
     Stores information about view mode. */
};

//---------------------------------------------------------------------------------

KexiQueryView::KexiQueryView(QWidget *parent)
        : KexiDataTableView(parent)
        , d(new Private())
{
    // setup main menu actions
    QList<QAction*> mainMenuActions;
    mainMenuActions
            << sharedAction("project_export_data_table");
    setMainMenuActions(mainMenuActions);

    tableView()->setInsertingEnabled(false); //default
}

KexiQueryView::~KexiQueryView()
{
    if (d->cursor)
        d->cursor->connection()->deleteCursor(d->cursor);
    delete d;
}

tristate KexiQueryView::executeQuery(KDbQuerySchema *query)
{
    if (!query)
        return false;
    KexiUtils::WaitCursor wait;
    KDbCursor *oldCursor = d->cursor;
    qDebug() << query->parameters();
    bool ok;
    KDbConnection * conn = KexiMainWindowIface::global()->project()->dbConnection();
    {
        KexiUtils::WaitCursorRemover remover;
        d->currentParams = KexiQueryParameters::getParameters(this,
                 *conn->driver(), *query, ok);
    }
    if (!ok) {//input cancelled
        return cancelled;
    }
    d->cursor = conn->executeQuery(*query, d->currentParams);
    if (!d->cursor) {
        window()->setStatus(
            conn,
            xi18n("Query executing failed."));
//! @todo also provide server result and sql statement
        return false;
    }
    setData(d->cursor);

//! @todo remove close() when dynamic cursors arrive
    d->cursor->close();

    if (oldCursor)
        oldCursor->connection()->deleteCursor(oldCursor);

//! @todo maybe allow writing and inserting for single-table relations?
    tableView()->setReadOnly(true);
//! @todo maybe allow writing and inserting for single-table relations?
    //set data model itself read-only too
    tableView()->data()->setReadOnly(true);
    tableView()->setInsertingEnabled(false);
    return true;
}

tristate KexiQueryView::afterSwitchFrom(Kexi::ViewMode mode)
{
    if (mode == Kexi::NoViewMode) {
        KDbQuerySchema *querySchema = static_cast<KDbQuerySchema *>(window()->schemaData());
        const tristate result = executeQuery(querySchema);
        if (true != result)
            return result;
    } else if (mode == Kexi::DesignViewMode || Kexi::TextViewMode) {
        KexiQueryPart::TempData * temp = static_cast<KexiQueryPart::TempData*>(window()->data());
        const tristate result = executeQuery(temp->query());
        if (true != result)
            return result;
    }
    return true;
}

KDbObject* KexiQueryView::storeNewData(const KDbObject& sdata,
                                                KexiView::StoreNewDataOptions options,
                                                bool &cancel)
{
    KexiView * view = window()->viewThatRecentlySetDirtyFlag();
    if (dynamic_cast<KexiQueryDesignerGuiEditor*>(view))
        return dynamic_cast<KexiQueryDesignerGuiEditor*>(view)->storeNewData(sdata, options, cancel);
    if (dynamic_cast<KexiQueryDesignerSQLView*>(view))
        return dynamic_cast<KexiQueryDesignerSQLView*>(view)->storeNewData(sdata, options, cancel);
    return 0;
}

tristate KexiQueryView::storeData(bool dontAsk)
{
    KexiView * view = window()->viewThatRecentlySetDirtyFlag();
    if (dynamic_cast<KexiQueryDesignerGuiEditor*>(view))
        return dynamic_cast<KexiQueryDesignerGuiEditor*>(view)->storeData(dontAsk);
    if (dynamic_cast<KexiQueryDesignerSQLView*>(view))
        return dynamic_cast<KexiQueryDesignerSQLView*>(view)->storeData(dontAsk);
    return false;
}

QList<QVariant> KexiQueryView::currentParameters() const
{
    return d->currentParams;
}

