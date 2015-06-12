/* This file is part of the KDE project
   Copyright (C) 2004-2012 Jarosław Staniek <staniek@kde.org>

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

#include "kexitabledesignerview.h"
#include "kexitabledesignerview_p.h"
#include "kexilookupcolumnpage.h"
#include "kexitabledesignercommands.h"
#include <KexiIcon.h>
#include <kexiproject.h>
#include <KexiMainWindowIface.h>
#include <widget/dataviewcommon/kexidataawarepropertyset.h>
#include <widget/properties/KexiCustomPropertyFactory.h>
#include <widget/tableview/KexiTableScrollArea.h>
#include <kexiutils/utils.h>
#include <KexiWindow.h>
#include <kexi_global.h>

#include <KPropertySet>

#include <KDb>
#include <KDbCursor>
#include <KDbTableSchema>
#include <KDbConnection>
#include <KDbUtils>
#include <KDbRecordEditBuffer>
#include <KDbError>
#include <KDbLookupFieldSchema>

#include <ktoggleaction.h>
#include <KMessageBox>
#include <kundo2command.h>
#include <KActionCollection>
#include <KLocalizedString>

#include <QByteArray>
#include <QHash>
#include <QDebug>

//! used only for BLOBs
#define DEFAULT_OBJECT_TYPE_VALUE "image"

//#define KexiTableDesignerView_DEBUG

//! @todo remove this when BLOBs are implemented
//#define KEXI_NO_BLOB_FIELDS

//#define KEXI_NO_UNDOREDO_ALTERTABLE

using namespace KexiTableDesignerCommands;

//! @internal Used in tryCastQVariant() anf canCastQVariant()
static bool isIntegerQVariant(QVariant::Type t)
{
    return t == QVariant::LongLong
           || t == QVariant::ULongLong
           || t == QVariant::Int
           || t == QVariant::UInt;
}

//! @internal Used in tryCastQVariant()
static bool canCastQVariant(QVariant::Type fromType, QVariant::Type toType)
{
    return (fromType == QVariant::Int && toType == QVariant::UInt)
           || (fromType == QVariant::ByteArray && toType == QVariant::String)
           || (fromType == QVariant::LongLong && toType == QVariant::ULongLong)
           || ((fromType == QVariant::String || fromType == QVariant::ByteArray)
               && (isIntegerQVariant(toType) || toType == QVariant::Double));
}

/*! @internal
 \return a variant value converted from \a fromVal to \a toType type.
 Null QVariant is returned if \a fromVal's type and \a toType type
 are incompatible. */
static QVariant tryCastQVariant(const QVariant& fromVal, QVariant::Type toType)
{
    const QVariant::Type fromType = fromVal.type();
    if (fromType == toType)
        return fromVal;
    if (canCastQVariant(fromType, toType) || canCastQVariant(toType, fromType)
            || (isIntegerQVariant(fromType) && toType == QVariant::Double)) {
        QVariant res(fromVal);
        if (res.convert(toType))
            return res;
    }
    return QVariant();
}


KexiTableDesignerView::KexiTableDesignerView(QWidget *parent)
        : KexiDataTableView(parent, false/*not db-aware*/)
        , KexiTableDesignerInterface()
        , d(new KexiTableDesignerViewPrivate(this))
{
    setObjectName("KexiTableDesignerView");
    //needed for custom "identifier" property editor widget
    KexiCustomPropertyFactory::init();

    KDbConnection *conn = KexiMainWindowIface::global()->project()->dbConnection();
    d->view = dynamic_cast<KexiTableScrollArea*>(mainWidget());

    d->data = new KDbTableViewData();
    if (conn->options()->isReadOnly())
        d->data->setReadOnly(true);
    d->data->setInsertingEnabled(false);

    KDbTableViewColumn *col = new KDbTableViewColumn("pk", KDbField::Text, QString(),
            xi18n("Additional information about the field"));
    col->setIcon(KexiUtils::colorizeIconToTextColor(koSmallIcon("help-about"), d->view->palette()));
    col->setHeaderTextVisible(false);
    col->field()->setSubType("QIcon");
    col->setReadOnly(true);
    d->data->addColumn(col);

    col = new KDbTableViewColumn("caption", KDbField::Text, xi18n("Field Caption"),
                                  xi18n("Describes caption for the field"));
    d->data->addColumn(col);

    col = new KDbTableViewColumn("type", KDbField::Enum, xi18n("Data Type"),
                                  xi18n("Describes data type for the field"));
    d->data->addColumn(col);

#ifdef KEXI_NO_BLOB_FIELDS
//! @todo remove this later
    QVector<QString> types(KDbField::LastTypeGroup - 1); //don't show last type (BLOB)
#else
    QVector<QString> types(KDbField::LastTypeGroup);
#endif
    d->maxTypeNameTextWidth = 0;
    QFontMetrics fm(font());
    for (uint i = 1; i <= (uint)types.count(); i++) {
        types[i-1] = KDbField::typeGroupName(i);
        d->maxTypeNameTextWidth = qMax(d->maxTypeNameTextWidth, fm.width(types[i-1]));
    }
    col->field()->setEnumHints(types);

    d->data->addColumn(col = new KDbTableViewColumn("comments", KDbField::Text,
            xi18n("Comments"), xi18n("Describes additional comments for the field")));

    d->view->setSpreadSheetMode(true);

    connect(d->data, SIGNAL(aboutToChangeCell(KDbRecordData*,int,QVariant&,KDbResultInfo*)),
            this, SLOT(slotBeforeCellChanged(KDbRecordData*,int,QVariant&,KDbResultInfo*)));
    connect(d->data, SIGNAL(rowUpdated(KDbRecordData*)),
            this, SLOT(slotRowUpdated(KDbRecordData*)));
    connect(d->data, SIGNAL(aboutToDeleteRow(KDbRecordData&,KDbResultInfo*,bool)),
            this, SLOT(slotAboutToDeleteRow(KDbRecordData&,KDbResultInfo*,bool)));

    setMinimumSize(d->view->minimumSizeHint().width(), d->view->minimumSizeHint().height());
    d->view->setFocus();

    d->sets = new KexiDataAwarePropertySet(this, d->view);
    connect(d->sets, SIGNAL(rowDeleted()), this, SLOT(updateActions()));
    connect(d->sets, SIGNAL(rowInserted()), this, SLOT(slotRowInserted()));

    connect(d->view->contextMenu(), SIGNAL(aboutToShow()),
            this, SLOT(slotAboutToShowContextMenu()));

    // - setup local actions
    QList<QAction*> viewActions;
    QAction* a;
    viewActions << (d->action_toggle_pkey = new KToggleAction(koIcon("key"), xi18n("Primary Key"), this));
    a = d->action_toggle_pkey;
    a->setObjectName("tablepart_toggle_pkey");
    a->setToolTip(xi18n("Sets or removes primary key"));
    a->setWhatsThis(xi18n("Sets or removes primary key for currently selected field."));
    connect(a, SIGNAL(triggered()), this, SLOT(slotTogglePrimaryKey()));
    setViewActions(viewActions);

    d->view->contextMenu()->insertAction(
        d->view->contextMenu()->actions()[1], d->action_toggle_pkey); //add at the beginning as 2nd
    d->view->contextMenu()->insertSeparator(d->view->contextMenu()->actions()[2]);   //as 3rd
    setAvailable("tablepart_toggle_pkey", !conn->options()->isReadOnly());

#ifndef KEXI_NO_UNDOREDO_ALTERTABLE
    plugSharedAction("edit_undo", this, SLOT(slotUndo()));
    plugSharedAction("edit_redo", this, SLOT(slotRedo()));
    setAvailable("edit_undo", false);
    setAvailable("edit_redo", false);
   //!todo qundo
    connect(d->history, SIGNAL(commandExecuted(K3Command*)),
            this, SLOT(slotCommandExecuted(K3Command*)));
#endif

#ifdef KEXI_DEBUG_GUI
    KDb::alterTableActionDebugGUI(QString()); //to create the tab
    KexiUtils::connectPushButtonActionForDebugWindow(
        "simulateAlterTableExecution", this, SLOT(slotSimulateAlterTableExecution()));
    KexiUtils::connectPushButtonActionForDebugWindow(
        "executeRealAlterTable", this, SLOT(executeRealAlterTable()));
#endif
    // setup main menu actions
    QList<QAction*> mainMenuActions;
    a = sharedAction("edit_clear_table");
    mainMenuActions << a;

    setMainMenuActions(mainMenuActions);
}

KexiTableDesignerView::~KexiTableDesignerView()
{
    delete d;
}

