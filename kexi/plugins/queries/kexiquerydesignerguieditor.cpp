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

#include "kexiquerydesignerguieditor.h"
#include <kexiutils/utils.h>
#include <kexiproject.h>
#include <KexiMainWindowIface.h>
#include <kexiinternalpart.h>
/*! @todo KEXI3 Port #include <kexidragobjects.h>
#include <widget/tableview/KexiTableScrollArea.h>
#include <widget/tableview/KexiDataTableView.h>
#include <kexi.h>
#include <kexisectionheader.h>
#include <widget/dataviewcommon/kexidataawarepropertyset.h>
#include <widget/relations/KexiRelationsView.h>
#include <widget/relations/KexiRelationsTableContainer.h>
#include "kexiquerypart.h"
#include "kexiqueryview.h"
#include <KexiWindow.h>
#include <KexiWindowData.h>
#include <kexi_global.h>

#include <KDbField>
#include <KDbQuerySchema>
#include <KDbConnection>
#include <KDbParser>
#include <KDbUtils>
#include <KDbRecordEditBuffer>
#include <KDbTableViewData>
#include <KDb>
#include <KDbExpression>

#include <KProperty>
#include <KPropertySet>

#include <KMessageBox>
#include <KLocalizedString>

#include <QDomDocument>
#include <QRegExp>
#include <QSplitter>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QSet>
#include <QLocale>
#include <QDebug>

//! @todo remove KEXI_NO_QUERY_TOTALS later
#define KEXI_NO_QUERY_TOTALS

//! indices for table columns
#define COLUMN_ID_COLUMN 0
#define COLUMN_ID_TABLE 1
#define COLUMN_ID_VISIBLE 2
#ifdef KEXI_NO_QUERY_TOTALS
# define COLUMN_ID_SORTING 3
# define COLUMN_ID_CRITERIA 4
#else
# define COLUMN_ID_TOTALS 3
# define COLUMN_ID_SORTING 4
# define COLUMN_ID_CRITERIA 5
#endif

/*! @internal */
class KexiQueryDesignerGuiEditor::Private
{
public:
    Private(KexiQueryDesignerGuiEditor *p)
     : q(p)
    {
        droppedNewRecord = 0;
        slotTableAdded_enabled = true;
        sortColumnPreferredWidth = 0;
    }

    bool changeSingleCellValue(KDbRecordData *record, int columnNumber,
                               const QVariant& value, KDbResultInfo* result) {
        data->clearRecordEditBuffer();
        if (!data->updateRecordEditBuffer(record, columnNumber, value)
                || !data->saveRecordChanges(record, true)) {
            if (result)
                *result = data->result();
            return false;
        }
        return true;
    }

    KexiQueryDesignerGuiEditor *q;
    KDbTableViewData *data;
    KexiDataTableView *dataTable;
    QPointer<KDbConnection> conn;

    KexiRelationsView *relations;
    KexiSectionHeader *head;
    QSplitter *spl;

    /*! Used to remember in slotDroppedAtRow() what data was dropped,
     so we can create appropriate prop. set in slotRowInserted()
     This information is cached and entirely refreshed on updateColumnsData(). */
    KDbTableViewData *fieldColumnData, *tablesColumnData;

    /*! Collects identifiers selected in 1st (field) column,
     so we're able to distinguish between table identifiers selected from
     the dropdown list, and strings (e.g. expressions) entered by hand.
     This information is cached and entirely refreshed on updateColumnsData().
     The dict is filled with (char*)1 values (doesn't matter what it is);
    */
    QSet<QString> fieldColumnIdentifiers;

    void addFieldColumnIdentifier(const QString& id)
    {
        fieldColumnIdentifiers.insert(id.toLower());
    }

    int comboArrowWidth;
    int sortColumnPreferredWidth;

    void initSortColumnPreferredWidth(const QVector<QString> &items)
    {
        int maxw = -1;
        foreach (const QString &text, items) {
            maxw = qMax(maxw, q->fontMetrics().width(text + " "));
        }
        sortColumnPreferredWidth = maxw + KexiUtils::comboBoxArrowSize(q->style()).width();
    }

    KexiDataAwarePropertySet* sets;
    KDbRecordData *droppedNewRecord;

    QString droppedNewTable, droppedNewField;

    bool slotTableAdded_enabled;
};

static bool isAsterisk(const QString& tableName, const QString& fieldName)
{
    return tableName == "*" || fieldName.endsWith('*');
}

//! @internal \return true if sorting is allowed for \a fieldName and \a tableName
static bool sortingAllowed(const QString& fieldName, const QString& tableName)
{
    return !(fieldName == "*" || (fieldName.isEmpty() && tableName == "*"));
}

//=========================================================

KexiQueryDesignerGuiEditor::KexiQueryDesignerGuiEditor(
    QWidget *parent)
        : KexiView(parent)
        , d(new Private(this))
{
    d->conn = KexiMainWindowIface::global()->project()->dbConnection();

    d->spl = new QSplitter(Qt::Vertical, this);
    d->spl->setChildrenCollapsible(false);
    d->relations = new KexiRelationsView(d->spl);
    d->spl->addWidget(d->relations);
    d->relations->setObjectName("relations");
    connect(d->relations, SIGNAL(tableAdded(KDbTableSchema&)),
            this, SLOT(slotTableAdded(KDbTableSchema&)));
    connect(d->relations, SIGNAL(tableHidden(KDbTableSchema&)),
            this, SLOT(slotTableHidden(KDbTableSchema&)));
    connect(d->relations, SIGNAL(appendFields(KDbTableOrQuerySchema&,QStringList)),
            this, SLOT(slotAppendFields(KDbTableOrQuerySchema&,QStringList)));

    d->head = new KexiSectionHeader(xi18n("Query Columns"), Qt::Vertical, d->spl);
    d->spl->addWidget(d->head);
    d->dataTable = new KexiDataTableView(d->head, false);
    d->head->setWidget(d->dataTable);
    d->dataTable->setObjectName("guieditor_dataTable");
    d->dataTable->dataAwareObject()->setSpreadSheetMode(true);

    d->data = new KDbTableViewData(); //just empty data
    d->sets = new KexiDataAwarePropertySet(this, d->dataTable->dataAwareObject());
    connect(d->sets, SIGNAL(propertyChanged(KPropertySet&,KProperty&)),
            this, SLOT(slotPropertyChanged(KPropertySet&,KProperty&)));

    initTableColumns();
    initTableRows();

    QList<int> c;
    c << COLUMN_ID_COLUMN << COLUMN_ID_TABLE << COLUMN_ID_CRITERIA;
    if (d->dataTable->tableView()/*sanity*/) {
        d->dataTable->tableView()->adjustColumnWidthToContents(COLUMN_ID_VISIBLE);
        d->dataTable->tableView()->setColumnWidth(COLUMN_ID_SORTING, d->sortColumnPreferredWidth);
        d->dataTable->tableView()->setStretchLastColumn(true);
        d->dataTable->tableView()->maximizeColumnsWidth(c);
        d->dataTable->tableView()->setDropsAtRowEnabled(true);
        connect(d->dataTable->tableView(), SIGNAL(dragOverRow(KDbRecordData*,int,QDragMoveEvent*)),
                this, SLOT(slotDragOverTableRow(KDbRecordData*,int,QDragMoveEvent*)));
        connect(d->dataTable->tableView(), SIGNAL(droppedAtRow(KDbRecordData*,int,QDropEvent*,KDbRecordData*&)),
                this, SLOT(slotDroppedAtRow(KDbRecordData*,int,QDropEvent*,KDbRecordData*&)));
        connect(d->dataTable->tableView(), SIGNAL(newItemAppendedForAfterDeletingInSpreadSheetMode()),
                this, SLOT(slotNewItemAppendedForAfterDeletingInSpreadSheetMode()));
    }
    connect(d->data, SIGNAL(aboutToChangeCell(KDbRecordData*,int,QVariant&,KDbResultInfo*)),
            this, SLOT(slotBeforeCellChanged(KDbRecordData*,int,QVariant&,KDbResultInfo*)));
    connect(d->data, SIGNAL(rowInserted(KDbRecordData*,uint,bool)),
            this, SLOT(slotRowInserted(KDbRecordData*,uint,bool)));
    connect(d->relations, SIGNAL(tablePositionChanged(KexiRelationsTableContainer*)),
            this, SLOT(slotTablePositionChanged(KexiRelationsTableContainer*)));
    connect(d->relations, SIGNAL(aboutConnectionRemove(KexiRelationsConnection*)),
            this, SLOT(slotAboutConnectionRemove(KexiRelationsConnection*)));

    addChildView(d->relations);
    addChildView(d->dataTable);
    setViewWidget(d->spl, false/* no focus proxy*/);
    setFocusProxy(d->dataTable);
    d->relations->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    d->head->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    updateGeometry();
    d->spl->setSizes(QList<int>() << 800 << 400);
}

KexiQueryDesignerGuiEditor::~KexiQueryDesignerGuiEditor()
{
    delete d;
}

