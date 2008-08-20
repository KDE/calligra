/* This file is part of the KDE project
   Copyright (C) 2004-2007 Jarosław Staniek <staniek@kde.org>

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

#ifndef KEXITABLEDESIGNERINTERVIEW_H
#define KEXITABLEDESIGNERINTERVIEW_H

#include <koproperty/property.h>
#include <kexidb/alter.h>
#include <core/kexitabledesignerinterface.h>

#include <kexidatatable.h>
#include "kexitablepart.h"
//Added by qt3to4:
#include <Q3CString>

namespace KexiDB
{
class RecordData;
}
class KexiTableDesignerViewPrivate;
class K3Command;
class CommandGroup;

namespace KoProperty
{
class Set;
}

//! Design view of the Table Designer
/*! Contains a spreadsheet-like space for entering field definitions.
 Property editor is provided for altering field definitions.

 The view also supports Undo and Redo operations.
 These are connected to a factility creating a list of actions used
 by AlterTableHandler to perform required operation of altering the table.

 Altering itself is performed upon design saving (storeData()).
 Saving unstored designs just creates a new table.
 Saving changes made to empty (not filled with data) table is performed
 by physically deleting the previous table schema and recreating it
 TODO: this will be not quite when we have db relationships supported.

 Saving changes made to table containing data requires use of the AlterTableHandler
 functionality.
*/
class KexiTableDesignerView : public KexiDataTable, public KexiTableDesignerInterface
{
    Q_OBJECT

public:
    /*! Creates a new alter table dialog. */
    KexiTableDesignerView(QWidget *parent);

    virtual ~KexiTableDesignerView();

    KexiTablePart::TempData* tempData() const;

    /*! Clears field information entered for row.
     This is performed by removing values from caption and data type columns.
     Used by InsertFieldCommand to undo inserting a new field. */
    virtual void clearRow(int row, bool addCommand = false);

    /*! Inserts a new field with \a caption for \a row.
     Property set is also created.  */
    virtual void insertField(int row, const QString& caption, bool addCommand = false);

    /*! Inserts a new \a field for \a row.
     Property set is also created. \a set will be deeply-copied into the new set.
     Used by InsertFieldCommand to insert a new field. */
    virtual void insertField(int row, KoProperty::Set& set, bool addCommand = false);

    /*! Inserts a new empty row at position \a row.
     Used by RemoveFieldCommand as a part of undo inserting a new field;
     also used by InsertEmptyRowCommand. */
    virtual void insertEmptyRow(int row, bool addCommand = false);

    /*! Deletes \a row from the table view. Property set is also deleted.
     All the subsequent fields are moved up. Used for undoing InsertEmptyRowCommand
     and by RemoveFieldCommand to remove a field. */
    virtual void deleteRow(int row, bool addCommand = false);

    /*! Deletes a field for \a row. Property set is also deleted.
     Used by RemoveFieldCommand to remove a field. */
//  virtual void deleteField( int row );

    /*! Changes property \a propertyName to \a newValue for a field at row \a row.
     If \a listData is not NULL and not empty, a deep copy of it is passed to Property::setListData().
     If \a listData \a nlist if not NULL but empty, Property::setListData(0) is called. */
    virtual void changeFieldPropertyForRow(int row,
                                           const Q3CString& propertyName, const QVariant& newValue,
                                           KoProperty::Property::ListData* const listData, bool addCommand);

    /*! Changes property \a propertyName to \a newValue.
     Works exactly like changeFieldPropertyForRow() except the field is pointed by \a fieldUID.
     Used by ChangeFieldPropertyCommand to change field's property. */
    void changeFieldProperty(int fieldUID, const Q3CString& propertyName,
                             const QVariant& newValue, KoProperty::Property::ListData* const listData = 0,
                             bool addCommand = false);

    /*! Changes visibility of property \a propertyName to \a visible for a field pointed by \a fieldUID.
     Used by ChangePropertyVisibilityCommand. */
    void changePropertyVisibility(int fieldUID, const Q3CString& propertyName, bool visible);

    /*! Builds table field's schema by looking at the \a set. */
    KexiDB::Field * buildField(const KoProperty::Set &set) const;

    /*! Creates temporary table for the current design and returns debug string for it. */
    virtual QString debugStringForCurrentTableSchema(tristate& result);

    /*! Simulates execution of alter table, and puts debug into \a debugTarget.
     A case when debugTarget is not 0 is true for the alter table test suite. */
    virtual tristate simulateAlterTableExecution(QString *debugTarget);

public slots:
    /*! Real execution of the Alter Table. For debugging of the real alter table.
     \return true on success, false on failure and cancelled if user has cancelled
     execution. */
    virtual tristate executeRealAlterTable();

protected slots:
    /*! Equivalent to updateActions(false). Called on row insert/delete
     in a KexiDataAwarePropertySet. */
    void updateActions();

    virtual void slotUpdateRowActions(int row);

    void slotAboutToShowContextMenu();