void KexiTableDesignerView::initData()
{
    //add column data
    d->data->deleteAllRecords();
    uint tableFieldCount = 0;
    d->primaryKeyExists = false;

    if (tempData()->table) {
        tableFieldCount = tempData()->table->fieldCount();

        //recreate table data rows
        for (uint i = 0; i < tableFieldCount; i++) {
            KDbField *field = tempData()->table->field(i);
            KDbRecordData *record = d->data->createItem();
            if (field->isPrimaryKey()) {
                (*record)[COLUMN_ID_ICON] = "key";
                d->primaryKeyExists = true;
            } else {
                KDbLookupFieldSchema *lookupFieldSchema
                    = field->table() ? field->table()->lookupFieldSchema(*field) : 0;
                if (lookupFieldSchema
                    && lookupFieldSchema->rowSource().type() != KDbLookupFieldSchema::RowSource::NoType
                    && !lookupFieldSchema->rowSource().name().isEmpty())
                {
                    (*record)[COLUMN_ID_ICON] = "combo";
                }
            }
            (*record)[COLUMN_ID_CAPTION] = field->captionOrName();
            (*record)[COLUMN_ID_TYPE] = field->typeGroup() - 1; //-1 because type groups are counted from 1
            (*record)[COLUMN_ID_DESC] = field->description();
            d->data->append(record);
        }
    }

    //add empty space, at least 2 times more than number of existing fields
    uint fullSize = qMax(d->sets->size(), uint(2 * tableFieldCount));
    for (uint i = tableFieldCount; i < fullSize; i++) {
        d->data->append(d->data->createItem());
    }

    //set data for our spreadsheet: this will clear our sets
    d->view->setData(d->data);

    //now recreate property sets
    if (tempData()->table) {
        for (uint i = 0; i < tableFieldCount; i++) {
            KDbField *field = tempData()->table->field(i);
            createPropertySet(i, *field);
        }
    }

    //column widths
    d->view->setColumnWidth(COLUMN_ID_ICON, IconSize(KIconLoader::Small) + 10);
    d->view->setColumnResizeEnabled(COLUMN_ID_ICON, false);
    d->view->adjustColumnWidthToContents(COLUMN_ID_CAPTION); //adjust column width
    d->view->setColumnWidth(COLUMN_ID_TYPE, d->maxTypeNameTextWidth + 2 * d->view->rowHeight());
    d->view->setStretchLastColumn(true);
    const int minCaptionColumnWidth = d->view->fontMetrics().width("wwwwwwwwwww");
    if (minCaptionColumnWidth > d->view->columnWidth(COLUMN_ID_CAPTION))
        d->view->setColumnWidth(COLUMN_ID_CAPTION, minCaptionColumnWidth);

    setDirty(false);
    d->view->setCursorPosition(0, COLUMN_ID_CAPTION); //set @ name column
    propertySetSwitched();
}

//! Gets subtype strings and names for type \a fieldType
void
KexiTableDesignerView::getSubTypeListData(KDbField::TypeGroup fieldTypeGroup,
        QStringList& stringsList, QStringList& namesList)
{
    /* disabled - "mime" is moved from subType to "objectType" custom property
      if (fieldTypeGroup==KDbField::BLOBGroup) {
        // special case: BLOB type uses "mime-based" subtypes
    //! @todo hardcoded!
        stringsList << "image";
        namesList << xi18n("Image object type", "Image");
      }
      else {*/
    stringsList = KDb::typeStringsForGroup(fieldTypeGroup);
    namesList = KDb::typeNamesForGroup(fieldTypeGroup);
// }
    qDebug() << "subType strings: " <<
        stringsList.join("|") << "\nnames: " << namesList.join("|");
}

KPropertySet *
KexiTableDesignerView::createPropertySet(int row, const KDbField& field, bool newOne)
{
    QString typeName = "KDbField::" + field.typeGroupString();
    KPropertySet *set = new KPropertySet(d->sets, typeName);
    if (KexiMainWindowIface::global()->project()->dbConnection()->options()->isReadOnly())
        set->setReadOnly(true);

    KProperty *prop;
    set->addProperty(prop = new KProperty("uid", d->generateUniqueId(), ""));
    prop->setVisible(false);

    //meta-info for property editor
    set->addProperty(prop = new KProperty("this:classString", xi18n("Table field")));
    prop->setVisible(false);
    set->addProperty(prop = new KProperty("this:iconName",
//! \todo add table_field icon
            "lineedit" //"table_field"
                                                    ));
    prop->setVisible(false);
    set->addProperty(prop = new KProperty("this:useCaptionAsObjectName",
            QVariant(true), QString())); //we want "caption" to be displayed in the header, not name
    prop->setVisible(false);

    //name
    set->addProperty(prop = new KProperty(
                                    "name", QVariant(field.name()), xi18n("Name"),
                                    QString(), KexiCustomPropertyFactory::Identifier));
    //type
    set->addProperty(prop = new KProperty("type", QVariant(field.type()), xi18n("Type")));
#ifndef KexiTableDesignerView_DEBUG
    prop->setVisible(false);//always hidden
#endif

    //subtype
    QStringList typeStringList, typeNameList;
    getSubTypeListData(field.typeGroup(), typeStringList, typeNameList);
    /* disabled - "mime" is moved from subType to "objectType" custom property
      QString subTypeValue;
      if (field.typeGroup()==KDbField::BLOBGroup) {
    // special case: BLOB type uses "mime-based" subtypes
    //! @todo this should be retrieved from KDbField when BLOB supports many different mimetypes
        subTypeValue = slist.first();
      }
      else {*/
    QString subTypeValue = field.typeString();
    //}
    set->addProperty(prop = new KProperty("subType",
            typeStringList, typeNameList, subTypeValue, xi18n("Subtype")));

    // objectType
    QStringList objectTypeStringList, objectTypeNameList;
//! @todo this should be retrieved from KDbField when BLOB supports many different mimetypes
    objectTypeStringList << "image";
    objectTypeNameList << xi18nc("Image object type", "Image");
    QString objectTypeValue(field.customProperty("objectType").toString());
    if (objectTypeValue.isEmpty())
        objectTypeValue = DEFAULT_OBJECT_TYPE_VALUE;
    set->addProperty(prop = new KProperty("objectType",
            objectTypeStringList, objectTypeNameList, objectTypeValue,
            xi18n("Subtype")/*! @todo other i18n string?*/));

    set->addProperty(prop = new KProperty("caption", QVariant(field.caption()),
                                                     xi18n("Caption")));
    prop->setVisible(false);//always hidden

    set->addProperty(prop = new KProperty("description", QVariant(field.description())));
    prop->setVisible(false);//always hidden

    set->addProperty(prop = new KProperty("unsigned", QVariant(field.isUnsigned()),
                                                     xi18n("Unsigned Number")));

    set->addProperty(prop = new KProperty("maxLength", (uint)field.maxLength(),
                                                     xi18n("Max Length")));

    set->addProperty(prop  = new KProperty("maxLengthIsDefault",
                               field.maxLengthStrategy() == KDbField::DefaultMaxLength));
    prop->setVisible(false); //always hidden

    set->addProperty(prop = new KProperty("precision", (int)field.precision()/*200?*/,
                                                     xi18n("Precision")));
#ifndef KEXI_SHOW_UNFINISHED
    prop->setVisible(false);
#endif
    set->addProperty(prop = new KProperty("visibleDecimalPlaces",
                                    field.visibleDecimalPlaces(), xi18n("Visible Decimal Places")));
    prop->setOption("min", -1);
    prop->setOption("minValueText", xi18nc("Auto Decimal Places", "Auto"));

//! @todo set reasonable default for column width
    set->addProperty(prop = new KProperty("defaultWidth", QVariant(0) /*field.width()*//*200?*/,
                                                     xi18n("Default Width")));
#ifndef KEXI_SHOW_UNFINISHED
    prop->setVisible(false);
#endif

    set->addProperty(prop = new KProperty("defaultValue", field.defaultValue(),
                                                     xi18n("Default Value"), QString(),
//! @todo use "Variant" type here when supported by KProperty
                                                (KProperty::Type)field.variantType()));
    prop->setOption("3rdState", xi18n("None"));

    set->addProperty(prop = new KProperty("primaryKey", QVariant(field.isPrimaryKey()),
                                                     xi18n("Primary Key")));
    prop->setIcon("key");

    set->addProperty(prop = new KProperty("unique", QVariant(field.isUniqueKey()),
                                                     xi18n("Unique")));

    set->addProperty(prop = new KProperty("notNull", QVariant(field.isNotNull()),
                                                     xi18n("Required")));

    set->addProperty(prop = new KProperty("allowEmpty", QVariant(!field.isNotEmpty()),
                                                xi18n("Allow Zero\nSize")));

    set->addProperty(prop = new KProperty("autoIncrement", QVariant(field.isAutoIncrement()),
                                                     xi18n("Autonumber")));
    prop->setIcon("autonumber");

    set->addProperty(prop = new KProperty("indexed", QVariant(field.isIndexed()),
                                                     xi18n("Indexed")));

    //- properties related to lookup columns (used and set by the "lookup column"
    //  tab in the property pane)
    KDbLookupFieldSchema *lookupFieldSchema
        = field.table() ? field.table()->lookupFieldSchema(field) : 0;
    set->addProperty(prop = new KProperty("rowSource",
        lookupFieldSchema ? lookupFieldSchema->rowSource().name() : QString(), xi18n("Record Source")));
    prop->setVisible(false);

    set->addProperty(prop = new KProperty("rowSourceType",
            lookupFieldSchema ? lookupFieldSchema->rowSource().typeName() : QString(),
            xi18nc("Record source type (in two rows)", "Record Source\nType")));
    prop->setVisible(false);

    set->addProperty(prop = new KProperty("boundColumn",
            lookupFieldSchema ? lookupFieldSchema->boundColumn() : -1, xi18n("Bound Column")));
    prop->setVisible(false);

//! @todo this is backward-compatible code for "single visible column" implementation
//!       for multiple columns, only the first is displayed, so there is a data loss is GUI is used
//!       -- special kproperty editor needed
    int visibleColumn = -1;
    if (lookupFieldSchema && !lookupFieldSchema->visibleColumns().isEmpty())
        visibleColumn = lookupFieldSchema->visibleColumns().first();
    set->addProperty(prop = new KProperty("visibleColumn", visibleColumn,
                                                     xi18n("Visible Column")));
    prop->setVisible(false);

//! @todo support columnWidths(), columnHeadersVisible(), maximumListRows(), limitToList(), displayWidget()

    //----
    d->updatePropertiesVisibility(field.type(), *set);

    connect(set, SIGNAL(propertyChanged(KPropertySet&,KProperty&)),
            this, SLOT(slotPropertyChanged(KPropertySet&,KProperty&)));

    d->sets->set(row, set, newOne);
    return set;
}