void
KexiQueryDesignerGuiEditor::initTableColumns()
{
    KDbTableViewColumn *col1 = new KDbTableViewColumn("column", KDbField::Enum, xi18n("Column"),
            xi18n("Describes field name or expression for the designed query."));
    col1->setRelatedDataEditable(true);

    d->fieldColumnData = new KDbTableViewData(KDbField::Text, KDbField::Text);
    col1->setRelatedData(d->fieldColumnData);
    d->data->addColumn(col1);

    KDbTableViewColumn *col2 = new KDbTableViewColumn("table", KDbField::Enum, xi18n("Table"),
            xi18n("Describes table for a given field. Can be empty."));
    d->tablesColumnData = new KDbTableViewData(KDbField::Text, KDbField::Text);
    col2->setRelatedData(d->tablesColumnData);
    d->data->addColumn(col2);

    KDbTableViewColumn *col3 = new KDbTableViewColumn("visible", KDbField::Boolean, xi18n("Visible"),
            xi18n("Describes visibility for a given field or expression."));
    col3->field()->setDefaultValue(QVariant(false));
    col3->field()->setNotNull(true);
    d->data->addColumn(col3);

#ifndef KEXI_NO_QUERY_TOTALS
    KDbTableViewColumn *col4 = new KDbTableViewColumn("totals", KDbField::Enum, futureI18n("Totals"),
            futureI18n("Describes a way of computing totals for a given field or expression."));
    QVector<QString> totalsTypes;
    totalsTypes.append(futureI18n("Group by"));
    totalsTypes.append(futureI18n("Sum"));
    totalsTypes.append(futureI18n("Average"));
    totalsTypes.append(futureI18n("Min"));
    totalsTypes.append(futureI18n("Max"));
    //! @todo more like this
    col4->field()->setEnumHints(totalsTypes);
    d->data->addColumn(col4);
#endif

    KDbTableViewColumn *col5 = new KDbTableViewColumn("sort", KDbField::Enum, xi18n("Sorting"),
            xi18n("Describes a way of sorting for a given field."));
    QVector<QString> sortTypes;
    sortTypes.append("");
    sortTypes.append(xi18n("Ascending"));
    sortTypes.append(xi18n("Descending"));
    col5->field()->setEnumHints(sortTypes);
    d->data->addColumn(col5);
    d->initSortColumnPreferredWidth(sortTypes);

    KDbTableViewColumn *col6 = new KDbTableViewColumn("criteria", KDbField::Text, xi18n("Criteria"),
            xi18n("Describes the criteria for a given field or expression."));
    d->data->addColumn(col6);
}

void KexiQueryDesignerGuiEditor::initTableRows()
{
    d->data->deleteAllRecords();
    for (int i = 0; i < (int)d->sets->size(); i++) {
        KDbRecordData* record;
        d->data->append(record = d->data->createItem());
        (*record)[COLUMN_ID_VISIBLE] = QVariant(false);
    }
    d->dataTable->dataAwareObject()->setData(d->data);

    updateColumnsData();
}

void KexiQueryDesignerGuiEditor::updateColumnsData()
{
    d->dataTable->dataAwareObject()->acceptRowEdit();

    QStringList sortedTableNames;
    foreach(KexiRelationsTableContainer* cont, *d->relations->tables()) {
        sortedTableNames += cont->schema()->name();
    }
    qSort(sortedTableNames);

    //several tables can be hidden now, so remove rows for these tables
    QList<int> rowsToDelete;
    for (int r = 0; r < (int)d->sets->size(); r++) {
        KPropertySet *set = d->sets->at(r);
        if (set) {
            QString tableName = (*set)["table"].value().toString();
            QString fieldName = (*set)["field"].value().toString();
            const bool allTablesAsterisk = tableName == "*" && d->relations->tables()->isEmpty();
            const bool fieldNotFound = tableName != "*"
                                       && !(*set)["isExpression"].value().toBool()
                                       && sortedTableNames.end() == qFind(sortedTableNames.begin(), sortedTableNames.end(), tableName);

            if (allTablesAsterisk || fieldNotFound) {
                //table not found: mark this line for later removal
                rowsToDelete += r;
            }
        }
    }
    d->data->deleteRows(rowsToDelete);

    //update 'table' and 'field' columns
    d->tablesColumnData->deleteAllRecords();
    d->fieldColumnData->deleteAllRecords();
    d->fieldColumnIdentifiers.clear();

    KDbRecordData *record = d->fieldColumnData->createItem();
    (*record)[COLUMN_ID_COLUMN] = "*";
    (*record)[COLUMN_ID_TABLE] = "*";
    d->fieldColumnData->append(record);
    d->addFieldColumnIdentifier((*record)[COLUMN_ID_COLUMN].toString()); //cache

    tempData()->unregisterForTablesSchemaChanges();
    foreach(const QString& tableName, sortedTableNames) {
        //table
        /*! @todo what about query? */
        KDbTableSchema *table = d->relations->tables()->value(tableName)->schema()->table();
        d->conn->registerForTableSchemaChanges(*tempData(), *table); //this table will be used
        record = d->tablesColumnData->createItem();
        (*record)[COLUMN_ID_COLUMN] = table->name();
        (*record)[COLUMN_ID_TABLE] = (*record)[COLUMN_ID_COLUMN];
        d->tablesColumnData->append(record);
        //fields
        record = d->fieldColumnData->createItem();
        (*record)[COLUMN_ID_COLUMN] = QString(table->name() + ".*");
        (*record)[COLUMN_ID_TABLE] = (*record)[COLUMN_ID_COLUMN];
        d->fieldColumnData->append(record);
        d->addFieldColumnIdentifier((*record)[COLUMN_ID_COLUMN].toString()); //cache
        foreach(KDbField *field, *table->fields()) {
            record = d->fieldColumnData->createItem();
            (*record)[COLUMN_ID_COLUMN] = QString(table->name() + '.' + field->name());
            (*record)[COLUMN_ID_TABLE] = QString("  " + field->name());
            d->fieldColumnData->append(record);
            d->addFieldColumnIdentifier((*record)[COLUMN_ID_COLUMN].toString()); //cache
        }
    }
//! @todo
}

KexiRelationsView *KexiQueryDesignerGuiEditor::relationsView() const
{
    return d->relations;
}

KexiQueryPart::TempData *
KexiQueryDesignerGuiEditor::tempData() const
{
    return static_cast<KexiQueryPart::TempData*>(window()->data());
}

static QString msgCannotSwitch_EmptyDesign()
{
    return xi18n("Cannot switch to data view, because query design is empty.\n"
                "First, please create your design.");
}

