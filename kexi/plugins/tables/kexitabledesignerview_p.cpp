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

#include "kexitabledesignerview_p.h"
#include "kexitabledesignerview.h"

#include <qlayout.h>
#include <qlabel.h>
#include <qsplitter.h>
//Added by qt3to4:
#include <Q3CString>

#include <kiconloader.h>
#include <kdebug.h>
#include <klocale.h>
#include <KActionCollection>

#include <koproperty/set.h>

#include <kexi_global.h>
#include <kexidb/cursor.h>
#include <kexidb/tableschema.h>
#include <kexidb/connection.h>
#include <kexidb/utils.h>
#include <kexidb/roweditbuffer.h>
#include <kexidb/error.h>
#include <kexiutils/identifier.h>
#include <kexiproject.h>
#include <KexiMainWindowIface.h>
#include <widget/tableview/kexidataawarepropertyset.h>
#include <widget/kexicustompropertyfactory.h>
#include <kexiutils/utils.h>
#include <KexiWindow.h>
#include <kexitableview.h>
#include "kexitabledesignercommands.h"

using namespace KexiTableDesignerCommands;

//----------------------------------------------

CommandHistory::CommandHistory(KActionCollection *actionCollection, bool withMenus)
        : K3CommandHistory(actionCollection, withMenus)
{
    // We need ALL the commands because we'll collect reuse their
    // data before performing alter table, so set that to the maximum,
    // as K3CommandHistory has default = 50.
    setUndoLimit(INT_MAX);
    setRedoLimit(INT_MAX);
}

void CommandHistory::addCommand(K3Command *command, bool execute)
{
    K3CommandHistory::addCommand(command, execute);
    m_commandsToUndo.append(command);
}

void CommandHistory::undo()
{
    if (!m_commandsToUndo.isEmpty()) {
        K3Command * cmd = m_commandsToUndo.takeAt(m_commandsToUndo.count() - 1);
        m_commandsToRedo.append(cmd);
    }
    K3CommandHistory::undo();
}

void CommandHistory::redo()
{
    if (!m_commandsToRedo.isEmpty()) {
        K3Command * cmd = m_commandsToRedo.takeAt(m_commandsToRedo.count() - 1);
        m_commandsToUndo.append(cmd);
    }
    K3CommandHistory::redo();
}

void CommandHistory::clear()
{
    K3CommandHistory::clear(); m_commandsToUndo.clear();
}

//----------------------------------------------

KexiTableDesignerViewPrivate::KexiTableDesignerViewPrivate(
    KexiTableDesignerView* aDesignerView)
        : designerView(aDesignerView)
        , sets(0)
        , uniqueIdCounter(0)
        , dontAskOnStoreData(false)
        , slotTogglePrimaryKeyCalled(false)
        , primaryKeyExists(false)
        , slotPropertyChanged_primaryKey_enabled(true)
        , slotPropertyChanged_subType_enabled(true)
        , addHistoryCommand_in_slotPropertyChanged_enabled(true)
        , addHistoryCommand_in_slotRowUpdated_enabled(true)
        , addHistoryCommand_in_slotAboutToDeleteRow_enabled(true)
        , addHistoryCommand_in_slotRowInserted_enabled(true)
        , slotBeforeCellChanged_enabled(true)
        , tempStoreDataUsingRealAlterTable(false)
{
    historyActionCollection = new KActionCollection((QWidget*)0);
    history = new CommandHistory(historyActionCollection, true);

    internalPropertyNames
    << "subType" << "uid" << "newrow" << "rowSource" << "rowSourceType"
    << "boundColumn" << "visibleColumn";
}

KexiTableDesignerViewPrivate::~KexiTableDesignerViewPrivate()
{
    delete sets;
    delete historyActionCollection;
    delete history;
}

int KexiTableDesignerViewPrivate::generateUniqueId()
{
    return ++uniqueIdCounter;
}

void KexiTableDesignerViewPrivate::setPropertyValueIfNeeded(
    const KoProperty::Set& set, const Q3CString& propertyName,
    const QVariant& newValue, const QVariant& oldValue, CommandGroup* commandGroup,
    bool forceAddCommand, bool rememberOldValue,
    QStringList* const slist, QStringList* const nlist)
{
    KoProperty::Property& property = set[propertyName];

    //remember because we'll change list data soon
    KoProperty::Property::ListData *oldListData = property.listData() ?
            new KoProperty::Property::ListData(*property.listData()) : 0;
    if (slist && nlist) {
        if (slist->isEmpty() || nlist->isEmpty()) {
            property.setListData(0);
        } else {
            property.setListData(*slist, *nlist);
        }
    }
    if (oldValue.type() == newValue.type()
            && (oldValue == newValue || (!oldValue.isValid() && !newValue.isValid()))
            && !forceAddCommand) {
        return;
    }

    const bool prev_addHistoryCommand_in_slotPropertyChanged_enabled
    = addHistoryCommand_in_slotPropertyChanged_enabled; //remember
    addHistoryCommand_in_slotPropertyChanged_enabled = false;
    if (property.value() != newValue)
        property.setValue(newValue, rememberOldValue);
    if (commandGroup) {
        commandGroup->addCommand(
            new ChangeFieldPropertyCommand(designerView, set, propertyName, oldValue, newValue,
                                           oldListData, property.listData()));
    }
    delete oldListData;
    addHistoryCommand_in_slotPropertyChanged_enabled
    = prev_addHistoryCommand_in_slotPropertyChanged_enabled; //restore
}