void KexiTableDesignerView::updateActions(bool activated)
{
    Q_UNUSED(activated);
    /*! \todo check if we can set pkey for this column type (eg. BLOB?) */
    setAvailable("tablepart_toggle_pkey",
                 propertySet() != 0
                 && !KexiMainWindowIface::global()->project()->dbConnection()->options()->isReadOnly());
    if (!propertySet())
        return;
    KPropertySet &set = *propertySet();
    d->slotTogglePrimaryKeyCalled = true;
    d->action_toggle_pkey->setChecked(set["primaryKey"].value().toBool());
    d->slotTogglePrimaryKeyCalled = false;
}

void KexiTableDesignerView::slotUpdateRowActions(int row)
{
    KexiDataTableView::slotUpdateRowActions(row);
    updateActions();
}

void KexiTableDesignerView::slotTogglePrimaryKey()
{
    if (d->slotTogglePrimaryKeyCalled)
        return;
    d->slotTogglePrimaryKeyCalled = true;
    if (!propertySet())
        return;
    KPropertySet &set = *propertySet();
    bool isSet = !set["primaryKey"].value().toBool();
    set.changeProperty("primaryKey", QVariant(isSet)); //this will update all related properties as well
    d->slotTogglePrimaryKeyCalled = false;
}

void KexiTableDesignerView::switchPrimaryKey(KPropertySet &propertySet,
                                             bool set, bool aWasPKey, Command* commandGroup)
{
    const bool was_pkey = aWasPKey || propertySet["primaryKey"].value().toBool();
    d->setPropertyValueIfNeeded(propertySet, "primaryKey", QVariant(set), commandGroup);
    if (&propertySet == this->propertySet()) {
        //update action and icon @ column 0 (only if we're changing current property set)
        d->action_toggle_pkey->setChecked(set);
        if (d->view->selectedItem()) {
            //show key in the table
            d->view->data()->clearRowEditBuffer();
            d->view->data()->updateRowEditBuffer(d->view->selectedItem(), COLUMN_ID_ICON,
                                                 QVariant(set ? "key" : ""));
            d->view->data()->saveRowChanges(*d->view->selectedItem(), true);
        }
        if (was_pkey || set) //change flag only if we're setting pk or really clearing it
            d->primaryKeyExists = set;
    }

    if (set) {
        //primary key is set, remove old pkey if exists
        KPropertySet *s = 0;
        int i;
        const int count = (int)d->sets->size();
        for (i = 0; i < count; i++) {
            s = d->sets->at(i);
            if (   s && s != &propertySet && (*s)["primaryKey"].value().toBool()
                && i != d->view->currentRow())
            {
                break;
            }
        }
        if (i < count) {//remove
            d->setPropertyValueIfNeeded(*s, "autoIncrement", QVariant(false), commandGroup);
            d->setPropertyValueIfNeeded(*s, "primaryKey", QVariant(false), commandGroup);
            //remove key from table
            d->view->data()->clearRowEditBuffer();
            KDbRecordData *record = d->view->itemAt(i);
            if (record) {
                d->view->data()->updateRowEditBuffer(record, COLUMN_ID_ICON, QVariant());
                d->view->data()->saveRowChanges(*record, true);
            }
        }
        //set unsigned big-integer type
        d->slotBeforeCellChanged_enabled = false;
        d->view->data()->clearRowEditBuffer();
        d->view->data()->updateRowEditBuffer(d->view->selectedItem(), COLUMN_ID_TYPE,
                                             QVariant(KDbField::IntegerGroup - 1/*counting from 0*/));
        d->view->data()->saveRowChanges(*d->view->selectedItem(), true);
        d->setPropertyValueIfNeeded(propertySet, "subType",
                                    KDbField::typeString(KDbField::BigInteger),
                                    commandGroup);
        d->setPropertyValueIfNeeded(propertySet, "unsigned", QVariant(true), commandGroup);
        //! @todo
        d->slotBeforeCellChanged_enabled = true;
    }
    updateActions();
}

tristate KexiTableDesignerView::beforeSwitchTo(Kexi::ViewMode mode, bool *dontStore)
{
    if (!d->view->acceptRowEdit())
        return false;
    tristate res = true;
    if (mode == Kexi::DataViewMode) {
        if (!isDirty() && window()->neverSaved()) {
            KMessageBox::sorry(this,
                               xi18n("Cannot switch to data view, because table design is empty.\n"
                                    "First, please create your design."));
            return cancelled;
        }
//<temporary>
        else if (isDirty() && !window()->neverSaved()) {
//   cancelled = (KMessageBox::No == KMessageBox::questionYesNo(this, xi18n("Saving changes for existing table design is not yet supported.\nDo you want to discard your changes now?")));
//   KDbConnection *conn = KexiMainWindowIface::global()->project()->dbConnection();
            bool emptyTable;
            int r = KMessageBox::warningYesNoCancel(this,
                xi18n("Saving changes for existing table design is now required.")
                + "\n"
                + d->messageForSavingChanges(emptyTable, /*skip warning?*/!isPhysicalAlteringNeeded()),
                QString(),
                KStandardGuiItem::save(), KStandardGuiItem::discard(), KStandardGuiItem::cancel(),
                QString(),
                KMessageBox::Notify | KMessageBox::Dangerous);
            if (r == KMessageBox::Cancel)
                res = cancelled;
            else
                res = true;
            dontStore = (r != KMessageBox::Yes);
            if (!dontStore)
                d->dontAskOnStoreData = true;
        }
//</temporary>
        //! @todo
        return res;
    } else if (mode == Kexi::TextViewMode) {
        //! @todo
    }
    return res;
}

tristate KexiTableDesignerView::afterSwitchFrom(Kexi::ViewMode mode)
{
    if (mode == Kexi::NoViewMode || mode == Kexi::DataViewMode) {
        initData();
    }
    return true;
}

KPropertySet *KexiTableDesignerView::propertySet()
{
    return d->sets ? d->sets->currentPropertySet() : 0;
}