bool
KexiQueryDesignerGuiEditor::buildSchema(QString *errMsg)
{
    //build query schema
    KexiQueryPart::TempData * temp = tempData();
    if (temp->query()) {
        KexiView *queryDataView = window()->viewForMode(Kexi::DataViewMode);
        if (queryDataView) {
            dynamic_cast<KexiQueryView*>(queryDataView)->setData(0);
        }
        temp->clearQuery();
    } else {
        temp->setQuery(new KDbQuerySchema());
    }

    //add tables
    foreach(KexiRelationsTableContainer* cont, *d->relations->tables()) {
        /*! @todo what about query? */
        temp->query()->addTable(cont->schema()->table());
    }

    //add fields, also build:
    // -WHERE expression
    // -ORDER BY list
    KDbExpression *whereExpr = 0;
    const uint count = qMin(d->data->count(), d->sets->size());
    bool fieldsFound = false;
    KDbTableViewDataIterator it(d->data->begin());
    for (uint i = 0; i < count && it != d->data->end(); ++it, i++) {
        if (!(**it)[COLUMN_ID_TABLE].isNull()
                && (**it)[COLUMN_ID_COLUMN].isNull()) {
            //show message about missing field name, and set focus to that cell
            qDebug() << "no field provided!";
            d->dataTable->dataAwareObject()->setCursorPosition(i, 0);
            if (errMsg)
                *errMsg = xi18n("Select column for table \"%1\"",
                               (**it)[COLUMN_ID_TABLE].toString());
            return false;
        }

        KPropertySet *set = d->sets->at(i);
        if (set) {
            QString tableName = (*set)["table"].value().toString().trimmed();
            QString fieldName = (*set)["field"].value().toString();
            QString fieldAndTableName = fieldName;
            KDbField *currentField = 0; // will be set if this column is a single field
            if (!tableName.isEmpty())
                fieldAndTableName.prepend(tableName + ".");
            const bool fieldVisible = (*set)["visible"].value().toBool();
            QString criteriaStr = (*set)["criteria"].value().toString();
            QByteArray alias((*set)["alias"].value().toByteArray());
            if (!criteriaStr.isEmpty()) {
                KDbToken token;
                KDbExpression *criteriaExpr = parseExpressionString(criteriaStr, &token,
                                                 true/*allowRelationalOperator*/);
                if (!criteriaExpr) {//for sanity
                    if (errMsg)
                        *errMsg = xi18n("Invalid criteria \"%1\"", criteriaStr);
                    delete whereExpr;
                    return false;
                }
                //build relational expression for column variable
                KDbVariableExpression *varExpr = new KDbVariableExpression(fieldAndTableName);
                criteriaExpr = new KDbBinaryExpression(varExpr, token, criteriaExpr);
                //critera ok: add it to WHERE section
                if (whereExpr)
                    whereExpr = new KDbBinaryExpression(whereExpr, KDbToken::AND, criteriaExpr);
                else //first expr.
                    whereExpr = criteriaExpr;
            }
            if (tableName.isEmpty()) {
                if ((*set)["isExpression"].value().toBool() == true) {
                    //add expression column
                    int dummyToken;
                    KDbExpression *columnExpr = parseExpressionString(fieldName, dummyToken,
                                                   false/*!allowRelationalOperator*/);
                    if (!columnExpr) {
                        if (errMsg)
                            *errMsg = xi18n("Invalid expression \"%1\"", fieldName);
                        return false;
                    }
                    temp->query()->addExpression(columnExpr, fieldVisible);
                    if (fieldVisible)
                        fieldsFound = true;
                    if (!alias.isEmpty())
                        temp->query()->setColumnAlias(temp->query()->fieldCount() - 1, alias);
                }
                //! @todo
            } else if (tableName == "*") {
                //all tables asterisk
                temp->query()->addAsterisk(new KDbQueryAsterisk(temp->query(), 0), fieldVisible);
                if (fieldVisible)
                    fieldsFound = true;
                continue;
            } else {
                KDbTableSchema *t = d->conn->tableSchema(tableName);
                if (fieldName == "*") {
                    //single-table asterisk: <tablename> + ".*" + number
                    temp->query()->addAsterisk(new KDbQueryAsterisk(temp->query(), t), fieldVisible);
                    if (fieldVisible)
                        fieldsFound = true;
                } else {
                    if (!t) {
                        qWarning() << "query designer: NO TABLE '"
                        << (*set)["table"].value().toString() << "'";
                        continue;
                    }
                    currentField = t->field(fieldName);
                    if (!currentField) {
                        qWarning() << "query designer: NO FIELD '" << fieldName << "'";
                        continue;
                    }
                    if (!fieldVisible && criteriaStr.isEmpty() && set->contains("isExpression")
                            && (*set)["sorting"].value().toString() != "nosorting") {
                        qDebug() << "invisible field with sorting: do not add it to the fields list";
                        continue;
                    }
                    temp->query()->addField(currentField, fieldVisible);
                    if (fieldVisible)
                        fieldsFound = true;
                    if (!alias.isEmpty())
                        temp->query()->setColumnAlias(temp->query()->fieldCount() - 1, alias);
                }
            }
        } else {//!set
            //qDebug() << (**it)[COLUMN_ID_TABLE].toString();
        }
    }
    if (!fieldsFound) {
        if (errMsg)
            *errMsg = msgCannotSwitch_EmptyDesign();
        return false;
    }
    if (whereExpr) {
        qDebug() << "setting CRITERIA:" << *whereExpr;
    }

    //set always, because if whereExpr==NULL,
    //this will clear prev. expr
    temp->query()->setWhereExpression(whereExpr);

    //add relations (looking for connections)
    foreach(KexiRelationsConnection* conn, *d->relations->connections()) {
        KexiRelationsTableContainer *masterTable = conn->masterTable();
        KexiRelationsTableContainer *detailsTable = conn->detailsTable();

        /*! @todo what about query? */
        temp->query()->addRelationship(
            masterTable->schema()->table()->field(conn->masterField()),
            detailsTable->schema()->table()->field(conn->detailsField()));
    }

    // Add sorting information (ORDER BY) - we can do that only now
    //  after all KDbQueryColumnInfo items are instantiated
    KDbOrderByColumnList orderByColumns;
    it = d->data->constBegin();
    int fieldNumber = -1; //field number (empty rows are omitted)
    for (uint i = 0/*row number*/; i < count && it != d->data->constEnd(); ++it, i++) {
        KPropertySet *set = d->sets->at(i);
        if (!set)
            continue;
        fieldNumber++;
        KDbField *currentField = 0;
        KDbQueryColumnInfo *currentColumn = 0;
        QString sortingString((*set)["sorting"].value().toString());
        if (sortingString != "ascending" && sortingString != "descending")
            continue;
        if (!(*set)["visible"].value().toBool()) {
            // this row defines invisible field but contains sorting information,
            // what means KDbField should be used as a reference for this sorting
            // Note1: alias is not supported here.

            // Try to find a field (not mentioned after SELECT):
            currentField = temp->query()->findTableField((*set)["field"].value().toString());
            if (!currentField) {
                qWarning() << "NO FIELD"
                    << (*set)["field"].value().toString()
                    << "available for sorting";
                continue;
            }
            orderByColumns.appendField(*currentField, sortingString == "ascending");
            continue;
        }
        currentField = temp->query()->field((uint)fieldNumber);
        if (!currentField || currentField->isExpression() || currentField->isQueryAsterisk())
//! @todo support expressions here
            continue;
//! @todo ok, but not for expressions
        QString aliasString((*set)["alias"].value().toString());
        currentColumn = temp->query()->columnInfo(
                            (*set)["table"].value().toString() + "."
                            + (aliasString.isEmpty() ? currentField->name() : aliasString));
        if (currentField && currentColumn) {
            if (currentColumn->visible)
                orderByColumns.appendColumn(*currentColumn, sortingString == "ascending");
            else if (currentColumn->field)
                orderByColumns.appendField(*currentColumn->field, sortingString == "ascending");
        }
    }
    temp->query()->setOrderByColumnList(orderByColumns);

    qDebug() << *temp->query();
    temp->registerTableSchemaChanges(temp->query());
    //! @todo ?
    return true;
}

tristate
KexiQueryDesignerGuiEditor::beforeSwitchTo(Kexi::ViewMode mode, bool *dontStore)
{
    Q_ASSERT(dontStore);
    qDebug() << mode;

    if (!d->dataTable->dataAwareObject()->acceptRowEdit())
        return cancelled;

    qDebug() << "queryChangedInPreviousView:" << tempData()->queryChangedInPreviousView();

    if (mode == Kexi::DesignViewMode) {
        return true;
    }
    else if (mode == Kexi::DataViewMode) {
        if (!isDirty() && window()->neverSaved()) {
            KMessageBox::information(this, msgCannotSwitch_EmptyDesign());
            return cancelled;
        }
        if (tempData()->queryChangedInPreviousView() || !tempData()->query()) {
            //remember current design in a temporary structure
            *dontStore = true;
            QString errMsg;
            //build schema; problems are not allowed
            if (!buildSchema(&errMsg)) {
                KMessageBox::sorry(this, errMsg);
                return cancelled;
            }
        }
        dontStore = true;
        //! @todo
        return true;
    } else if (mode == Kexi::TextViewMode) {
        *dontStore = true;
        if (tempData()->queryChangedInPreviousView() || !tempData()->query()) {
            //remember current design in a temporary structure
            //build schema; ignore problems
            buildSchema();
        }
        /*  if (tempData()->query && tempData()->query->fieldCount()==0) {
              //no fields selected: let's add "*" (all-tables asterisk),
              // otherwise SQL statement will be invalid
              tempData()->query->addAsterisk( new KDbQueryAsterisk( tempData()->query ) );
            }*/
        //! @todo
        return true;
    }

    return false;
}

tristate
KexiQueryDesignerGuiEditor::afterSwitchFrom(Kexi::ViewMode mode)
{
    if (mode == Kexi::NoViewMode || (mode == Kexi::DataViewMode && !tempData()->query())) {
        //this is not a SWITCH but a fresh opening in this view mode
        if (!window()->neverSaved()) {
            if (!loadLayout()) {
                //err msg
                window()->setStatus(d->conn,
                                    xi18n("Query definition loading failed."),
                                    xi18n("Query design may be corrupted so it could not be opened even in text view.\n"
                                         "You can delete the query and create it again."));
                return false;
            }
            // Invalid queries case:
            // KexiWindow::switchToViewMode() first opens DesignViewMode,
            // and then KexiQueryPart::loadSchemaData() doesn't allocate KDbQuerySchema object
            // do we're carefully looking at window()->schemaData()
            KDbQuerySchema * q = dynamic_cast<KDbQuerySchema *>(window()->schemaData());
            if (q) {
                KDbResultInfo result;
                showFieldsForQuery(q, result);
                if (!result.success) {
                    window()->setStatus(&result, xi18n("Query definition loading failed."));
                    tempData()->proposeOpeningInTextViewModeBecauseOfProblems = true;
                    return false;
                }
            }
//! @todo load global query properties
        }
    } else if (mode == Kexi::TextViewMode || mode == Kexi::DataViewMode) {
        // Switch from text or data view. In the second case, the design could be changed as well
        // because there could be changes made in the text view before switching to the data view.
        if (tempData()->queryChangedInPreviousView()) {
            //previous view changed query data
            //-clear and regenerate GUI items
            initTableRows();
            //! @todo
            if (tempData()->query()) {
                //there is a query schema to show
                showTablesForQuery(tempData()->query());
                //-show fields
                KDbResultInfo result;
                showFieldsAndRelationsForQuery(tempData()->query(), result);
                if (!result.success) {
                    window()->setStatus(&result, xi18n("Query definition loading failed."));
                    return false;
                }
            } else {
                d->relations->clear();
            }
        }
//! @todo load global query properties
    }

    if (mode == Kexi::DataViewMode) {
        //this is just a SWITCH from data view
        //set cursor if needed:
        if (d->dataTable->dataAwareObject()->currentRow() < 0
                || d->dataTable->dataAwareObject()->currentColumn() < 0) {
            d->dataTable->dataAwareObject()->ensureCellVisible(0, 0);
            d->dataTable->dataAwareObject()->setCursorPosition(0, 0);
        }
    }
    if (d->sets->size() > 0) {
        d->dataTable->tableView()->adjustColumnWidthToContents(COLUMN_ID_COLUMN);
        d->dataTable->tableView()->adjustColumnWidthToContents(COLUMN_ID_TABLE);
    }
    tempData()->setQueryChangedInPreviousView(false);
    setFocus(); //to allow shared actions proper update
    return true;
}