void KexiTableDesignerViewPrivate::setPropertyValueIfNeeded(
    const KoProperty::Set& set, const Q3CString& propertyName,
    const QVariant& newValue, CommandGroup* commandGroup,
    bool forceAddCommand, bool rememberOldValue,
    QStringList* const slist, QStringList* const nlist)
{
    KoProperty::Property& property = set[propertyName];
    QVariant oldValue(property.value());
    setPropertyValueIfNeeded(set, propertyName, newValue, property.value(),
                             commandGroup, forceAddCommand, rememberOldValue, slist, nlist);
}

void KexiTableDesignerViewPrivate::setVisibilityIfNeeded(const KoProperty::Set& set, KoProperty::Property* prop,
        bool visible, bool &changed, CommandGroup *commandGroup)
{
    if (prop->isVisible() != visible) {
        if (commandGroup) {
            commandGroup->addCommand(
                new ChangePropertyVisibilityCommand(designerView, set, prop->name(), visible));
        }
        prop->setVisible(visible);
        changed = true;
    }
}

bool KexiTableDesignerViewPrivate::updatePropertiesVisibility(KexiDB::Field::Type fieldType, KoProperty::Set &set,
        CommandGroup *commandGroup)
{
    bool changed = false;
    KoProperty::Property *prop;
    bool visible;

    prop = &set["subType"];
    kexipluginsdbg << "subType=" << prop->value().toInt()
    << " type=" << set["type"].value().toInt();

    //if there is no more than 1 subType name or it's a PK: hide the property
    visible =
        (prop->listData() && prop->listData()->keys.count() > 1 /*disabled || isObjectTypeGroup*/)
        && set["primaryKey"].value().toBool() == false;
    setVisibilityIfNeeded(set, prop, visible, changed, commandGroup);

    prop = &set["objectType"];
    const bool isObjectTypeGroup
    = set["type"].value().toInt() == (int)KexiDB::Field::BLOB; // used only for BLOBs
    visible = isObjectTypeGroup;
    setVisibilityIfNeeded(set, prop,  visible, changed, commandGroup);

    prop = &set["unsigned"];
    visible = KexiDB::Field::isNumericType(fieldType);
    setVisibilityIfNeeded(set, prop, visible, changed, commandGroup);

    prop = &set["length"];
    visible = (fieldType == KexiDB::Field::Text);
    if (prop->isVisible() != visible) {
//    prop->setVisible( visible );
        //update the length when it makes sense
        const int lengthToSet = visible ? KexiDB::Field::defaultTextLength() : 0;
        setPropertyValueIfNeeded(set, "length", lengthToSet,
                                 commandGroup, false, false /*!rememberOldValue*/);
//  if (lengthToSet != prop->value().toInt())
//   prop->setValue( lengthToSet, false );
//    changed = true;
    }
    setVisibilityIfNeeded(set, prop, visible, changed, commandGroup);
#ifndef KEXI_NO_UNFINISHED
    prop = &set["precision"];
    visible = KexiDB::Field::isFPNumericType(fieldType);
    setVisibilityIfNeeded(set, prop, visible, changed, commandGroup);
#endif
    prop = &set["visibleDecimalPlaces"];
    visible = KexiDB::supportsVisibleDecimalPlacesProperty(fieldType);
    setVisibilityIfNeeded(set, prop, visible, changed, commandGroup);

    prop = &set["unique"];
    visible = fieldType != KexiDB::Field::BLOB;
    setVisibilityIfNeeded(set, prop, visible, changed, commandGroup);

    prop = &set["indexed"];
    visible = fieldType != KexiDB::Field::BLOB;
    setVisibilityIfNeeded(set, prop, visible, changed, commandGroup);

    prop = &set["allowEmpty"];
    visible = KexiDB::Field::hasEmptyProperty(fieldType);
    setVisibilityIfNeeded(set, prop, visible, changed, commandGroup);

    prop = &set["autoIncrement"];
    visible = KexiDB::Field::isAutoIncrementAllowed(fieldType);
    setVisibilityIfNeeded(set, prop, visible, changed, commandGroup);

//! @todo remove this when BLOB supports default value
#ifdef KEXI_NO_UNFINISHED
    prop = &set["defaultValue"];
    visible = !isObjectTypeGroup;
    setVisibilityIfNeeded(set, prop, visible, changed, commandGroup);
#endif

    return changed;
}

QString KexiTableDesignerViewPrivate::messageForSavingChanges(bool &emptyTable, bool skipWarning)
{
    KexiDB::Connection *conn = KexiMainWindowIface::global()->project()->dbConnection();
    bool ok;
    emptyTable = conn->isEmpty(*designerView->tempData()->table, ok) && ok;
    return i18n("Do you want to save the design now?")
           + ((emptyTable || skipWarning) ? QString() :
              (QString("\n\n") + designerView->part()->i18nMessage(":additional message before saving design",
                      designerView->window()).toString()));
}

void KexiTableDesignerViewPrivate::updateIconForRecord(KexiDB::RecordData &record, KoProperty::Set& set)
{
    QVariant icon;
    if (!set["rowSource"].value().toString().isEmpty()
            && !set["rowSourceType"].value().toString().isEmpty()) {
        icon = "combo";
    }
    //show/hide icon in the table
    view->data()->clearRowEditBuffer();
    view->data()->updateRowEditBuffer(&record, COLUMN_ID_ICON, icon);
    view->data()->saveRowChanges(record, true);
}

#include "kexitabledesignerview_p.moc"