void KexiTableDesignerView::slotBeforeCellChanged(
    KDbRecordData *record, int colnum, QVariant& newValue, KDbResultInfo* /*result*/)
{
    if (!d->slotBeforeCellChanged_enabled)
        return;
    // qDebug() << d->view->selectedItem() << " " << item
    //<< " " << d->sets->at( d->view->currentRow() ) << " " << propertySet();
    if (colnum == COLUMN_ID_CAPTION) {//'caption'
        //if 'type' is not filled yet
        if (record->at(COLUMN_ID_TYPE).isNull()) {
            //auto select 1st row of 'type' column
            d->view->data()->updateRowEditBuffer(record, COLUMN_ID_TYPE, QVariant((int)0));
        }

        KPropertySet *propertySetForRecord = d->sets->findPropertySetForItem(*record);
        if (propertySetForRecord) {
            d->addHistoryCommand_in_slotPropertyChanged_enabled = false; // because we'll add
                                                                         // the two changes as one group
            QString oldName(propertySetForRecord->property("name").value().toString());
            QString oldCaption(propertySetForRecord->property("caption").value().toString());

            //remember this action containing 2 subactions
            //Parent command is a Command containing 2 child commands
            Command *changeCaptionAndNameCommand = new Command(
                kundo2_i18n(
                    "Change \"%1\" field's name to \"%2\" and caption from \"%3\" to \"%4\"",
                    oldName, propertySetForRecord->property("name").value().toString(),
                    oldCaption, newValue.toString()), 0, this
            );

            //we need to create the action now as set["name"] will be changed soon.
            //Child 1 is the caption
            /*ChangeFieldPropertyCommand *changeCaptionCommand = */
                (void)new ChangeFieldPropertyCommand(changeCaptionAndNameCommand, this,
                          *propertySetForRecord, "caption", oldCaption, newValue);

            //update field caption and name
            propertySetForRecord->changeProperty("caption", newValue);
            propertySetForRecord->changeProperty("name",
                                                 KDb::stringToIdentifier(newValue.toString()));

            //Child 2 is the name
            /*ChangeFieldPropertyCommand *changeNameCommand =*/
            (void)new ChangeFieldPropertyCommand(
                changeCaptionAndNameCommand, this, *propertySetForRecord,
                "name", oldName, propertySetForRecord->property("name").value().toString());
            addHistoryCommand(changeCaptionAndNameCommand, false /* !execute */);
            d->addHistoryCommand_in_slotPropertyChanged_enabled = true;
        }
    } else if (colnum == COLUMN_ID_TYPE) {//'type'
        if (newValue.isNull()) {
            //'type' col will be cleared: clear all other columns as well
            d->slotBeforeCellChanged_enabled = false;
            d->view->data()->updateRowEditBuffer(record, COLUMN_ID_ICON, QVariant());
            d->view->data()->updateRowEditBuffer(record, COLUMN_ID_CAPTION, QVariant(QString()));
            d->view->data()->updateRowEditBuffer(record, COLUMN_ID_DESC, QVariant());
            d->slotBeforeCellChanged_enabled = true;
            return;
        }

        KPropertySet *propertySetForRecord = d->sets->findPropertySetForItem(*record);
        if (!propertySetForRecord)
            return;

        KPropertySet &set = *propertySetForRecord; //propertySet();

        //'type' col is changed (existed before)
        //-get type group number
        KDbField::TypeGroup fieldTypeGroup;
        int i_fieldTypeGroup = newValue.toInt() + 1/*counting from 1*/;
        if (i_fieldTypeGroup < 1 || i_fieldTypeGroup >
#ifdef KEXI_NO_BLOB_FIELDS
//! @todo remove this later
                (int)KDbField::LastTypeGroup - 1) //don't show last (BLOB) type
#else
                (int)KDbField::LastTypeGroup)
#endif
            return;
        fieldTypeGroup = static_cast<KDbField::TypeGroup>(i_fieldTypeGroup);

        //-get 1st type from this group, and update 'type' property
        KDbField::Type fieldType = KDb::defaultTypeForGroup(fieldTypeGroup);
        if (fieldType == KDbField::InvalidType)
            fieldType = KDbField::Text;

        //-get subtypes for this type: keys (slist) and names (nlist)
        QStringList slist, nlist;
        getSubTypeListData(fieldTypeGroup, slist, nlist);

        QString subTypeValue;
        /* disabled - "mime" is moved from subType to "objectType" custom property
            if (fieldType==KDbField::BLOB) {
              // special case: BLOB type uses "mime-based" subtypes
              subTypeValue = slist.first();
            }
            else {*/
        subTypeValue = KDbField::typeString(fieldType);
        //}
        KProperty *subTypeProperty = &set["subType"];
        qDebug() << subTypeProperty->value();

        // *** this action contains subactions ***
        Command *changeDataTypeCommand = new Command(
            kundo2_i18n("Change data type for field \"%1\" to \"%2\"",
                 set["name"].value().toString(), KDbField::typeName(fieldType)), 0, this);

//qDebug() << "++++++++++" << slist << nlist;

        //update subtype list and value
        const bool forcePropertySetReload = KDbField::typeGroup(
            KDbField::typeForString(subTypeProperty->value().toString())) != fieldTypeGroup;   //<-- ?????
        const bool useListData = slist.count() > 1;

        if (!useListData) {
            slist.clear(); //empty list will be passed
            nlist.clear();
        }
        d->setPropertyValueIfNeeded(set, "type", (int)fieldType, changeDataTypeCommand,
                                    false /*!forceAddCommand*/, true /*rememberOldValue*/);

        // notNull and defaultValue=false is reasonable for boolean type
        if (fieldType == KDbField::Boolean) {
//! @todo maybe this is good for other data types as well?
            d->setPropertyValueIfNeeded(set, "notNull", QVariant(true), changeDataTypeCommand,
                                        false /*!forceAddCommand*/, false /*!rememberOldValue*/);
            d->setPropertyValueIfNeeded(set, "defaultValue", QVariant(false), changeDataTypeCommand,
                                        false /*!forceAddCommand*/, false /*!rememberOldValue*/);
        }
        if (set["primaryKey"].value().toBool() == true) {
            //primary keys require big int, so if selected type is not integer- remove PK
            if (fieldTypeGroup != KDbField::IntegerGroup) {
                /*not needed, line below will do the work
                d->view->data()->updateRowEditBuffer(record, COLUMN_ID_ICON, QVariant());
                d->view->data()->saveRowChanges(*record); */
                //set["primaryKey"] = QVariant(false);
                d->setPropertyValueIfNeeded(set, "primaryKey", QVariant(false), changeDataTypeCommand);
//! @todo should we display (passive?) dialog informing about cleared pkey?
            }
        }
        d->setPropertyValueIfNeeded(set, "subType", subTypeValue,
                                    changeDataTypeCommand, false, false /*!rememberOldValue*/,
                                    &slist, &nlist);

        if (d->updatePropertiesVisibility(fieldType, set, changeDataTypeCommand)
                || forcePropertySetReload) {
            //properties' visiblility changed: refresh prop. set
            propertySetReloaded(true);
        }

        addHistoryCommand(changeDataTypeCommand, false /* !execute */);
    } else if (colnum == COLUMN_ID_DESC) {//'description'
        KPropertySet *propertySetForRecord = d->sets->findPropertySetForItem(*record);
        if (!propertySetForRecord)
            return;
        //update field desc.
        QVariant oldValue((*propertySetForRecord)["description"].value());
        qDebug() << oldValue;
        propertySetForRecord->changeProperty("description", newValue);
    }
}

void KexiTableDesignerView::slotRowUpdated(KDbRecordData *record)
{
    const int row = d->view->data()->indexOf(record);
    if (row < 0)
        return;

    setDirty();

    //-check if the row was empty before updating
    //if yes: we want to add a property set for this new row (field)
    QString fieldCaption(record->at(COLUMN_ID_CAPTION).toString());
    const bool prop_set_allowed = !record->at(COLUMN_ID_TYPE).isNull();

    if (!prop_set_allowed && d->sets->at(row)/*propertySet()*/) {
        //there is a property set, but it's not allowed - remove it:
        d->sets->eraseAt(row);

        //clear 'type' column:
        d->view->data()->clearRowEditBuffer();
        d->view->data()->updateRowEditBuffer(record, COLUMN_ID_TYPE, QVariant());
        d->view->data()->saveRowChanges(*record);

    } else if (prop_set_allowed && !d->sets->at(row)/*propertySet()*/) {
        //-- create a new field:
        KDbField::TypeGroup fieldTypeGroup = static_cast<KDbField::TypeGroup>(
                    record->at(COLUMN_ID_TYPE).toInt() + 1/*counting from 1*/);
        int intFieldType = KDb::defaultTypeForGroup(fieldTypeGroup);
        if (intFieldType == 0)
            return;

        QString description(record->at(COLUMN_ID_DESC).toString());

//! @todo check uniqueness:
        QString fieldName(KDb::stringToIdentifier(fieldCaption));

        KDbField::Type fieldType = KDb::intToFieldType(intFieldType);
        uint maxLength = 0;
        if (fieldType == KDbField::Text) {
            maxLength = KDbField::defaultMaxLength();
        }

        KDbField field( //tmp
            fieldName,
            fieldType,
            KDbField::NoConstraints,
            KDbField::NoOptions,
            maxLength,
            /*precision*/0,
            /*defaultValue*/QVariant(),
            fieldCaption,
            description);

        // reasonable case for boolean type: set notNull flag and "false" as default value
        switch (fieldType) {
            case KDbField::Boolean:
                field.setNotNull(true);
                field.setDefaultValue(QVariant(false));
                break;
            case KDbField::Text:
                field.setMaxLengthStrategy(KDbField::DefaultMaxLength);
                break;
            default:;
        }

        qDebug() << field.debugString();

        //create a new property set:
        KPropertySet *newSet = createPropertySet(row, field, true);

        //refresh property editor:
        propertySetSwitched();

        if (row >= 0) {
            if (d->addHistoryCommand_in_slotRowUpdated_enabled) {
                addHistoryCommand(new InsertFieldCommand(0, this, row, *newSet /*propertySet()*/),    //, field /*will be copied*/
                                  false /* !execute */);
            }
        } else {
            qWarning() << "record # not found  !";
        }
    }
}

void KexiTableDesignerView::updateActions()
{
    updateActions(false);
}