KDbObject*
KexiQueryDesignerGuiEditor::storeNewData(const KDbObject& sdata,
                                         KexiView::StoreNewDataOptions options,
                                         bool *cancel)
{
    Q_ASSERT(cancel);
    Q_UNUSED(options);
    if (!d->dataTable->dataAwareObject()->acceptRowEdit()) {
        *cancel = true;
        return 0;
    }
    QString errMsg;
    KexiQueryPart::TempData * temp = tempData();
    if (!temp->query() || !(viewMode() == Kexi::DesignViewMode && !temp->queryChangedInPreviousView())) {
        //only rebuild schema if it has not been rebuilt previously
        if (!buildSchema(&errMsg)) {
            KMessageBox::sorry(this, errMsg);
            *cancel = true;
            return 0;
        }
    }
    (KDbObject&)*temp->query() = sdata; //copy main attributes

    bool ok = d->conn->storeNewObjectData(temp->query());
    if (ok) {
        ok = KexiMainWindowIface::global()->project()->removeUserDataBlock(temp->query()->id()); // for sanity
    }
    window()->setId(temp->query()->id());

    if (ok)
        ok = storeLayout();

    if (!ok) {
        temp->setQuery(0);
        return 0;
    }
    return temp->takeQuery(); //will be returned, so: don't keep it in temp
}

tristate KexiQueryDesignerGuiEditor::storeData(bool dontAsk)
{
    if (!d->dataTable->dataAwareObject()->acceptRowEdit())
        return cancelled;

    const bool was_dirty = isDirty();
    tristate res = KexiView::storeData(dontAsk); //this clears dirty flag
    if (true == res)
        res = buildSchema();
    if (true == res)
        res = storeLayout();
    if (true != res) {
        if (was_dirty)
            setDirty(true);
    }
    return res;
}

void KexiQueryDesignerGuiEditor::showTablesForQuery(KDbQuerySchema *query)
{
    // instead of hiding all tables and showing some tables,
    // show only these new and hide these unncecessary; the same for connections)
    d->slotTableAdded_enabled = false; //speedup
    d->relations->removeAllConnections(); //connections will be recreated
    d->relations->hideAllTablesExcept(query->tables());
    foreach(KDbTableSchema* table, *query->tables()) {
        d->relations->addTable(table);
    }

    d->slotTableAdded_enabled = true;
    updateColumnsData();
}

void KexiQueryDesignerGuiEditor::addConnection(
    KDbField *masterField, KDbField *detailsField)
{
    SourceConnection conn;
    conn.masterTable = masterField->table()->name(); //<<<TODO
    conn.masterField = masterField->name();
    conn.detailsTable = detailsField->table()->name();
    conn.detailsField = detailsField->name();
    d->relations->addConnection(conn);
}

void KexiQueryDesignerGuiEditor::showFieldsForQuery(KDbQuerySchema *query, KDbResultInfo& result)
{
    showFieldsOrRelationsForQueryInternal(query, true, false, result);
}

void KexiQueryDesignerGuiEditor::showRelationsForQuery(KDbQuerySchema *query, KDbResultInfo& result)
{
    showFieldsOrRelationsForQueryInternal(query, false, true, result);
}

void KexiQueryDesignerGuiEditor::showFieldsAndRelationsForQuery(KDbQuerySchema *query,
        KDbResultInfo& result)
{
    showFieldsOrRelationsForQueryInternal(query, true, true, result);
}

void KexiQueryDesignerGuiEditor::showFieldsOrRelationsForQueryInternal(
    KDbQuerySchema *query, bool showFields, bool showRelations, KDbResultInfo& result)
{
    result.clear();
    const bool was_dirty = isDirty();

    //1. Show explicitly declared relations:
    if (showRelations) {
        foreach(KDbRelationship *rel, *query->relationships()) {
//! @todo: now only sigle-field relationships are implemented!
            KDbField *masterField = rel->masterIndex()->fields()->first();
            KDbField *detailsField = rel->detailsIndex()->fields()->first();
            addConnection(masterField, detailsField);
        }
    }

    //2. Collect information about criterias
    // --this must be top level chain of AND's
    // --this will also show joins as: [table1.]field1 = [table2.]field2
    KDb::CaseInsensitiveHash<QString, KDbExpression*> criterias;
    KDbExpression* e = query->whereExpression();
    KDbExpression* eItem = 0;
    while (e) {
        //eat parentheses because the expression can be (....) AND (... AND ... )
        while (e && e->toUnary() && e->token() == '(')
            e = e->toUnary()->arg();

        if (e->toBinary() && e->token() == AND) {
            eItem = e->toBinary()->left();
            e = e->toBinary()->right();
        } else {
            eItem = e;
            e = 0;
        }

        //eat parentheses
        while (eItem && eItem->toUnary() && eItem->token() == '(')
            eItem = eItem->toUnary()->arg();

        if (!eItem)
            continue;

        qDebug() << eItem->toString(0);
        KDbBinaryExpression* binary = eItem->toBinary();
        if (binary && eItem->expressionClass() == KDb::RelationalExpression) {
            KDbField *leftField = 0, *rightField = 0;
            if (eItem->token() == '='
                    && binary->left()->toVariable()
                    && binary->right()->toVariable()
                    && (leftField = query->findTableField(binary->left()->toString(0)))
                    && (rightField = query->findTableField(binary->right()->toString(0)))) {
//! @todo move this check to parser on KDbQuerySchema creation
//!       or to KDbQuerySchema creation (WHERE expression should be then simplified
//!       by removing joins

                //this is relationship defined as following JOIN: [table1.]field1 = [table2.]field2
                if (showRelations) {
//! @todo testing primary key here is too simplified; maybe look ar isForeignKey() or indices..
//! @todo what about multifield joins?
                    if (leftField->isPrimaryKey())
                        addConnection(leftField /*master*/, rightField /*details*/);
                    else
                        addConnection(rightField /*master*/, leftField /*details*/);
//! @todo addConnection() should have "bool oneToOne" arg, for 1-to-1 relations
                }
            } else if (binary->left()->toVariable()) {
                //this is: variable , op , argument
                //store variable -> argument:
                criterias.insertMulti(binary->left()->toVariable()->name, binary->right());
            } else if (binary->right()->toVariable()) {
                //this is: argument , op , variable
                //store variable -> argument:
                criterias.insertMulti(binary->right()->toVariable()->name, binary->left());
            }
        }
    } //while

    if (!showFields)
        return;

    //3. show fields (including * and table.*)
    uint row_num = 0;
    QSet<KDbExpression*> usedCriterias; // <-- used criterias will be saved here
    //     so in step 4. we will be able to add
    //     remaining invisible columns with criterias
    qDebug() << *query;
    foreach(KDbField* field, *query->fields()) {
        qDebug() << *field;
    }
    foreach(KDbField* field, *query->fields()) {
        //append a new row
        QString tableName, fieldName, columnAlias, criteriaString;
        KDbBinaryExpression *criteriaExpr = 0;
        KDbExpression *criteriaArgument = 0;
        if (field->isQueryAsterisk()) {
            if (field->table()) {//single-table asterisk
                tableName = field->table()->name();
                fieldName = "*";
            } else {//all-tables asterisk
                tableName = "*";
                fieldName = "";
            }
        } else {
            columnAlias = query->columnAlias(row_num);
            if (field->isExpression()) {
//! @todo ok? perhaps do not allow to omit aliases?
                fieldName = field->expression()->toString(0);
            }
            else {
                tableName = field->table()->name();
                fieldName = field->name();
                criteriaArgument = criterias.value(fieldName);
                if (!criteriaArgument) {//try table.field
                    criteriaArgument = criterias.value(tableName + "." + fieldName);
                }
                if (criteriaArgument) {//criteria expression is just a parent of argument
                    criteriaExpr = criteriaArgument->parent()->toBinary();
                    usedCriterias.insert(criteriaArgument); //save info. about used criteria
                }
            }
        }
        //create new row data
        KDbRecordData *newRecord = createNewRow(tableName, fieldName, true /* visible*/);
        if (criteriaExpr) {
//! @todo fix for !INFIX operators
            if (criteriaExpr->token() == '=')
                criteriaString = criteriaArgument->toString(0);
            else
                criteriaString = criteriaExpr->tokenToString(0) + " " + criteriaArgument->toString(0);
            (*newRecord)[COLUMN_ID_CRITERIA] = criteriaString;
        }
        d->dataTable->dataAwareObject()->insertItem(newRecord, row_num);
        //OK, row inserted: create a new set for it
        KPropertySet &set = *createPropertySet(row_num, tableName, fieldName, true/*new one*/);
        if (!columnAlias.isEmpty())
            set["alias"].setValue(columnAlias, false);
        if (!criteriaString.isEmpty())
            set["criteria"].setValue(criteriaString, false);
        if (field->isExpression()) {
            if (!d->changeSingleCellValue(*newRecord, COLUMN_ID_COLUMN,
                                          QVariant(columnAlias + ": " + field->expression()->toString(0)), &result))
                return; //problems with setting column expression
        }
        row_num++;
    }

    //4. show ORDER BY information
    d->data->clearRecordEditBuffer();
    KDbOrderByColumnList& orderByColumns = query->orderByColumnList();
    QHash<KDbQueryColumnInfo*, int> columnsOrder(
        query->columnsOrder(KDbQuerySchema::UnexpandedListWithoutAsterisks));
    for (KDbOrderByColumn::ListConstIterator orderByColumnIt(orderByColumns.constBegin());
            orderByColumnIt != orderByColumns.constEnd(); ++orderByColumnIt) {
        KDbOrderByColumn* orderByColumn = *orderByColumnIt;
        KDbQueryColumnInfo *column = orderByColumn->column();
        KDbRecordData *record = 0;
        KPropertySet *rowPropertySet = 0;
        if (column) {
            //sorting for visible column
            if (column->visible) {
                if (columnsOrder.contains(column)) {
                    const int columnPosition = columnsOrder.value(column);
                    record = d->data->at(columnPosition);
                    rowPropertySet = d->sets->at(columnPosition);
                    qDebug() << "\tSetting \"" << *orderByColumn << "\" sorting for record #"
                        << columnPosition;
                }
            }
        } else if (orderByColumn->field()) {
            //this will be presented as invisible field: create new row
            KDbField* field = orderByColumn->field();
            QString tableName(field->table() ? field->table()->name() : QString());
            record = createNewRow(tableName, field->name(), false /* !visible*/);
            d->dataTable->dataAwareObject()->insertItem(record, row_num);
            rowPropertySet = createPropertySet(row_num, tableName, field->name(), true /*newOne*/);
            propertySetSwitched();
            qDebug() << "\tSetting \"" << *orderByColumn << "\" sorting for invisible field"
                << field->name() << ", table " << tableName << " -row #" << row_num;
            row_num++;
        }
        //alter sorting for either existing or new row
        if (record && rowPropertySet) {
            // this will automatically update "sorting" property
            d->data->updateRecordEditBuffer(record, COLUMN_ID_SORTING,
                                         orderByColumn->ascending() ? 1 : 2);
            // in slotBeforeCellChanged()
            d->data->saveRecordChanges(record, true);
            (*rowPropertySet)["sorting"].clearModifiedFlag(); // this property should look "fresh"
            if (!(*record)[COLUMN_ID_VISIBLE].toBool()) //update
                (*rowPropertySet)["visible"].setValue(QVariant(false), false/*rememberOldValue*/);
        }
    }

    //5. Show fields for unused criterias (with "Visible" column set to false)
    foreach(
        KDbExpression *criteriaArgument, // <-- contains field or table.field
        criterias)
    {
        if (usedCriterias.contains(criteriaArgument))
            continue;
        //unused: append a new row
        KDbBinaryExpression *criteriaExpr = criteriaArgument->parent()->toBinary();
        if (!criteriaExpr) {
            qWarning() << "criteriaExpr is not a binary expr";
            continue;
        }
        KDbVariableExpression *columnNameArgument = criteriaExpr->left()->toVariable(); //left or right
        if (!columnNameArgument) {
            columnNameArgument = criteriaExpr->right()->toVariable();
            if (!columnNameArgument) {
                qWarning() << "columnNameArgument is not a variable (table or table.field) expr";
                continue;
            }
        }
        KDbField* field = 0;
        if (!columnNameArgument->name.contains('.') && query->tables()->count() == 1) {
            //extreme case: only field name provided for one-table query:
            field = query->tables()->first()->field(columnNameArgument->name);
        } else {
            field = query->findTableField(columnNameArgument->name);
        }

        if (!field) {
            qWarning() << "no columnInfo found in the query for name" << columnNameArgument->name;
            continue;
        }
        QString tableName, fieldName, columnAlias, criteriaString;
//! @todo what about ALIAS?
        tableName = field->table()->name();
        fieldName = field->name();
        //create new row data
        KDbRecordData *newRecord = createNewRow(tableName, fieldName, false /* !visible*/);
        if (criteriaExpr) {
//! @todo fix for !INFIX operators
            if (criteriaExpr->token() == '=')
                criteriaString = criteriaArgument->toString(0);
            else
                criteriaString = criteriaExpr->tokenToString(0) + " " + criteriaArgument->toString(0);
            (*newRecord)[COLUMN_ID_CRITERIA] = criteriaString;
        }
        d->dataTable->dataAwareObject()->insertItem(newRecord, row_num);
        //OK, row inserted: create a new set for it
        KPropertySet &set = *createPropertySet(row_num++, tableName, fieldName, true/*new one*/);
//! @todo  if (!columnAlias.isEmpty())
//! @todo   set["alias"].setValue(columnAlias, false);
////  if (!criteriaString.isEmpty())
        set["criteria"].setValue(criteriaString, false);
        set["visible"].setValue(QVariant(false), false);
    }

    //current property set has most probably changed
    propertySetSwitched();

    if (!was_dirty)
        setDirty(false);
    //move to 1st column, 1st row
    d->dataTable->dataAwareObject()->ensureCellVisible(0, 0);
// tempData()->registerTableSchemaChanges(query);
}

