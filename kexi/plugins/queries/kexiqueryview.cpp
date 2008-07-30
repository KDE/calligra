/* This file is part of the KDE project
   Copyright (C) 2004 Lucijan Busch <lucijan@kde.org>
   Copyright (C) 2004, 2006 Jaroslaw Staniek <js@iidea.pl>

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

#include <kexiproject.h>
#include <kexidb/connection.h>
#include <kexidb/parser/parser.h>
#include <kexidb/cursor.h>
#include <KexiMainWindowIface.h>
#include <kexiutils/utils.h>
#include <KexiWindow.h>

#include "kexiqueryview.h"
#include "kexiquerydesignersql.h"
#include "kexiquerydesignerguieditor.h"
#include "kexiquerypart.h"
#include <widget/tableview/kexitableview.h>
#include <widget/kexiqueryparameters.h>

//! @internal
class KexiQueryView::Private
{
  public:
    Private()
      : cursor(0)
//			, queryHasBeenChangedInViewMode( Kexi::NoViewMode )
    {}
    ~Private() {}
    KexiDB::Cursor *cursor;
    /*! Used in storeNewData(), storeData() to decide whether 
     we should ask other view to save changes.
     Stores information about view mode. */
//		int queryHasBeenChangedInViewMode;
};

//---------------------------------------------------------------------------------

KexiQueryView::KexiQueryView(QWidget *parent)
 : KexiDataTable(parent)
 , d( new Private() )
{
  tableView()->setInsertingEnabled(false); //default
}

KexiQueryView::~KexiQueryView()
{
  if (d->cursor)
    d->cursor->connection()->deleteCursor(d->cursor);
  delete d;
}

tristate KexiQueryView::executeQuery(KexiDB::QuerySchema *query)
{
  if (!query)
    return false;
  KexiUtils::WaitCursor wait;
  KexiDB::Cursor *oldCursor = d->cursor;
  KexiDB::debug( query->parameters() );
  bool ok;
  QList<QVariant> params;
  {
    KexiUtils::WaitCursorRemover remover;
    params = KexiQueryParameters::getParameters(this, 
      *KexiMainWindowIface::global()->project()->dbConnection()->driver(), *query, ok);
  }
  if (!ok) {//input cancelled
    return cancelled;
  }
  d->cursor = KexiMainWindowIface::global()->project()->dbConnection()->executeQuery(*query, params);
  if (!d->cursor) {
    window()->setStatus(
      KexiMainWindowIface::global()->project()->dbConnection(), 
      i18n("Query executing failed."));
//! @todo also provide server result and sql statement
    return false;
  }
  setData(d->cursor);

//! @todo remove close() when dynamic cursors arrive
  d->cursor->close();

  if (oldCursor)
    oldCursor->connection()->deleteCursor(oldCursor);

//! @todo maybe allow writing and inserting for single-table relations?
  tableView()->setReadOnly( true );
//! @todo maybe allow writing and inserting for single-table relations?
  //set data model itself read-only too
  tableView()->data()->setReadOnly( true );
  tableView()->setInsertingEnabled( false );
  return true;
}

tristate KexiQueryView::afterSwitchFrom(Kexi::ViewMode mode)
{
  if (mode==Kexi::NoViewMode) {
    KexiDB::QuerySchema *querySchema = static_cast<KexiDB::QuerySchema *>(window()->schemaData());
    const tristate result = executeQuery(querySchema);
    if (true != result)
      return result;
  }
  else if (mode==Kexi::DesignViewMode || Kexi::TextViewMode) {
    KexiQueryPart::TempData * temp = static_cast<KexiQueryPart::TempData*>(window()->data());

    //remember what view we should use to store data changes, if needed
//		if (temp->queryChangedInPreviousView)
//			d->queryHasBeenChangedInViewMode = mode;
//		else
//			d->queryHasBeenChangedInViewMode = Kexi::NoViewMode;

    const tristate result = executeQuery(temp->query());
    if (true != result)
      return result;
  }
  return true;
}

KexiDB::SchemaData* KexiQueryView::storeNewData(const KexiDB::SchemaData& sdata, bool &cancel)
{
  KexiView * view = window()->viewThatRecentlySetDirtyFlag();
  if (dynamic_cast<KexiQueryDesignerGuiEditor*>(view))
    return dynamic_cast<KexiQueryDesignerGuiEditor*>(view)->storeNewData(sdata, cancel);
  if (dynamic_cast<KexiQueryDesignerSQLView*>(view))
    return dynamic_cast<KexiQueryDesignerSQLView*>(view)->storeNewData(sdata, cancel);
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


#include "kexiqueryview.moc"