void KexiTableDesignerView::slotPropertyChanged(KPropertySet& set, KProperty& property)
{
    const QByteArray pname(property.name());
    qDebug() << pname << " = " << property.value()
        << " (oldvalue = " << property.oldValue() << ")";

    // true is PK should be altered
    bool changePrimaryKey = false;
    // true is PK should be set to true, otherwise unset
    bool setPrimaryKey = false;

    if (pname == "primaryKey" && d->slotPropertyChanged_primaryKey_enabled) {
        changePrimaryKey = true;
        setPrimaryKey = property.value().toBool();
    }

    // update "lookup column" icon
    if (pname == "rowSource" || pname == "rowSourceType") {
//! @todo indicate invalid definitions of lookup columns as well using a special icon
//!       (e.g. due to missing data source)
        const int row = d->sets->findRowForPropertyValue("uid", set["uid"].value().toInt());
        KDbRecordData *record = d->view->itemAt(row);
        if (record)
            d->updateIconForRecord(*record, set);
    }

    //setting autonumber requires setting PK as well
    Command *setAutonumberCommand = 0;
    Command *toplevelCommand = 0;
    if (pname == "autoIncrement" && property.value().toBool() == true) {
        if (set["primaryKey"].value().toBool() == false) {//we need PKEY here!
            QString msg =
              xi18n("<para>Setting autonumber requires primary key to be set for current field.</para>");
            if (d->primaryKeyExists)
                msg += xi18n("<para>Previous primary key will be removed.</para>");
            msg += xi18n("<para>Do you want to create primary key for current field? "
                        "Click <interface>Cancel</interface> to cancel setting autonumber.</para>");

            if (KMessageBox::Yes == KMessageBox::questionYesNo(this, msg,
                xi18n("Setting Autonumber Field"),
                KGuiItem(xi18n("Create &Primary Key"), koIconName("key")), KStandardGuiItem::cancel()))
            {
                changePrimaryKey = true;
                setPrimaryKey = true;
                //switchPrimaryKey(set, true);
                // this will be toplevel command
                setAutonumberCommand = new Command(
                    kundo2_i18n("Set autonumber for field <resource>%1</resource>", set["name"].value().toString()), 0, this);
                toplevelCommand = setAutonumberCommand;
                d->setPropertyValueIfNeeded(set, "autoIncrement", QVariant(true), setAutonumberCommand);
            } else {
                setAutonumberCommand = new Command(
                    kundo2_i18n("Remove autonumber from field <resource>%1</resource>", set["name"].value().toString()),
                    0, this);
                d->setPropertyValueIfNeeded(set, "autoIncrement", QVariant(false), setAutonumberCommand,
                                            true /*forceAddCommand*/, false/*rememberOldValue*/);
                addHistoryCommand(setAutonumberCommand, false /* !execute */);
                return;
            }
        }
    }

    //clear PK when these properties were set to false:
    if ((pname == "indexed" || pname == "unique" || pname == "notNull")
            && set["primaryKey"].value().toBool() && property.value().toBool() == false) {
//! @todo perhaps show a hint in help panel telling what happens?
        changePrimaryKey = true;
        setPrimaryKey = false;
        // this will be toplevel command
        Command *unsetIndexedOrUniquOrNotNullCommand = new Command(
            kundo2_i18n("Set <resource>%1</resource> property for field <resource>%2</resource>",
                 property.caption(), set["name"].value().toString()), 0, this);
        toplevelCommand = unsetIndexedOrUniquOrNotNullCommand;
        d->setPropertyValueIfNeeded(set, pname, QVariant(false), unsetIndexedOrUniquOrNotNullCommand);
        if (pname == "notNull") {
            d->setPropertyValueIfNeeded(set, "unique", QVariant(false), unsetIndexedOrUniquOrNotNullCommand);
        }
    }

    if (pname == "defaultValue") {
        KDbField::Type type = KDb::intToFieldType(set["type"].value().toInt());
        set["defaultValue"].setType((KProperty::Type)KDbField::variantType(type));
    }

    if (pname == "subType" && d->slotPropertyChanged_subType_enabled) {
        d->slotPropertyChanged_subType_enabled = false;
        if (set["primaryKey"].value().toBool() == true
                && property.value().toString() != KDbField::typeString(KDbField::BigInteger))
        {
            qDebug() << "INVALID " << property.value().toString();
//   if (KMessageBox::Yes == KMessageBox::questionYesNo(this, msg,
//    xi18n("This field has primary key assigned. Setting autonumber field"),
//    KGuiItem(xi18n("Create &Primary Key"), koIconName("key")), KStandardGuiItem::cancel() ))
        }
        KDbField::Type type = KDb::intToFieldType(set["type"].value().toInt());
        QString typeName;
        /* disabled - "mime" is moved from subType to "objectType" custom property
            if (type==KDbField::BLOB) { //special case
              //find i18n'd text
              QStringList stringsList, namesList;
              getSubTypeListData(KDbField::BLOBGroup, stringsList, namesList);
              const int stringIndex = stringsList.findIndex( property.value().toString() );
              if (-1 == stringIndex || stringIndex>=(int)namesList.count())
                typeName = property.value().toString(); //for sanity
              else
                typeName = namesList[stringIndex];
            }
            else {*/
        typeName = KDbField::typeName(KDbField::typeForString(property.value().toString()));
        Command* changeFieldTypeCommand = new Command(
            kundo2_i18n(
                "Change type for field <resource>%1</resource> to <resource>%2</resource>",
                set["name"].value().toString(), typeName), 0, this);
        d->setPropertyValueIfNeeded(set, "subType", property.value(), property.oldValue(),
                                    changeFieldTypeCommand);

        qDebug() << set["type"].value();
        const KDbField::Type newType = KDbField::typeForString(property.value().toString());
        set["type"].setValue(newType);

        // cast "defaultValue" property value to a new type
        QVariant oldDefVal(set["defaultValue"].value());
        QVariant newDefVal(tryCastQVariant(oldDefVal, KDbField::variantType(type)));
        if (oldDefVal.type() != newDefVal.type())
            set["defaultValue"].setType(newDefVal.type());
        d->setPropertyValueIfNeeded(set, "defaultValue", newDefVal, newDefVal,
                                    changeFieldTypeCommand);

        d->updatePropertiesVisibility(newType, set);
        //properties' visiblility changed: refresh prop. set
        propertySetReloaded(true);
        d->slotPropertyChanged_subType_enabled = true;

        addHistoryCommand(changeFieldTypeCommand, false /* !execute */);
        return;
    }
    //! @todo add command text
    if (   d->addHistoryCommand_in_slotPropertyChanged_enabled
        && !changePrimaryKey/*we'll add multiple commands for PK*/)
    {
        addHistoryCommand(new ChangeFieldPropertyCommand(0, this, set,
                          property.name(), property.oldValue() /* ??? */, property.value()),
                          false /* !execute */);
    }

    if (changePrimaryKey) {
        d->slotPropertyChanged_primaryKey_enabled = false;
        if (setPrimaryKey) {
            //primary key implies some rules
            //this action contains subactions
            Command * setPrimaryKeyCommand = new Command(
                kundo2_i18n("Set primary key for field <resource>%1</resource>",
                     set["name"].value().toString()), toplevelCommand, this);
            if (!toplevelCommand) {
                 toplevelCommand = setPrimaryKeyCommand;
            }

            d->setPropertyValueIfNeeded(set, "primaryKey", QVariant(true), setPrimaryKeyCommand,
                                        true /*forceAddCommand*/);
            d->setPropertyValueIfNeeded(set, "unique", QVariant(true), setPrimaryKeyCommand);
            d->setPropertyValueIfNeeded(set, "notNull", QVariant(true), setPrimaryKeyCommand);
            d->setPropertyValueIfNeeded(set, "allowEmpty", QVariant(false), setPrimaryKeyCommand);
            d->setPropertyValueIfNeeded(set, "indexed", QVariant(true), setPrimaryKeyCommand);
//! \todo: add setting for this: "Integer PKeys have autonumber set by default"
            d->setPropertyValueIfNeeded(set, "autoIncrement", QVariant(true), setPrimaryKeyCommand);

            /*    set["unique"] = QVariant(true);
                    set["notNull"] = QVariant(true);
                    set["allowEmpty"] = QVariant(false);
                    set["indexed"] = QVariant(true);
                    set["autoIncrement"] = QVariant(true);*/
        }
        else { // set PK to false
            //remember this action containing 2 subactions
            Command *setPrimaryKeyCommand = new Command(
                kundo2_i18n("Unset primary key for field <resource>%1</resource>",
                     set["name"].value().toString()), toplevelCommand, this);
            if (!toplevelCommand) {
                toplevelCommand = setPrimaryKeyCommand;
            }
            d->setPropertyValueIfNeeded(set, "primaryKey", QVariant(false), setPrimaryKeyCommand,
                                        true /*forceAddCommand*/);
            d->setPropertyValueIfNeeded(set, "autoIncrement", QVariant(false), setPrimaryKeyCommand);
        }
        switchPrimaryKey(set, setPrimaryKey, true/*wasPKey*/, toplevelCommand);
        d->updatePropertiesVisibility(
            KDbField::typeForString(set["subType"].value().toString()), set, toplevelCommand);
        addHistoryCommand(toplevelCommand, false /* !execute */);
        //properties' visiblility changed: refresh prop. set
        propertySetReloaded(true/*preservePrevSelection*/);
        d->slotPropertyChanged_primaryKey_enabled = true;
    }
}

void KexiTableDesignerView::slotRowInserted()
{
    updateActions();

    if (d->addHistoryCommand_in_slotRowInserted_enabled) {
        const int row = d->view->currentRow();
        if (row >= 0) {
            addHistoryCommand(new InsertEmptyRowCommand(0, this, row), false /* !execute */);
        }
    }
    //! @todo
}

void KexiTableDesignerView::slotAboutToDeleteRow(
    KDbRecordData& record, KDbResultInfo* result, bool repaint)
{
    Q_UNUSED(result)
    Q_UNUSED(repaint)
    if (record[COLUMN_ID_ICON].toString() == "key")
        d->primaryKeyExists = false;

    if (d->addHistoryCommand_in_slotAboutToDeleteRow_enabled) {
        const int row = d->view->data()->indexOf(&record);
        KPropertySet *set = row >= 0 ? d->sets->at(row) : 0;
        //set can be 0 here, what means "removing empty row"
        addHistoryCommand(
            new RemoveFieldCommand(0, this, row, set),
            false /* !execute */
        );
    }
}