bool KexiQueryDesignerGuiEditor::loadLayout()
{
    QString xml;
//! @todo if (!loadDataBlock( xml, "query_layout" )) {
    loadDataBlock(&xml, "query_layout");
    //! @todo errmsg
    if (xml.isEmpty()) {
        //in a case when query layout was not saved, build layout by hand
        // -- dynamic cast because of a need for handling invalid queries
        //    (as in KexiQueryDesignerGuiEditor::afterSwitchFrom()):
        KDbQuerySchema * q = dynamic_cast<KDbQuerySchema *>(window()->schemaData());
        if (q) {
            showTablesForQuery(q);
            KDbResultInfo result;
            showRelationsForQuery(q, result);
            if (!result.success) {
                window()->setStatus(&result, xi18n("Query definition loading failed."));
                return false;
            }
        }
        return true;
    }

    QDomDocument doc;
    doc.setContent(xml);
    QDomElement doc_el = doc.documentElement(), el;
    if (doc_el.tagName() != "query_layout") {
        //! @todo errmsg
        return false;
    }

    const bool was_dirty = isDirty();

    //add tables and relations to the relation view
    for (el = doc_el.firstChild().toElement(); !el.isNull(); el = el.nextSibling().toElement()) {
        if (el.tagName() == "table") {
            KDbTableSchema *t = d->conn->tableSchema(el.attribute("name"));
            int x = el.attribute("x", "-1").toInt();
            int y = el.attribute("y", "-1").toInt();
            int width = el.attribute("width", "-1").toInt();
            int height = el.attribute("height", "-1").toInt();
            QRect rect;
            if (x != -1 || y != -1 || width != -1 || height != -1)
                rect = QRect(x, y, width, height);
            d->relations->addTable(t, rect);
        } else if (el.tagName() == "conn") {
            SourceConnection src_conn;
            src_conn.masterTable = el.attribute("mtable");
            src_conn.masterField = el.attribute("mfield");
            src_conn.detailsTable = el.attribute("dtable");
            src_conn.detailsField = el.attribute("dfield");
            d->relations->addConnection(src_conn);
        }
    }

    if (!was_dirty)
        setDirty(false);
    return true;
}

bool KexiQueryDesignerGuiEditor::storeLayout()
{
    KexiQueryPart::TempData * temp = tempData();

    // Save SQL without driver-escaped keywords.
    if (window()->schemaData()) //set this instance as obsolete (only if it's stored)
        d->conn->setQuerySchemaObsolete(window()->schemaData()->name());

    KDbConnection::SelectStatementOptions options;
    options.addVisibleLookupColumns = false;
    QString sqlText = KDb::selectStatement(temp->query(), options);
    if (!storeDataBlock(sqlText, "sql")) {
        return false;
    }

    //serialize detailed XML query definition
    QString xml = "<query_layout>", tmp;
    foreach(KexiRelationsTableContainer* cont, *d->relations->tables()) {
        /*! @todo what about query? */
        tmp = QString("<table name=\"") + QString(cont->schema()->name()) + "\" x=\""
              + QString::number(cont->x())
              + "\" y=\"" + QString::number(cont->y())
              + "\" width=\"" + QString::number(cont->width())
              + "\" height=\"" + QString::number(cont->height())
              + "\"/>";
        xml += tmp;
    }


    foreach(KexiRelationsConnection *conn, *d->relations->connections()) {
        tmp = QString("<conn mtable=\"") + QString(conn->masterTable()->schema()->name())
              + "\" mfield=\"" + conn->masterField() + "\" dtable=\""
              + QString(conn->detailsTable()->schema()->name())
              + "\" dfield=\"" + conn->detailsField() + "\"/>";
        xml += tmp;
    }
    xml += "</query_layout>";
    if (!storeDataBlock(xml, "query_layout")) {
        return false;
    }
    return true;
}

QSize KexiQueryDesignerGuiEditor::sizeHint() const
{
    QSize s1 = d->relations->sizeHint();
    QSize s2 = d->head->sizeHint();
    return QSize(qMax(s1.width(), s2.width()), s1.height() + s2.height());
}

