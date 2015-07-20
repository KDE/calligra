/* This file is part of the KDE project
   Copyright (C) 2004 Lucijan Busch <lucijan@kde.org>
   Copyright (C) 2004-2015 Jarosław Staniek <staniek@kde.org>

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
class KProperty;
class KPropertySet;
class KDbConnection;
class KDbQuerySchema;
class KDbTableSchema;
class KDbTableOrQuerySchema;
class KDbResultInfo;
class KDbRecordData;
class KexiRelationsView;
class KexiRelationsTableContainer;
class KexiRelationsConnection;
namespace KexiPart
{
class Item;
}

//! Design view of the Query Designer
class KexiQueryDesignerGuiEditor : public KexiView
{
    Q_OBJECT

public:
    explicit KexiQueryDesignerGuiEditor(QWidget *parent);
    virtual ~KexiQueryDesignerGuiEditor();

    KexiRelationsView *relationsView() const;

    virtual QSize sizeHint() const;

public Q_SLOTS:
    virtual void setFocus();

protected:
    void initTableColumns(); //!< Called just once.
    void initTableRows(); //!< Called to have all rows empty.
    virtual tristate beforeSwitchTo(Kexi::ViewMode mode, bool *dontStore);
    virtual tristate afterSwitchFrom(Kexi::ViewMode mode);

    virtual KDbObject* storeNewData(const KDbObject& object,
                                             KexiView::StoreNewDataOptions options,
                                             bool *cancel);
    virtual tristate storeData(bool dontAsk = false);

    /*! Updates data in columns depending on tables that are currently inserted.
     Tabular Data in combo box popups is updated as well. */
    void updateColumnsData();

    /*! \return property buffer associated with currently selected row (i.e. field)
     or 0 if current row is empty. */
    virtual KPropertySet *propertySet();

    KPropertySet* createPropertySet(int row,
                                       const QString& tableName, const QString& fieldName, bool newOne = false);

    /*! Builds query schema out of information provided by gui.
     The schema is stored in temp->query member.
     \a errMsg is optional error message returned.
     \return true on proper schema creation. */
    bool buildSchema(QString *errMsg = 0);

    KexiQueryPart::TempData * tempData() const;

    /*! Helper: allocates and initializes new table view's row. Doesn't insert it, just returns.
     \a tableName and \a fieldName should be provided.
     \a visible flag sets value for "Visible" column. */
    KDbRecordData* createNewRow(const QString& tableName, const QString& fieldName,
                                     bool visible) const;

    KDbExpression parseExpressionString(const QString& fullString, KDbToken *token,
                                         bool allowRelationalOperator);

    /*! @internal generates smallest unique alias */
    QByteArray generateUniqueAlias() const;

    void updatePropertiesVisibility(KPropertySet& buf);

protected Q_SLOTS:
    void slotDragOverTableRecord(KDbRecordData *data, int record, QDragMoveEvent* e);
    void slotDroppedAtRecord(KDbRecordData *data, int record,
                          QDropEvent *ev, KDbRecordData*& newRecord);
    //! Reaction on appending a new item after deleting one
    void slotNewItemAppendedForAfterDeletingInSpreadSheetMode();
    void slotTableAdded(KDbTableSchema* t);
    void slotTableHidden(KDbTableSchema* t);

    //! Called before cell change in tableview.
    void slotBeforeCellChanged(KDbRecordData* data, int colnum,
                               QVariant* newValue, KDbResultInfo* result);

    void slotRecordInserted(KDbRecordData* data, int record, bool repaint);
    void slotTablePositionChanged(KexiRelationsTableContainer*);
    void slotAboutConnectionRemove(KexiRelationsConnection*);
    void slotAppendFields(KDbTableOrQuerySchema& tableOrQuery, const QStringList& fieldNames);

    /*! Loads layout of relation GUI diagram. */
    bool loadLayout();

    /*! Stores layout of relation GUI diagram. */
    bool storeLayout();

    void showTablesForQuery(KDbQuerySchema *query);
    //! @internal
    void showFieldsOrRelationsForQueryInternal(
        KDbQuerySchema *query, bool showFields, bool showRelations, KDbResultInfo& result);
    //! convenience method equal to showFieldsOrRelationsForQueryInternal(query, true, true)
    void showFieldsAndRelationsForQuery(KDbQuerySchema *query, KDbResultInfo& result);
    //! convenience method equal to showFieldsOrRelationsForQueryInternal(query, true, false)
    void showFieldsForQuery(KDbQuerySchema *query, KDbResultInfo& result);
    //! convenience method equal to showFieldsOrRelationsForQueryInternal(query, false, true)
    void showRelationsForQuery(KDbQuerySchema *query, KDbResultInfo& result);

    void addConnection(KDbField *masterField, KDbField *detailsField);

    void slotPropertyChanged(KPropertySet& set, KProperty& property);

    void slotNewItemStored(KexiPart::Item* item);
    void slotItemRemoved(const KexiPart::Item& item);
    void slotItemRenamed(const KexiPart::Item& item, const QString& oldName);

private:
    void slotBeforeColumnCellChanged(KDbRecordData *data,
        QVariant& newValue, KDbResultInfo* result);

    void slotBeforeTableCellChanged(KDbRecordData *data,
        QVariant& newValue, KDbResultInfo* result);

    void slotBeforeVisibleCellChanged(KDbRecordData *data,
        QVariant& newValue, KDbResultInfo* result);

    void slotBeforeTotalsCellChanged(KDbRecordData *data,
        QVariant& newValue, KDbResultInfo* result);

    void slotBeforeSortingCellChanged(KDbRecordData *data,
        QVariant& newValue, KDbResultInfo* result);

    void slotBeforeCriteriaCellChanged(KDbRecordData *data,
        QVariant& newValue, KDbResultInfo* result);

    class Private;
    Private * const d;

    friend class KexiQueryView; // for storeNewData() and storeData() only
};

#endif