KDbField * KexiTableDesignerView::buildField(const KPropertySet &set) const
{
    //create a map of property values
    qDebug() << set["type"].value();
    QMap<QByteArray, QVariant> values(KPropertyValues(set));
    //remove internal values, to avoid creating custom field's properties
    KDbField *field = new KDbField();

    for (QMutableMapIterator<QByteArray, QVariant> it(values); it.hasNext();) {
        it.next();
        const QByteArray propName(it.key());
        if (d->internalPropertyNames.contains(propName)
                || propName.startsWith("this:")
                || (/*sanity*/propName == "objectType"
                    && KDbField::BLOB != KDb::intToFieldType(set["type"].value().toInt()))
           )
        {
            it.remove();
        }
    }
    //assign properties to the field
    // (note that "objectType" property will be saved as custom property)
    if (!KDb::setFieldProperties(*field, values)) {
        delete field;
        return 0;
    }
    return field;
}

tristate KexiTableDesignerView::buildSchema(KDbTableSchema &schema, bool beSilent)
{
    if (!d->view->acceptRowEdit())
        return cancelled;

    //check for missing captions
    KPropertySet *b = 0;
    bool no_fields = true;
    int i;
    QSet<QString> names;
    for (i = 0; i < (int)d->sets->size(); i++) {
        b = d->sets->at(i);
        if (b) {
            no_fields = false;
            const QString name((*b)["name"].value().toString().toLower());
            if (name.isEmpty()) {
                if (beSilent) {
                    qWarning() << QString("no field caption entered at record %1...").arg(i + 1);
                } else {
                    d->view->setCursorPosition(i, COLUMN_ID_CAPTION);
                    d->view->startEditCurrentCell();
                    KMessageBox::information(this, xi18n("You should enter field caption."));
                }
                return cancelled;
            }
            if (names.contains(name)) {
                break;
            }
            names.insert(name);   //remember
        }
    }

    //check for empty design
    if (no_fields) {
        if (beSilent) {
            qWarning() << "no field defined...";
        } else {
            KMessageBox::sorry(this,
                xi18n("You have added no fields.\nEvery table should have at least one field."));
        }
        return cancelled;
    }

    //check for duplicates
    if (b && i < (int)d->sets->size()) {
        if (beSilent) {
            qWarning() << QString("duplicated field name '%1'")
                .arg((*b)["name"].value().toString());
        } else {
            d->view->setCursorPosition(i, COLUMN_ID_CAPTION);
            d->view->startEditCurrentCell();
//! @todo for "names hidden" mode we won't get this error because user is unable to change names
            KMessageBox::sorry(this,
            xi18n(
               "You have added \"%1\" field name twice.\nField names cannot be repeated. "
               "Correct name of the field.",
               (*b)["name"].value().toString()));
        }
        return cancelled;
    }

    //check for pkey; automatically add a pkey if user wanted
    if (!d->primaryKeyExists) {
        if (beSilent) {
            qDebug() << "no primay key defined...";
        } else {
            const int questionRes = KMessageBox::questionYesNoCancel(this,
                xi18nc("@info",
                     "Table <resource>%1</resource> has no primary key defined."
                     "<para><note>Although a primary key is not required, it is needed "
                     "for creating relations between database tables. "
                     "Do you want a primary key to be automatically added now?</note></para>"
                     "<para>If you want to add a primary key by hand, press <interface>Cancel</interface> "
                     "to cancel saving table design.</para>", schema.name()),
                QString(),
                KGuiItem(xi18nc("Add Database Primary Key to a Table", "&Add Primary Key"), koIconName("key")),
                KGuiItem(xi18nc("Do Not Add Database Primary Key to a Table", "Do &Not Add"), KStandardGuiItem::no().icon()),
                KStandardGuiItem::cancel(),
                "autogeneratePrimaryKeysOnTableDesignSaving");
            if (questionRes == KMessageBox::Cancel) {
                return cancelled;
            }
            else if (questionRes == KMessageBox::Yes) {
                //-find unique name, starting with, "id", "id2", ....
                int i = 0;
                int idIndex = 1; //means "id"
                QString pkFieldName("id%1");
                KLocalizedString pkFieldCaption(kxi18nc("Identifier%1", "Id%1"));
                while (i < (int)d->sets->size()) {
                    KPropertySet *set = d->sets->at(i);
                    if (set) {
                        if (   (*set)["name"].value().toString()
                               == pkFieldName.arg(idIndex == 1 ? QString() : QString::number(idIndex))
                            || (*set)["caption"].value().toString()
                               == pkFieldCaption.subs(idIndex == 1 ? QString() : QString::number(idIndex)).toString()
                           )
                        {
                            //try next id index
                            i = 0;
                            idIndex++;
                            continue;
                        }
                    }
                    i++;
                }
                pkFieldName = pkFieldName.arg(idIndex == 1 ? QString() : QString::number(idIndex));
                //ok, add PK with such unique name
                d->view->insertEmptyRow(0);
                d->view->setCursorPosition(0, COLUMN_ID_CAPTION);
                d->view->data()->updateRowEditBuffer(d->view->selectedItem(), COLUMN_ID_CAPTION,
                    pkFieldCaption.subs(idIndex == 1 ? QString() : QString::number(idIndex)).toString()
                                                    );
                d->view->data()->updateRowEditBuffer(d->view->selectedItem(), COLUMN_ID_TYPE,
                    QVariant(KDbField::IntegerGroup - 1/*counting from 0*/));
                if (!d->view->data()->saveRowChanges(*d->view->selectedItem(), true)) {
                    return cancelled;
                }
                slotTogglePrimaryKey();
            }
        }
    }

    //we're ready...
    for (i = 0;i < (int)d->sets->size();++i) {//for every field create Field definition
        KPropertySet *s = d->sets->at(i);
        if (!s)
            continue;
        KDbField * f = buildField(*s);
        if (!f)
            continue; //hmm?
        schema.addField(f);
        if (   !(*s)["rowSource"].value().toString().isEmpty()
            && !(*s)["rowSourceType"].value().toString().isEmpty())
        {
            //add lookup column
            KDbLookupFieldSchema *lookupFieldSchema = new KDbLookupFieldSchema();
            lookupFieldSchema->rowSource().setTypeByName((*s)["rowSourceType"].value().toString());
            lookupFieldSchema->rowSource().setName((*s)["rowSource"].value().toString());
            lookupFieldSchema->setBoundColumn((*s)["boundColumn"].value().toInt());
//! @todo this is backward-compatible code for "single visible column" implementation
//!       for multiple columns, only the first is displayed, so there is a data loss is GUI is used
//!       -- special kproperty editor needed
            QList<uint> visibleColumns;
            const int visibleColumn = (*s)["visibleColumn"].value().toInt();
            if (visibleColumn >= 0)
                visibleColumns.append((uint)visibleColumn);
            lookupFieldSchema->setVisibleColumns(visibleColumns);
//! @todo support columnWidths(), columnHeadersVisible(), maximumListRows(), limitToList(), displayWidget()
            if (!schema.setLookupFieldSchema(f->name(), lookupFieldSchema)) {
                qWarning() << "!schema.setLookupFieldSchema()";
                delete lookupFieldSchema;
                return false;
            }
        }
    }
    return true;
}

//! @internal
//! A recursive function for copying alter table actions from undo/redo commands.
static void copyAlterTableActions(const KUndo2Command* command,
                                  KDbAlterTableHandler::ActionList &actions)
{
    for (int i = 0; i < command->childCount(); ++i) {
            copyAlterTableActions(command->child(i), actions);
    }

    const Command* cmd = dynamic_cast<const Command*>(command);
    if (!cmd) {
        qWarning() << "cmd is not of type 'Command'!";
        return;
    }
    KDbAlterTableHandler::ActionBase* action = cmd->createAction();
    //some commands can contain null actions, e.g. "set visibility" command
    if (action)
        actions.append(action);
}

tristate KexiTableDesignerView::buildAlterTableActions(
    KDbAlterTableHandler::ActionList &actions)
{
    actions.clear();
    qDebug()
        << d->history->count()
        << " top-level command(s) to process...";

    for (int i = 0; i < d->history->count(); ++i) {
      copyAlterTableActions(d->history->command(i), actions);
    }
    return true;
}

KDbObject* KexiTableDesignerView::storeNewData(const KDbObject& sdata,
                                                        KexiView::StoreNewDataOptions options,
                                                        bool *cancel)
{
    Q_ASSERT(cancel);
    if (tempData()->table || window()->schemaData()) //must not be
        return 0;

    //create table schema definition
    tempData()->table = new KDbTableSchema(sdata.name());
    tempData()->table->setName(sdata.name());
    tempData()->table->setCaption(sdata.caption());
    tempData()->table->setDescription(sdata.description());

    tristate res = buildSchema(*tempData()->table);
    *cancel = ~res;

    //FINALLY: create table:
    if (res == true) {
        //! @todo
        KDbConnection *conn = KexiMainWindowIface::global()->project()->dbConnection();
        res = conn->createTable(tempData()->table, options & KexiView::OverwriteExistingData);
        if (res == true) {
            res = KexiMainWindowIface::global()->project()->removeUserDataBlock(tempData()->table->id());
        }
        else {
            window()->setStatus(conn, "");
        }
    }

    if (res == true) {
        //we've current schema
        tempData()->tableSchemaChangedInPreviousView = true;
        d->history->clear();
    } else {
        delete tempData()->table;
        tempData()->table = 0;
    }
    return tempData()->table;
}