KDbRecordData*
KexiQueryDesignerGuiEditor::createNewRow(const QString& tableName, const QString& fieldName,
        bool visible) const
{
    KDbRecordData *newRecord = d->data->createItem();
    QString key;
    if (tableName == "*")
        key = "*";
    else {
        if (!tableName.isEmpty())
            key = (tableName + ".");
        key += fieldName;
    }
    (*newRecord)[COLUMN_ID_COLUMN] = key;
    (*newRecord)[COLUMN_ID_TABLE] = tableName;
    (*newRecord)[COLUMN_ID_VISIBLE] = QVariant(visible);
#ifndef KEXI_NO_QUERY_TOTALS
    (*newRecord)[COLUMN_ID_TOTALS] = QVariant(0);
#endif
    return newRecord;
}

void KexiQueryDesignerGuiEditor::slotDragOverTableRow(
    KDbRecordData * /*record*/, int /*row*/, QDragMoveEvent* e)
{
    if (e->provides("kexi/field")) {
        e->setAccepted(true);
    }
}

void
KexiQueryDesignerGuiEditor::slotDroppedAtRow(KDbRecordData * /*record*/, int /*row*/,
        QDropEvent *ev, KDbRecordData*& newRecord)
{
    QString sourcePartClass;
    QString srcTable;
    QStringList srcFields;

    /*! @todo KEXI3 Port kexidragobjects.cpp
    if (!KexiFieldDrag::decode(ev, &sourcePartClass, &srcTable, &srcFields))
        return;
    */

    if (srcFields.count() != 1) {
        return;
    }

    //insert new row at specific place
    newRecord = createNewRow(srcTable, srcFields[0], true /* visible*/);
    d->droppedNewRecord = newRecord;
    d->droppedNewTable = srcTable;
    d->droppedNewField = srcFields[0];
    //! @todo
}

void KexiQueryDesignerGuiEditor::slotNewItemAppendedForAfterDeletingInSpreadSheetMode()
{
    KDbRecordData *record = d->data->last();
    if (record)
        (*record)[COLUMN_ID_VISIBLE] = QVariant(false); //the same init as in initTableRows()
}

void KexiQueryDesignerGuiEditor::slotRowInserted(KDbRecordData* record, uint row, bool /*repaint*/)
{
    if (d->droppedNewRecord && d->droppedNewRecord == record) {
        createPropertySet(row, d->droppedNewTable, d->droppedNewField, true);
        propertySetSwitched();
        d->droppedNewRecord = 0;
    }
    tempData()->setQueryChangedInPreviousView(true);
}

void KexiQueryDesignerGuiEditor::slotTableAdded(KDbTableSchema & /*t*/)
{
    if (!d->slotTableAdded_enabled)
        return;
    updateColumnsData();
    setDirty();
    tempData()->setQueryChangedInPreviousView(true);
    d->dataTable->setFocus();
}

void KexiQueryDesignerGuiEditor::slotTableHidden(KDbTableSchema & /*t*/)
{
    updateColumnsData();
    setDirty();
    tempData()->setQueryChangedInPreviousView(true);
}

QByteArray KexiQueryDesignerGuiEditor::generateUniqueAlias() const
{
//! @todo add option for using non-i18n'd "expr" prefix?
    const QByteArray expStr(
        xi18nc("short for 'expression' word (only latin letters, please)", "expr").toLatin1());
//! @todo optimization: cache it?
    QSet<QByteArray> aliases;
    const int setsSize = d->sets->size();
    for (int r = 0; r < setsSize; r++) {
//! @todo use iterator here
        KPropertySet *set = d->sets->at(r);
        if (set) {
            const QByteArray a((*set)["alias"].value().toByteArray().toLower());
            if (!a.isEmpty())
                aliases.insert(a);
        }
    }
    int aliasNr = 1;
    for (;;aliasNr++) {
        if (!aliases.contains(expStr + QByteArray::number(aliasNr)))
            break;
    }
    return expStr + QByteArray::number(aliasNr);
}

//! @todo this is primitive, temporary: reuse SQL parser
KDbExpression*
KexiQueryDesignerGuiEditor::parseExpressionString(const QString& fullString, KDbToken *token,
        bool allowRelationalOperator)
{
    QString str = fullString.trimmed();
    int len = 0;
    //KDbExpression *expr = 0;
    //1. get token
    *token = KDbToken();
    //2-char-long tokens
    if (str.startsWith(QLatin1String(">="))) {
        *token = KDbToken::GREATER_OR_EQUAL;
        len = 2;
    } else if (str.startsWith(QLatin1String("<="))) {
        *token = KDbToken::LESS_OR_EQUAL;
        len = 2;
    } else if (str.startsWith(QLatin1String("<>"))) {
        *token = KDbToken::NOT_EQUAL;
        len = 2;
    } else if (str.startsWith(QLatin1String("!="))) {
        *token = KDbToken::NOT_EQUAL2;
        len = 2;
    } else if (str.startsWith(QLatin1String("=="))) {
        *token = '=';
        len = 2;
    } else if (str.startsWith(QLatin1String("LIKE "),  Qt::CaseInsensitive)) {
        *token = KDbToken::LIKE;
        len = 5;
    }
    else if (str.startsWith(QLatin1String("NOT "),  Qt::CaseInsensitive)) {
        str = str.mid(4).trimmed();
        if (str.startsWith(QLatin1String("LIKE "),  Qt::CaseInsensitive)) {
            *token = KDbToken::NOT_LIKE;
            len = 5;
        }
        else {
            return 0;
        }
    }
    else {
        if (str.startsWith(QLatin1Char('=')) //1-char-long tokens
             || str.startsWith(QLatin1Char('<'))
             || str.startsWith(QLatin1Char('>')))
        {
            token = str[0].toLatin1();
            len = 1;
        } else {
            if (allowRelationalOperator)
                token = '=';
        }
    }

    if (!allowRelationalOperator && token != 0)
        return 0;

    //1. get expression after token
    if (len > 0)
        str = str.mid(len).trimmed();
    if (str.isEmpty())
        return 0;

    KDbExpression *valueExpr = 0;
    QRegExp re;
    if (str.length() >= 2 &&
            (
                (str.startsWith(QLatin1Char('"')) && str.endsWith(QLatin1Char('"')))
                || (str.startsWith(QLatin1Char('\'')) && str.endsWith(QLatin1Char('\''))))
       ) {
        valueExpr = new KDbConstExpression(KDbToken::CHARACTER_STRING_LITERAL, str.mid(1, str.length() - 2));
    } else if (str.startsWith(QLatin1Char('[')) && str.endsWith(QLatin1Char(']'))) {
        valueExpr = new KDbQueryParameterExpression(str.mid(1, str.length() - 2));
    } else if ((re = QRegExp("(\\d{1,4})-(\\d{1,2})-(\\d{1,2})")).exactMatch(str)) {
        valueExpr = new KDbConstExpression(KDbToken::DATE_CONST, QDate::fromString(
                                              re.cap(1).rightJustified(4, '0') + "-" + re.cap(2).rightJustified(2, '0')
                                              + "-" + re.cap(3).rightJustified(2, '0'), Qt::ISODate));
    } else if ((re = QRegExp("(\\d{1,2}):(\\d{1,2})")).exactMatch(str)
               || (re = QRegExp("(\\d{1,2}):(\\d{1,2}):(\\d{1,2})")).exactMatch(str)) {
        QString res = re.cap(1).rightJustified(2, '0') + ":" + re.cap(2).rightJustified(2, '0')
                      + ":" + re.cap(3).rightJustified(2, '0');
//  qDebug() << res;
        valueExpr = new KDbConstExpression(KDbToken::TIME_CONST, QTime::fromString(res, Qt::ISODate));
    } else if ((re = QRegExp("(\\d{1,4})-(\\d{1,2})-(\\d{1,2})\\s+(\\d{1,2}):(\\d{1,2})")).exactMatch(str)
               || (re = QRegExp("(\\d{1,4})-(\\d{1,2})-(\\d{1,2})\\s+(\\d{1,2}):(\\d{1,2}):(\\d{1,2})")).exactMatch(str)) {
        QString res = re.cap(1).rightJustified(4, '0') + "-" + re.cap(2).rightJustified(2, '0')
                      + "-" + re.cap(3).rightJustified(2, '0')
                      + "T" + re.cap(4).rightJustified(2, '0') + ":" + re.cap(5).rightJustified(2, '0')
                      + ":" + re.cap(6).rightJustified(2, '0');
//  qDebug() << res;
        valueExpr = new KDbConstExpr(DATETIME_CONST,
                                          QDateTime::fromString(res, Qt::ISODate));
    } else if ((str[0] >= '0' && str[0] <= '9') || str[0] == '-' || str[0] == '+') {
        //number
        QLocale locale;
        const QChar decimalSym = locale.decimalPoint();
        bool ok;
        int pos = str.indexOf('.');
        if (pos == -1) {//second chance: local decimal symbol
            pos = str.indexOf(decimalSym);
        }
        if (pos >= 0) {//real const number
            const int left = str.left(pos).toInt(&ok);
            if (!ok)
                return 0;
            const int right = str.mid(pos + 1).toInt(&ok);
            if (!ok)
                return 0;
            valueExpr = new KDbConstExpression(KDbToken::REAL_CONST, QPoint(left, right)); //decoded to QPoint
        } else {
            //integer const
            const qint64 val = str.toLongLong(&ok);
            if (!ok)
                return 0;
            valueExpr = new KDbConstExpression(KDbToken::INTEGER_CONST, val);
        }
    } else if (str.toLower() == "null") {
        valueExpr = new KDbConstExpression(KDbToken::SQL_NULL, QVariant());
    } else {//identfier
        if (!KDb::isIdentifier(str))
            return 0;
        valueExpr = new KDbVariableExpression(str);
        //find first matching field for name 'str':
        foreach(KexiRelationsTableContainer *cont, *d->relations->tables()) {
            /*! @todo what about query? */
            if (cont->schema()->table() && cont->schema()->table()->field(str)) {
                valueExpr->toVariable()->field = cont->schema()->table()->field(str);
                break;
            }
        }
    }
    return valueExpr;
}

