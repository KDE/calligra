/* This file is part of the KDE project
   Copyright (C) 2004 Lucijan Busch <lucijan@kde.org>
   Copyright (C) 2004-2007 Jaroslaw Staniek <js@iidea.pl>

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

#ifndef KEXIQUERYDESIGNERGUIEDITOR_H
#define KEXIQUERYDESIGNERGUIEDITOR_H

#include <KexiView.h>
#include "kexiquerypart.h"

class QDragMoveEvent;
class QDropEvent;
class KexiRelationsView;
class KexiRelationsTableContainer;
class KexiRelationsConnection;

namespace KexiPart
{
  class Item;
}

namespace KoProperty {
  class Property;
  class Set;
}

namespace KexiDB
{
  class Connection;
  class QuerySchema;
  class TableSchema;
  class TableOrQuerySchema;
  class ResultInfo;
  class RecordData;
}

//! Design view of the Query Designer
class KexiQueryDesignerGuiEditor : public KexiView
{
  Q_OBJECT

  public:
    KexiQueryDesignerGuiEditor(QWidget *parent);
    virtual ~KexiQueryDesignerGuiEditor();

//		KexiDB::QuerySchema	*schema();

    KexiRelationsView *relationsView() const;

    virtual QSize sizeHint() const;

  public slots:
    virtual void setFocus();

  protected:
    void initTableColumns(); //!< Called just once.
    void initTableRows(); //!< Called to have all rows empty.
//unused		void addRow(const QString &tbl, const QString &field);
//		void			restore();
    virtual tristate beforeSwitchTo(Kexi::ViewMode mode, bool &dontStore);
    virtual tristate afterSwitchFrom(Kexi::ViewMode mode);

    virtual KexiDB::SchemaData* storeNewData(const KexiDB::SchemaData& sdata, bool &cancel);
    virtual tristate storeData(bool dontAsk = false);

    /*! Updates data in columns depending on tables that are currently inserted.
     Tabular Data in combo box popups is updated as well. */
    void updateColumnsData();

    /*! \return property buffer associated with currently selected row (i.e. field)
     or 0 if current row is empty. */
    virtual KoProperty::Set *propertySet();

    KoProperty::Set* createPropertySet( int row,
      const QString& tableName, const QString& fieldName, bool newOne = false );

    /*! Builds query schema out of information provided by gui.
     The schema is stored in temp->query member.
     \a errMsg is optional error message returned.
     \return true on proper schema creation. */
    bool buildSchema(QString *errMsg = 0);

    KexiQueryPart::TempData * tempData() const;

    /*! Helper: allocates and initializes new table view's row. Doesn't insert it, just returns. 
     \a tableName and \a fieldName shoudl be provided. 
     \a visible flag sets value for "Visible" column. */
    KexiDB::RecordData* createNewRow(const QString& tableName, const QString& fieldName,
      bool visible) const;

    KexiDB::BaseExpr* parseExpressionString(const QString& fullString, int& token,
      bool allowRelationalOperator);

    /*! @internal generates smallest unique alias */
    QByteArray generateUniqueAlias() const;

    void updatePropertiesVisibility(KoProperty::Set& buf);

  protected slots:
    void slotDragOverTableRow(KexiDB::RecordData *record, int row, QDragMoveEvent* e);
    void slotDroppedAtRow(KexiDB::RecordData *record, int row,
      QDropEvent *ev, KexiDB::RecordData*& newRecord);
    //! Reaction on appending a new item after deleting one
    void slotNewItemAppendedForAfterDeletingInSpreadSheetMode();
    void slotTableAdded(KexiDB::TableSchema &t);
    void slotTableHidden(KexiDB::TableSchema &t);

    //! Called before cell change in tableview.
    void slotBeforeCellChanged(KexiDB::RecordData* record, int colnum,
      QVariant& newValue, KexiDB::ResultInfo* result);

    void slotRowInserted(KexiDB::RecordData* record, uint row, bool repaint);
    void slotTablePositionChanged(KexiRelationsTableContainer*);
    void slotAboutConnectionRemove(KexiRelationsConnection*);
    void slotAppendFields(KexiDB::TableOrQuerySchema& tableOrQuery,const QStringList& fieldNames);

    /*! Loads layout of relation GUI diagram. */
    bool loadLayout();

    /*! Stores layout of relation GUI diagram. */
    bool storeLayout();

    void showTablesForQuery(KexiDB::QuerySchema *query);
    //! @internal
    void showFieldsOrRelationsForQueryInternal(
      KexiDB::QuerySchema *query, bool showFields, bool showRelations, KexiDB::ResultInfo& result);
    //! convenience method equal to showFieldsOrRelationsForQueryInternal(query, true, true)
    void showFieldsAndRelationsForQuery(KexiDB::QuerySchema *query, KexiDB::ResultInfo& result);
    //! convenience method equal to showFieldsOrRelationsForQueryInternal(query, true, false)
    void showFieldsForQuery(KexiDB::QuerySchema *query, KexiDB::ResultInfo& result);
    //! convenience method equal to showFieldsOrRelationsForQueryInternal(query, false, true)
    void showRelationsForQuery(KexiDB::QuerySchema *query, KexiDB::ResultInfo& result);

    void addConnection(KexiDB::Field *masterField, KexiDB::Field *detailsField);

    void slotPropertyChanged(KoProperty::Set& list, KoProperty::Property& property);

//		void slotObjectCreated(const QString &mime, const QString& name);
    void slotNewItemStored(KexiPart::Item&);
    void slotItemRemoved(const KexiPart::Item& item);
    void slotItemRenamed(const KexiPart::Item& item, const QString& oldName);

  private:
    class Private;
    Private *d;

    friend class KexiQueryView; // for storeNewData() and storeData() only
};

#endif