KDbObject* KexiTableDesignerView::copyData(const KDbObject& sdata,
                                                     KexiView::StoreNewDataOptions options,
                                                     bool *cancel)
{
    Q_ASSERT(cancel);
    Q_UNUSED(options);
    Q_UNUSED(cancel);

    if (!tempData()->table) {
        qWarning() << "Cannot copy data without source table (tempData()->table)";
        return 0;
    }
    KDbConnection *conn = KexiMainWindowIface::global()->project()->dbConnection();
    KDbTableSchema *copiedTable = conn->copyTable(*tempData()->table, sdata);
    if (!copiedTable) {
        return 0;
    }
    if (!KexiMainWindowIface::global()->project()->copyUserDataBlock(tempData()->table->id(),
                                                                     copiedTable->id()))
    {
        conn->dropTable(copiedTable);
        delete copiedTable;
        return 0;
    }
    return copiedTable;
}

tristate KexiTableDesignerView::storeData(bool dontAsk)
{
    if (!tempData()->table || !window()->schemaData()) {
        d->recentResultOfStoreData = false;
        return false;
    }

    KDbConnection *conn = KexiMainWindowIface::global()->project()->dbConnection();
    KDbAlterTableHandler *alterTableHandler = 0;
    KDbTableSchema *newTable = 0;

    //- create action list for the alter table handler
    KDbAlterTableHandler::ActionList actions;
    tristate res = buildAlterTableActions(actions);
//!< @todo this is temporary flag before we switch entirely to real alter table
    bool realAlterTableCanBeUsed = false;
    if (res == true) {
        alterTableHandler = new KDbAlterTableHandler(*conn);
        alterTableHandler->setActions(actions);

        if (!d->tempStoreDataUsingRealAlterTable) {
            //only compute requirements
            KDbAlterTableHandler::ExecutionArguments args;
            args.onlyComputeRequirements = true;
            (void)alterTableHandler->execute(tempData()->table->name(), args);
            res = args.result;
            if (   res == true
                && 0 == (args.requirements & (0xffff ^ KDbAlterTableHandler::SchemaAlteringRequired)))
            {
                realAlterTableCanBeUsed = true;
            }
        }
    }

    if (res == true) {
        res = KexiTablePart::askForClosingObjectsUsingTableSchema(
                  this, *conn, *tempData()->table,
                  xi18n("You are about to change the design of table \"%1\" "
                       "but following objects using this table are opened:",
                       tempData()->table->name()));
    }

    if (res == true) {
        if (!d->tempStoreDataUsingRealAlterTable && !realAlterTableCanBeUsed) {
//! @todo temp; remove this case:
            delete alterTableHandler;
            alterTableHandler = 0;
            // - inform about removing the current table and ask for confirmation
            if (!d->dontAskOnStoreData && !dontAsk) {
                bool emptyTable;
                const QString msg = d->messageForSavingChanges(emptyTable);
                if (!emptyTable) {
                    if (KMessageBox::No == KMessageBox::questionYesNo(this, msg))
                        res = cancelled;
                }
            }
            d->dontAskOnStoreData = false; //one-time use
            if (~res) {
                d->recentResultOfStoreData = res;
                return res;
            }
            // keep old behaviour:
            newTable = new KDbTableSchema();
            // copy the schema data
            static_cast<KDbObject&>(*newTable)
                = static_cast<KDbObject&>(*tempData()->table);
            res = buildSchema(*newTable);
            qDebug() << "BUILD SCHEMA:";
            newTable->debug();

            res = conn->alterTable(*tempData()->table, *newTable);
            if (res != true)
                window()->setStatus(conn, "");
        } else {
            KDbAlterTableHandler::ExecutionArguments args;
            newTable = alterTableHandler->execute(tempData()->table->name(), args);
            res = args.result;
            qDebug() << "ALTER TABLE EXECUTE: "
            << res.toString();
            if (true != res) {
                alterTableHandler->debugError();
                window()->setStatus(alterTableHandler, "");
            }
        }
    }
    if (res == true) {
        //change current schema
        tempData()->table = newTable;
        tempData()->tableSchemaChangedInPreviousView = true;
        d->history->clear();
    } else {
        delete newTable;
    }
    delete alterTableHandler;
    d->recentResultOfStoreData = res;
    return res;
}

tristate KexiTableDesignerView::simulateAlterTableExecution(QString *debugTarget)
{
#ifndef KEXI_NO_UNDOREDO_ALTERTABLE
# ifdef KEXI_DEBUG_GUI
    if (KexiMainWindowIface::global()->currentWindow() != window()) {
        //to avoid executing for multiple alter table views
        return false;
    }
    if (!tempData()->table || !window()->schemaData())
        return false;
    KDbConnection *conn = KexiMainWindowIface::global()->project()->dbConnection();
    KDbAlterTableHandler::ActionList actions;
    /*tristate res =*/ buildAlterTableActions(actions);
//! @todo result?
    KDbAlterTableHandler alterTableHandler(*conn);
    alterTableHandler.setActions(actions);
    KDbAlterTableHandler::ExecutionArguments args;
    if (debugTarget) {
        args.debugString = debugTarget;
    } else {
        args.simulate = true;
    }
    (void)alterTableHandler.execute(tempData()->table->name(), args);
    return args.result;
# else
    Q_UNUSED(debugTarget);
    return false;
# endif
#else
    Q_UNUSED(debugTarget);
    return false;
#endif
}

void KexiTableDesignerView::slotSimulateAlterTableExecution()
{
    (void)simulateAlterTableExecution(0);
}

tristate KexiTableDesignerView::executeRealAlterTable()
{
    d->tempStoreDataUsingRealAlterTable = true;
    d->recentResultOfStoreData = false;
    // will call KexiMainWindow::slotProjectSaveAs() and thus storeData():
    QMetaObject::invokeMethod(
        KexiMainWindowIface::global()->thisWidget(), "slotProjectSave");
    d->tempStoreDataUsingRealAlterTable = false;
    return d->recentResultOfStoreData;
}

KexiTablePart::TempData* KexiTableDesignerView::tempData() const
{
    return static_cast<KexiTablePart::TempData*>(window()->data());
}

#ifdef KEXI_DEBUG_GUI
void KexiTableDesignerView::debugCommand(const KUndo2Command* command, int nestingLevel)
{
    if (dynamic_cast<const Command*>(command)) {
        KDb::alterTableActionDebugGUI(
            dynamic_cast<const Command*>(command)->debugString(), nestingLevel);
    }
    else {
        KDb::alterTableActionDebugGUI(command->text().toString(), nestingLevel);
    }
    //show subcommands
    for (int i = 0; i < command->childCount(); ++i) {
        debugCommand(command->child(i), nestingLevel + 1);
    }
}
#endif

void KexiTableDesignerView::addHistoryCommand(KexiTableDesignerCommands::Command* command, bool execute)
{
#ifdef KEXI_NO_UNDOREDO_ALTERTABLE
    Q_UNUSED(command);
    Q_UNUSED(execute);
#else
# ifdef KEXI_DEBUG_GUI
    debugCommand(command, 0);
# endif
    if (!execute) {
        command->setRedoEnabled(false);
    }
    d->history->push(command);
    if (!execute) {
        command->setRedoEnabled(true);
    }
    updateUndoRedoActions();
#endif
}

void KexiTableDesignerView::updateUndoRedoActions()
{
#ifndef KEXI_NO_UNDOREDO_ALTERTABLE
    setAvailable("edit_undo", d->historyActionCollection->action("edit_undo")->isEnabled());
    setAvailable("edit_redo", d->historyActionCollection->action("edit_redo")->isEnabled());
#endif
}

void KexiTableDesignerView::slotUndo()
{
#ifndef KEXI_NO_UNDOREDO_ALTERTABLE
# ifdef KEXI_DEBUG_GUI
    KDb::alterTableActionDebugGUI(QString("UNDO:"));
# endif
    d->history->undo();
    updateUndoRedoActions();
#endif
}

void KexiTableDesignerView::slotRedo()
{
#ifndef KEXI_NO_UNDOREDO_ALTERTABLE
# ifdef KEXI_DEBUG_GUI
    KDb::alterTableActionDebugGUI(QString("REDO:"));
# endif
    d->history->redo();
    updateUndoRedoActions();
#endif
}

void KexiTableDesignerView::slotAboutToShowContextMenu()
{
    //update title
    QString title;
    if (propertySet()) {
        const KPropertySet &set = *propertySet();
        QString captionOrName(set["caption"].value().toString());
        if (captionOrName.isEmpty())
            captionOrName = set["name"].value().toString();
        title = xi18n("Table field \"%1\"", captionOrName);
    } else {
        title = xi18nc("Empty table row", "Empty Row");
    }
//! \todo replace lineedit with table_field icon
    d->view->setContextMenuTitle(koIcon("lineedit"), title);
}

QString KexiTableDesignerView::debugStringForCurrentTableSchema(tristate& result)
{
    KDbTableSchema tempTable;
    //copy schema data
    static_cast<KDbObject&>(tempTable)
        = static_cast<KDbObject&>(*tempData()->table);
    result = buildSchema(tempTable, true /*beSilent*/);
    if (true != result)
        return QString();
    return tempTable.debugString(false /*without name*/);
}

// -- low-level actions used by undo/redo framework