void KexiQueryDesignerGuiEditor::slotBeforeCellChanged(KDbRecordData *record,
    int colnum, QVariant& newValue, KDbResultInfo* result)
{
    switch (colnum) {
    case COLUMN_ID_COLUMN: slotBeforeColumnCellChanged(record, newValue, result); break;
    case COLUMN_ID_TABLE: slotBeforeTableCellChanged(record, newValue, result); break;
    case COLUMN_ID_VISIBLE: slotBeforeVisibleCellChanged(record, newValue, result); break;
#ifndef KEXI_NO_QUERY_TOTALS
    case COLUMN_ID_TOTALS: slotBeforeTotalsCellChanged(record, newValue, result); break;
#endif
    case COLUMN_ID_SORTING: slotBeforeSortingCellChanged(record, newValue, result); break;
    case COLUMN_ID_CRITERIA: slotBeforeCriteriaCellChanged(record, newValue, result); break;
    default: Q_ASSERT_X(false, "colnum", "unhandled value");
    }
}

void KexiQueryDesignerGuiEditor::slotBeforeColumnCellChanged(KDbRecordData *record,
    QVariant& newValue, KDbResultInfo* result)
{
    if (newValue.isNull()) {
        d->data->updateRecordEditBuffer(record, COLUMN_ID_TABLE, QVariant(),
                                     false/*!allowSignals*/);
        d->data->updateRecordEditBuffer(record, COLUMN_ID_VISIBLE, QVariant(false));//invisible
        d->data->updateRecordEditBuffer(record, COLUMN_ID_SORTING, QVariant());
#ifndef KEXI_NO_QUERY_TOTALS
            d->data->updateRecordEditBuffer(record, COLUMN_ID_TOTALS, QVariant());//remove totals
#endif
        d->data->updateRecordEditBuffer(record, COLUMN_ID_CRITERIA, QVariant());//remove crit.
        d->sets->eraseCurrentPropertySet();
        return;
    }

    //auto fill 'table' column
    QString fieldId(newValue.toString().trimmed());   //tmp, can look like "table.field"
    QString fieldName; //"field" part of "table.field" or expression string
    QString tableName; //empty for expressions
    QByteArray alias;
    QString columnValueForExpr; //for setting pretty printed "alias: expr" in 1st column
    const bool isExpression = !d->fieldColumnIdentifiers.contains(fieldId.toLower());
    if (isExpression) {
        //this value is entered by hand and doesn't match
        //any value in the combo box -- we're assuming this is an expression
        //-table remains null
        //-find "alias" in something like "alias : expr"
        const int id = fieldId.indexOf(':');
        if (id > 0) {
            alias = fieldId.left(id).trimmed().toLatin1();
            if (!KDb::isIdentifier(alias)) {
                result->success = false;
                result->allowToDiscardChanges = true;
                result->column = COLUMN_ID_COLUMN;
                result->msg = xi18n(
                    "Entered column alias \"%1\" is not a valid identifier.", QString(alias));
                result->desc = xi18n("Identifiers should start with a letter or '_' character");
                return;
            }
        }
        fieldName = fieldId.mid(id + 1).trimmed();
        //check expr.
        KDbExpression *e;
        int dummyToken;
        if ((e = parseExpressionString(fieldName, dummyToken,
                                       false/*allowRelationalOperator*/)))
        {
            fieldName = e->toString(0); //print it prettier
            //this is just checking: destroy expr. object
            delete e;
        }
        else {
            result->success = false;
            result->allowToDiscardChanges = true;
            result->column = COLUMN_ID_COLUMN;
            result->msg = xi18n("Invalid expression \"%1\"", fieldName);
            return;
        }
    }
    else {//not expr.
        //this value is properly selected from combo box list
        if (fieldId == "*") {
            tableName = "*";
        }
        else {
            if (!KDb::splitToTableAndFieldParts(
                        fieldId, tableName, fieldName, KDb::SetFieldNameIfNoTableName))
            {
                qWarning() << "no 'field' or 'table.field'";
                return;
            }
        }
    }
    bool saveOldValue = true;
    KPropertySet *set = d->sets->findPropertySetForItem(*record);
    if (!set) {
        saveOldValue = false; // no old val.
        const int row = d->data->indexOf(record);
        if (row < 0) {
            result->success = false;
            return;
        }
        set = createPropertySet(row, tableName, fieldName, true);
        propertySetSwitched();
    }
    d->data->updateRecordEditBuffer(record, COLUMN_ID_TABLE, QVariant(tableName),
                                 false/*!allowSignals*/);
    d->data->updateRecordEditBuffer(record, COLUMN_ID_VISIBLE, QVariant(true));
#ifndef KEXI_NO_QUERY_TOTALS
    d->data->updateRecordEditBuffer(record, COLUMN_ID_TOTALS, QVariant(0));
#endif
    if (!sortingAllowed(fieldName, tableName)) {
        // sorting is not available for "*" or "table.*" rows
//! @todo what about expressions?
        d->data->updateRecordEditBuffer(record, COLUMN_ID_SORTING, QVariant());
    }
    //update properties
    (*set)["field"].setValue(fieldName, saveOldValue);
    if (isExpression) {
        //-no alias but it's needed:
        if (alias.isEmpty()) //-try oto get old alias
            alias = (*set)["alias"].value().toByteArray();
        if (alias.isEmpty()) //-generate smallest unique alias
            alias = generateUniqueAlias();
    }
    (*set)["isExpression"].setValue(QVariant(isExpression), saveOldValue);
    if (!alias.isEmpty()) {
        (*set)["alias"].setValue(alias, saveOldValue);
        //pretty printed "alias: expr"
        newValue = QString(QString(alias) + ": " + fieldName);
    }
    (*set)["caption"].setValue(QString(), saveOldValue);
    (*set)["table"].setValue(tableName, saveOldValue);
    updatePropertiesVisibility(*set);
}

void KexiQueryDesignerGuiEditor::slotBeforeTableCellChanged(KDbRecordData *record,
    QVariant& newValue, KDbResultInfo* result)
{
    Q_UNUSED(result)
    if (newValue.isNull()) {
        if (!(*record)[COLUMN_ID_COLUMN].toString().isEmpty()) {
            d->data->updateRecordEditBuffer(record, COLUMN_ID_COLUMN, QVariant(),
                                         false/*!allowSignals*/);
        }
        d->data->updateRecordEditBuffer(record, COLUMN_ID_VISIBLE, QVariant(false));//invisible
#ifndef KEXI_NO_QUERY_TOTALS
        d->data->updateRecordEditBuffer(record, COLUMN_ID_TOTALS, QVariant());//remove totals
#endif
        d->data->updateRecordEditBuffer(record, COLUMN_ID_CRITERIA, QVariant());//remove crit.
        d->sets->eraseCurrentPropertySet();
    }
    //update property
    KPropertySet *set = d->sets->findPropertySetForItem(*record);
    if (set) {
        if ((*set)["isExpression"].value().toBool() == false) {
            (*set)["table"] = newValue;
            (*set)["caption"] = QVariant(QString());
        }
        else {
            //do not set table for expr. columns
            newValue = QVariant();
        }
        updatePropertiesVisibility(*set);
    }
}

void KexiQueryDesignerGuiEditor::slotBeforeVisibleCellChanged(KDbRecordData *record,
    QVariant& newValue, KDbResultInfo* result)
{
    Q_UNUSED(result)
    bool saveOldValue = true;
    if (!propertySet()) {
        saveOldValue = false;
        createPropertySet(d->dataTable->dataAwareObject()->currentRow(),
                          (*record)[COLUMN_ID_TABLE].toString(),
                          (*record)[COLUMN_ID_COLUMN].toString(), true);
#ifndef KEXI_NO_QUERY_TOTALS
        d->data->updateRecordEditBuffer(record, COLUMN_ID_TOTALS, QVariant(0));//totals
#endif
        propertySetSwitched();
    }
    KPropertySet &set = *propertySet();
    set["visible"].setValue(newValue, saveOldValue);
}

void KexiQueryDesignerGuiEditor::slotBeforeTotalsCellChanged(KDbRecordData *record,
    QVariant& newValue, KDbResultInfo* result)
{
#ifdef KEXI_NO_QUERY_TOTALS
    Q_UNUSED(record)
    Q_UNUSED(newValue)
    Q_UNUSED(result)
#else
    //! @todo unused yet
    setDirty(true);
    tempData()->setQueryChangedInPreviousView(true);
#endif
}