    //! Called before cell change in tableview.
    void slotBeforeCellChanged(KexiDB::RecordData *record, int colnum,
                               QVariant& newValue, KexiDB::ResultInfo* result);

    //! Called on row change in a tableview.
    void slotRowUpdated(KexiDB::RecordData *record);

    //! Called before row inserting in tableview.
    void slotRowInserted();
//  void slotAboutToInsertRow(KexiDB::RecordData* record, KexiDB::ResultInfo* result, bool repaint);

    //! Called before row deleting in tableview.
    void slotAboutToDeleteRow(KexiDB::RecordData& record, KexiDB::ResultInfo* result, bool repaint);

    /*! Called after any property has been changed in the current property set,
     to perform some actions (like updating other dependent properties) */
    void slotPropertyChanged(KoProperty::Set& set, KoProperty::Property& property);

    /*! Toggles primary key for currently selected field.
     Does nothing for empty row. */
    void slotTogglePrimaryKey();

    /*! Undoes the recently performed action. */
    void slotUndo();

    /*! Redoes the recently undoed action. */
    void slotRedo();

    /*! Reaction on command execution from the command history */
    void slotCommandExecuted(K3Command *command);

    /*! Simulates real execution of the Alter Table. For debugging. */
    void slotSimulateAlterTableExecution();

protected:
    virtual void updateActions(bool activated);

    //! called whenever data should be reloaded (on switching to this view mode)
    void initData();

    /*! Creates a new property set for \a field.
     The property set will be asigned to \a row, and owned by this dialog.
     If \a newOne is true, the property set will be marked as newly created.
     \return newly created property set. */
    KoProperty::Set* createPropertySet(int row, const KexiDB::Field& field, bool newOne = false);

    virtual tristate beforeSwitchTo(Kexi::ViewMode mode, bool &dontStore);

    virtual tristate afterSwitchFrom(Kexi::ViewMode mode);

    /*! \return property set associated with currently selected row (i.e. field)
     or 0 if current row is empty. */
    virtual KoProperty::Set *propertySet();

//  void removeCurrentPropertySet();

    /*! Reimplemented from KexiView, because tables creation is more complex.
     No table schema altering is required, so just buildSchema() is used to create a new schema.
    */
    virtual KexiDB::SchemaData* storeNewData(const KexiDB::SchemaData& sdata, bool &cancel);

    /*! Reimplemented from KexiView, because table storage is more complex.
     Table schema altering may be required, so just buildSchema() is used to create a new schema.
    */
    virtual tristate storeData(bool dontAsk = false);

    /*! Builds table schema by looking at the current design. Used in storeNewData()
     and storeData().
     If \a beSilent is true, no message boxes are used to show questions or warnings.
     This is used in the altertable test suite (kexi/tests/altertable).
     \return true on successful schema creating, false on failure and cancelled when there
     was a problem with user's design (and user has been informed about it). */
    tristate buildSchema(KexiDB::TableSchema &schema, bool beSilent = false);

    /*! Builds action list usable for KexiDB::AlterTableHandler by looking at undo buffer
     of commands' history. Used in storeData() */
    tristate buildAlterTableActions(KexiDB::AlterTableHandler::ActionList &actions);

    /*! Helper, used for slotTogglePrimaryKey() and slotPropertyChanged().
     Assigns primary key icon and value for property set \a propertySet,
     and deselects it from previous pkey's row.
     \a aWasPKey is internal.
     If \a commandGroup is not 0, it is used as parent group for storing actions' history. */
    void switchPrimaryKey(KoProperty::Set &propertySet, bool set, bool aWasPKey = false,
                          CommandGroup* commandGroup = 0);

    //! Gets subtype strings and names for type \a fieldType.
    void getSubTypeListData(KexiDB::Field::TypeGroup fieldTypeGroup,
                            QStringList& stringsList, QStringList& namesList);

    /*! Adds history command \a command to the undo/redo buffer.
     If \a execute is true, the command is executed afterwards. */
    void addHistoryCommand(K3Command* command, bool execute);

    //! Updates undo/redo shared actions availability by looking at command history's action
    void updateUndoRedoActions();

#ifdef KEXI_DEBUG_GUI
    void debugCommand(K3Command* command, int nestingLevel);
#endif

    /*! Inserts a new \a field for \a row.
     Property set is also created. If \a set is not 0 (the default),
     it will be copied into the new set. Used by insertField(). */
    void insertFieldInternal(int row, KoProperty::Set* set, const QString& caption, bool addCommand);

    //! Reimplemented to pass the information also to the "Lookup" tab
    virtual void propertySetSwitched();

    /*! \return true if physical altering is needed for the current list of actions.
     Used in KexiTableDesignerView::beforeSwitchTo() to avoid warning about removinf
     table data if table recreating is not needed.
     True is also returned if there is any trouble with getting the answer. */
    bool isPhysicalAlteringNeeded();

private:
    KexiTableDesignerViewPrivate * const d;
};

#endif