void KexiTableDesignerView::clearRow(int row, bool addCommand)
{
    if (!d->view->acceptRowEdit())
        return;
    KDbRecordData *record = d->view->itemAt(row);
    if (!record)
        return;
    //clear from prop. set
    d->sets->eraseAt(row);
    //clear row in table view (just clear value in COLUMN_ID_TYPE column)
    if (!addCommand) {
        d->addHistoryCommand_in_slotRowUpdated_enabled = false;
        d->addHistoryCommand_in_slotPropertyChanged_enabled = false;
        d->slotBeforeCellChanged_enabled = false;
    }
    d->view->data()->updateRowEditBuffer(record, COLUMN_ID_TYPE, QVariant());
    if (!addCommand) {
        d->addHistoryCommand_in_slotRowUpdated_enabled = true;
        d->addHistoryCommand_in_slotPropertyChanged_enabled = true;
        d->slotBeforeCellChanged_enabled = true;
    }
    d->view->data()->saveRowChanges(*record, true);
}

void KexiTableDesignerView::insertField(int row, const QString& caption, bool addCommand)
{
    insertFieldInternal(row, 0, caption, addCommand);
}

void KexiTableDesignerView::insertField(int row, KPropertySet& set, bool addCommand)
{
    insertFieldInternal(row, &set, QString(), addCommand);
}

void KexiTableDesignerView::insertFieldInternal(int row, KPropertySet* set, //const KDbField& field,
        const QString& caption, bool addCommand)
{
    if (set && (!set->contains("type") || !set->contains("caption"))) {
        qWarning() << "no 'type' or 'caption' property in set!";
        return;
    }
    if (!d->view->acceptRowEdit())
        return;
    KDbRecordData *record = d->view->itemAt(row);
    if (!record)
        return;
    if (!addCommand) {
        d->addHistoryCommand_in_slotRowUpdated_enabled = false;
        d->addHistoryCommand_in_slotPropertyChanged_enabled = false;
        d->slotBeforeCellChanged_enabled = false;
    }
    d->view->data()->updateRowEditBuffer(record, COLUMN_ID_CAPTION,
                         set ? (*set)["caption"].value() : QVariant(caption));
    d->view->data()->updateRowEditBuffer(record, COLUMN_ID_TYPE,
                         set ? (int)KDbField::typeGroup((*set)["type"].value().toInt()) - 1/*counting from 0*/
                         : (((int)KDbField::TextGroup) - 1)/*default type, counting from 0*/
                        );
    d->view->data()->updateRowEditBuffer(record, COLUMN_ID_DESC,
                         set ? (*set)["description"].value() : QVariant());
    if (!addCommand) {
        d->slotBeforeCellChanged_enabled = true;
    }
    //this will create a new property set:
    d->view->data()->saveRowChanges(*record);
    if (set) {
        KPropertySet *newSet = d->sets->at(row);
        if (newSet) {
            *newSet = *set; //deep copy
        } else {
            qWarning() << "!newSet, row==" << row;
        }
    }
    if (!addCommand) {
        d->addHistoryCommand_in_slotPropertyChanged_enabled = true;
        d->addHistoryCommand_in_slotRowUpdated_enabled = true;
    }
    d->view->updateRow(row);
    propertySetReloaded(true);
}

void KexiTableDesignerView::insertEmptyRow(int row, bool addCommand)
{
    if (!addCommand) {
        d->addHistoryCommand_in_slotRowInserted_enabled = false;
    }
    d->view->insertEmptyRow(row);
    if (!addCommand) {
        d->addHistoryCommand_in_slotRowInserted_enabled = true;
    }
}

void KexiTableDesignerView::deleteRow(int row, bool addCommand)
{
    KDbRecordData *record = d->view->itemAt(row);
    if (!record)
        return;
    if (!addCommand) {
        d->addHistoryCommand_in_slotAboutToDeleteRow_enabled = false;
    }
    const bool res = d->view->deleteItem(record);
    if (!addCommand) {
        d->addHistoryCommand_in_slotAboutToDeleteRow_enabled = true;
    }
    if (!res)
        return;
}

void KexiTableDesignerView::changeFieldPropertyForRow(int row,
        const QByteArray& propertyName, const QVariant& newValue,
        KProperty::ListData* const listData, bool addCommand)
{
#ifdef KEXI_DEBUG_GUI
    KDb::alterTableActionDebugGUI(QString("** changeFieldProperty: \"")
                                     + QString(propertyName) + "\" to \""
                                     + newValue.toString() + "\"", 2/*nestingLevel*/);
#endif
    if (!d->view->acceptRowEdit())
        return;

    KPropertySet* set = d->sets->at(row);
    if (!set || !set->contains(propertyName))
        return;
    KProperty &property = set->property(propertyName);
    if (listData) {
        if (listData->keys.isEmpty())
            property.setListData(0);
        else
            property.setListData(new KProperty::ListData(*listData));
    }
    if (propertyName != "type") //delayed type update (we need to have subtype set properly)
        property.setValue(newValue);
    KDbRecordData *record = d->view->itemAt(row);
    Q_ASSERT(record);

    if (propertyName == "type") {
        d->slotPropertyChanged_subType_enabled = false;
        d->view->data()->updateRowEditBuffer(record, COLUMN_ID_TYPE,
                                             int(KDbField::typeGroup(newValue.toInt())) - 1);
        d->view->data()->saveRowChanges(*record);
        d->addHistoryCommand_in_slotRowUpdated_enabled = true;
        property.setValue(newValue); //delayed type update (we needed to have subtype set properly)
    }

    if (!addCommand) {
        d->addHistoryCommand_in_slotRowUpdated_enabled = false;
        d->addHistoryCommand_in_slotPropertyChanged_enabled = false;
        d->slotPropertyChanged_subType_enabled = false;
    }
    //special cases: properties displayed within the data grid:
    if (propertyName == "caption") {
        if (!addCommand) {
            d->slotBeforeCellChanged_enabled = false;
        }
        d->view->data()->updateRowEditBuffer(record, COLUMN_ID_CAPTION, newValue);
        d->view->data()->saveRowChanges(*record);
        if (!addCommand) {
            d->slotBeforeCellChanged_enabled = true;
        }
    } else if (propertyName == "description") {
        if (!addCommand) {
            d->slotBeforeCellChanged_enabled = false;
        }
        d->view->data()->updateRowEditBuffer(record, COLUMN_ID_DESC, newValue);
        if (!addCommand) {
            d->slotBeforeCellChanged_enabled = true;
        }
        d->view->data()->saveRowChanges(*record);
    }
    if (!addCommand) {
        d->addHistoryCommand_in_slotPropertyChanged_enabled = true;
        d->addHistoryCommand_in_slotRowUpdated_enabled = true;
        d->slotPropertyChanged_subType_enabled = true;
    }
    d->view->updateRow(row);
}

void KexiTableDesignerView::changeFieldProperty(int fieldUID,
        const QByteArray& propertyName, const QVariant& newValue,
        KProperty::ListData* const listData, bool addCommand)
{
    //find a property by UID
    const int row = d->sets->findRowForPropertyValue("uid", fieldUID);
    if (row < 0) {
        qWarning() << "field with uid=" << fieldUID << " not found!";
        return;
    }
    changeFieldPropertyForRow(row, propertyName, newValue, listData, addCommand);
}

void KexiTableDesignerView::changePropertyVisibility(
    int fieldUID, const QByteArray& propertyName, bool visible)
{
#ifdef KEXI_DEBUG_GUI
    KDb::alterTableActionDebugGUI(QString("** changePropertyVisibility: \"")
                                     + QString(propertyName) + "\" to \""
                                     + (visible ? "true" : "false") + "\"", 2/*nestingLevel*/);
#endif
    if (!d->view->acceptRowEdit())
        return;

    //find a property by name
    const int row = d->sets->findRowForPropertyValue("uid", fieldUID);
    if (row < 0)
        return;
    KPropertySet* set = d->sets->at(row);
    if (!set || !set->contains(propertyName))
        return;

    KProperty &property = set->property(propertyName);
    if (property.isVisible() != visible) {
        property.setVisible(visible);
        propertySetReloaded(true);
    }
}

void KexiTableDesignerView::propertySetSwitched()
{
    KexiDataTableView::propertySetSwitched();
    KexiLookupColumnPage *page = qobject_cast<KexiTablePart*>(window()->part())->lookupColumnPage();
    if (page)
        page->assignPropertySet(propertySet());
}

bool KexiTableDesignerView::isPhysicalAlteringNeeded()
{
    //- create action list for the alter table handler
    KDbAlterTableHandler::ActionList actions;
    tristate res = buildAlterTableActions(actions);
    if (res != true)
        return true;

    KDbConnection *conn = KexiMainWindowIface::global()->project()->dbConnection();
    KDbAlterTableHandler *alterTableHandler = new KDbAlterTableHandler(*conn);
    alterTableHandler->setActions(actions);

    //only compute requirements
    KDbAlterTableHandler::ExecutionArguments args;
    args.onlyComputeRequirements = true;
    (void)alterTableHandler->execute(tempData()->table->name(), args);
    res = args.result;
    delete alterTableHandler;
    if (   res == true
        && 0 == (args.requirements & (0xffff ^ KDbAlterTableHandler::SchemaAlteringRequired)))
    {
        return false;
    }
    return true;
}