void KexiQueryDesignerGuiEditor::slotBeforeSortingCellChanged(KDbRecordData *record,
    QVariant& newValue, KDbResultInfo* result)
{
    bool saveOldValue = true;
    KPropertySet *set = d->sets->findPropertySetForItem(*record);
    if (!set) {
        saveOldValue = false;
        set = createPropertySet(d->dataTable->dataAwareObject()->currentRow(),
                                (*record)[COLUMN_ID_TABLE].toString(),
                                (*record)[COLUMN_ID_COLUMN].toString(), true);
#ifndef KEXI_NO_QUERY_TOTALS
        d->data->updateRecordEditBuffer(record, COLUMN_ID_TOTALS, QVariant(0));//totals
#endif
        propertySetSwitched();
    }
    QString table(set->property("table").value().toString());
    QString field(set->property("field").value().toString());
    if (newValue.toInt() == 0 || sortingAllowed(field, table)) {
        KProperty &property = set->property("sorting");
        QString key(property.listData()->keysAsStringList()[ newValue.toInt()]);
        qDebug() << "new key=" << key;
        property.setValue(key, saveOldValue);
    }
    else { //show msg: sorting is not available
        result->success = false;
        result->allowToDiscardChanges = true;
        result->column = COLUMN_ID_SORTING;
        result->msg = xi18n("Could not set sorting for multiple columns (%1)",
                           table == "*" ? table : (table + ".*"));
    }
}

void KexiQueryDesignerGuiEditor::slotBeforeCriteriaCellChanged(KDbRecordData *record,
    QVariant& newValue, KDbResultInfo* result)
{
//! @todo this is primitive, temporary: reuse SQL parser
    QString operatorStr, argStr;
    KDbExpression* e = 0;
    const QString str = newValue.toString().trimmed();
    KDbToken token;
    QString field, table;
    KPropertySet *set = d->sets->findPropertySetForItem(*record);
    if (set) {
        field = (*set)["field"].value().toString();
        table = (*set)["table"].value().toString();
    }
    if (!str.isEmpty() && (!set || table == "*" || field.contains("*"))) {
        //asterisk found! criteria not allowed
        result->success = false;
        result->allowToDiscardChanges = true;
        result->column = COLUMN_ID_CRITERIA;
        if (propertySet())
            result->msg = xi18n("Could not set criteria for \"%1\"",
                               table == "*" ? table : field);
        else
            result->msg = xi18n("Could not set criteria for empty record");
    }
    else if (str.isEmpty()
             || (e = parseExpressionString(str, &token, true/*allowRelationalOperator*/)))
    {
        if (e) {
            QString tokenStr;
            if (token.value() != '=') {
                tokenStr = token.toString() + " ";
            }
            if (set) {
                (*set)["criteria"] = QString(tokenStr + e->toString(0)); //print it prettier
            }
            //this is just checking: destroy expr. object
            delete e;
        } else if (set && str.isEmpty()) {
            (*set)["criteria"] = QVariant(); //clear it
        }
        setDirty(true);
        tempData()->setQueryChangedInPreviousView(true);
    }
    else {
        result->success = false;
        result->allowToDiscardChanges = true;
        result->column = COLUMN_ID_CRITERIA;
        result->msg = xi18n("Invalid criteria \"%1\"", newValue.toString());
    }
}

void KexiQueryDesignerGuiEditor::slotTablePositionChanged(KexiRelationsTableContainer*)
{
    setDirty(true);
    // this is not needed here because only position has changed: tempData()->setQueryChangedInPreviousView(true);
}

void KexiQueryDesignerGuiEditor::slotAboutConnectionRemove(KexiRelationsConnection*)
{
    setDirty(true);
    tempData()->setQueryChangedInPreviousView(true);
}

void KexiQueryDesignerGuiEditor::slotAppendFields(
    KDbTableOrQuerySchema& tableOrQuery, const QStringList& fieldNames)
{
//! @todo how about query columns and multiple fields?
    KDbTableSchema *table = tableOrQuery.table();
    if (!table || fieldNames.isEmpty())
        return;
    QString fieldName(fieldNames.first());
    if (fieldName != "*" && !table->field(fieldName))
        return;
    int row_num;
    //find last filled row in the GUI table
    for (row_num = d->sets->size() - 1; row_num >= 0 && !d->sets->at(row_num); row_num--) {
    }
    row_num++; //after
    //add row
    KDbRecordData *newRecord = createNewRow(table->name(), fieldName, true /* visible*/);
    d->dataTable->dataAwareObject()->insertItem(newRecord, row_num);
    d->dataTable->dataAwareObject()->setCursorPosition(row_num, 0);
    //create buffer
    createPropertySet(row_num, table->name(), fieldName, true/*new one*/);
    propertySetSwitched();
    d->dataTable->setFocus();
}

KPropertySet *KexiQueryDesignerGuiEditor::propertySet()
{
    return d->sets->currentPropertySet();
}

void KexiQueryDesignerGuiEditor::updatePropertiesVisibility(KPropertySet& set)
{
    const bool asterisk = isAsterisk(
                              set["table"].value().toString(), set["field"].value().toString()
                          );
#ifdef KEXI_SHOW_UNFINISHED
    set["caption"].setVisible(!asterisk);
#endif
    set["alias"].setVisible(!asterisk);
    /*always invisible #ifdef KEXI_SHOW_UNFINISHED
      set["sorting"].setVisible( !asterisk );
    #endif*/
    propertySetReloaded(true);
}

KPropertySet*
KexiQueryDesignerGuiEditor::createPropertySet(int row,
        const QString& tableName, const QString& fieldName, bool newOne)
{
    //const bool asterisk = isAsterisk(tableName, fieldName);
    QString typeName = "KexiQueryDesignerGuiEditor::Column";
    KPropertySet *set = new KPropertySet(d->sets, typeName);
    KProperty *prop;

    //meta-info for property editor
    set->addProperty(prop = new KProperty("this:classString", xi18n("Query column")));
    prop->setVisible(false);
//! \todo add table_field icon (add buff->addProperty(prop = new KexiProperty("this:iconName", "table_field") );
// prop->setVisible(false);

    set->addProperty(prop = new KProperty("table", QVariant(tableName)));
    prop->setVisible(false);//always hidden

    set->addProperty(prop = new KProperty("field", QVariant(fieldName)));
    prop->setVisible(false);//always hidden

    set->addProperty(prop = new KProperty("caption", QVariant(QString()), xi18n("Caption")));
#ifndef KEXI_SHOW_UNFINISHED
    prop->setVisible(false);
#endif

    set->addProperty(prop = new KProperty("alias", QVariant(QString()), xi18n("Alias")));

    set->addProperty(prop = new KProperty("visible", QVariant(true)));
    prop->setVisible(false);

    /*! @todo
      set->addProperty(prop = new KexiProperty("totals", QVariant(QString())) );
      prop->setVisible(false);*/

    //sorting
    QStringList slist, nlist;
    slist << "nosorting" << "ascending" << "descending";
    nlist << xi18n("None") << xi18n("Ascending") << xi18n("Descending");
    set->addProperty(prop = new KProperty("sorting",
            slist, nlist, slist[0], xi18n("Sorting")));
    prop->setVisible(false);

    set->addProperty(prop = new KProperty("criteria", QVariant(QString())));
    prop->setVisible(false);

    set->addProperty(prop = new KProperty("isExpression", QVariant(false)));
    prop->setVisible(false);

    d->sets->set(row, set, newOne);

    updatePropertiesVisibility(*set);
    return set;
}

void KexiQueryDesignerGuiEditor::setFocus()
{
    d->dataTable->setFocus();
}

void KexiQueryDesignerGuiEditor::slotPropertyChanged(KPropertySet& set, KProperty& property)
{
    const QByteArray pname(property.name());
    /*! @todo use KexiProperty::setValidator(QString) when implemented as described in TODO #60
     */
    if (pname == "alias" || pname == "name") {
        const QVariant& v = property.value();
        if (!v.toString().trimmed().isEmpty() && !KDb::isIdentifier(v.toString())) {
            KMessageBox::sorry(this,
                               KexiUtils::identifierExpectedMessage(property.caption(), v.toString()));
            property.resetValue();
        }
        if (pname == "alias") {
            if (set["isExpression"].value().toBool() == true) {
                //update value in column #1
                d->dataTable->dataAwareObject()->acceptEditor();
                d->data->updateRecordEditBuffer(d->dataTable->dataAwareObject()->selectedItem(),
                                             0, QVariant(set["alias"].value().toString()
                                                         + ": " + set["field"].value().toString()));
                d->data->saveRecordChanges(d->dataTable->dataAwareObject()->selectedItem(), true);
            }
        }
    }
    tempData()->setQueryChangedInPreviousView(true);
}

void KexiQueryDesignerGuiEditor::slotNewItemStored(KexiPart::Item& item)
{
    d->relations->objectCreated(item.pluginId(), item.name());
}

void KexiQueryDesignerGuiEditor::slotItemRemoved(const KexiPart::Item& item)
{
    d->relations->objectDeleted(item.pluginId(), item.name());
}

void KexiQueryDesignerGuiEditor::slotItemRenamed(const KexiPart::Item& item, const QString& oldName)
{
    d->relations->objectRenamed(item.pluginId(), oldName, item.name());
}

