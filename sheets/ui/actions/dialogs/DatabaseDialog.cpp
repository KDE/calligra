/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2023 Tomas Mecir <mecirt@gmail.com>
   SPDX-FileCopyrightText: 1998-2022 The Calligra Team <calligra-devel@kde.org>
   SPDX-FileCopyrightText: 2002-2003 Norbert Andres <nandres@web.de>
   SPDX-FileCopyrightText: 2002 Ariya Hidayat <ariya@kde.org>
   SPDX-FileCopyrightText: 2002 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Local
#include "DatabaseDialog.h"

#ifndef QT_NO_SQL

#include "core/Sheet.h"
#include "engine/CellBase.h"
#include "engine/MapBase.h"
#include "engine/ValueStorage.h"
#include "ui/Selection.h"

#include "ui/commands/DataManipulators.h"

#include <KoCanvasBase.h>

#include <KComboBox>
#include <KLocalizedString>
#include <KMessageBox>
#include <KPageWidget>
#include <klineedit.h>
#include <ktextedit.h>

#include <QCheckBox>
#include <QGridLayout>
#include <QLabel>
#include <QListWidget>
#include <QRadioButton>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlField>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QTreeWidget>

using namespace Calligra::Sheets;

/********************************************************
 *                 Database Assistant                   *
 ********************************************************/

DatabaseDialog::DatabaseDialog(QWidget *parent, Selection *selection)
    : ActionDialog(parent, User1 | User2)
    , m_currentPage(eDatabase)
    , m_selection(selection)
    , m_targetRect(selection->lastRange())
{
    setObjectName(QLatin1String("DatabaseDialog"));
    setWindowTitle(i18n("Insert Data From Database"));
    setButtonText(Apply, i18n("Finish"));
    setButtonText(User1, i18n("Next"));
    setButtonText(User2, i18n("Previous"));

    main = new KPageWidget;
    setMainWidget(main);
    main->setFaceType(KPageWidget::Plain);

    // database page

    QFrame *databaseFrame = new QFrame(this);
    QGridLayout *databaseFrameLayout = new QGridLayout(databaseFrame);

    // Row 0 - type
    m_Type = new QLabel(databaseFrame);
    m_Type->setText(i18n("Type:"));
    databaseFrameLayout->addWidget(m_Type, 0, 0);

    m_driver = new KComboBox(databaseFrame);
    databaseFrameLayout->addWidget(m_driver, 0, 1);

    // Row 1 - db name
    QLabel *dbName = new QLabel(databaseFrame);
    dbName->setText(i18n("Database name: "));
    databaseFrameLayout->addWidget(dbName, 1, 0);

    m_databaseName = new KLineEdit(databaseFrame);
    databaseFrameLayout->addWidget(m_databaseName, 1, 1);

    // Row 2 - host
    QLabel *TextLabel2 = new QLabel(databaseFrame);
    TextLabel2->setText(i18n("Host:"));
    databaseFrameLayout->addWidget(TextLabel2, 2, 0);

    m_host = new KLineEdit(databaseFrame);
    m_host->setText("localhost");
    databaseFrameLayout->addWidget(m_host, 2, 1);

    // Row 3 - port
    QLabel *TextLabel3 = new QLabel(databaseFrame);
    TextLabel3->setText(i18n("Port:\n(if necessary)"));
    databaseFrameLayout->addWidget(TextLabel3, 3, 0);

    m_port = new KLineEdit(databaseFrame);
    m_port->setValidator(new QIntValidator(m_port));
    databaseFrameLayout->addWidget(m_port, 3, 1);

    // Row 4 - username
    QLabel *TextLabel4 = new QLabel(databaseFrame);
    TextLabel4->setText(
        i18n("User name:\n"
             "(if necessary)"));
    databaseFrameLayout->addWidget(TextLabel4, 4, 0);

    m_username = new KLineEdit(databaseFrame);
    databaseFrameLayout->addWidget(m_username, 4, 1);

    // Row 5 - password
    QLabel *TextLabel5 = new QLabel(databaseFrame);
    TextLabel5->setText(
        i18n("Password:\n"
             "(if necessary)"));
    databaseFrameLayout->addWidget(TextLabel5, 5, 0);

    m_password = new KLineEdit(databaseFrame);
    m_password->setEchoMode(KLineEdit::Password);
    databaseFrameLayout->addWidget(m_password, 5, 1);

    // Row 6 - status
    m_databaseStatus = new QLabel(databaseFrame);
    m_databaseStatus->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    m_databaseStatus->setMaximumSize(QSize(32767, 30));
    m_databaseStatus->setText(" ");
    databaseFrameLayout->addWidget(m_databaseStatus, 6, 0, 1, 2);

    databaseFrameLayout->setRowStretch(7, 1);

    m_database = new KPageWidgetItem(databaseFrame, i18n("Database"));
    main->addPage(m_database);

    // tables page

    QFrame *tablesFrame = new QFrame(this);
    QGridLayout *tablesFrameLayout = new QGridLayout(tablesFrame);

    //   QHBoxLayout * Layout21 = new QHBoxLayout();
    //   Layout21->setMargin(0);
    //   Layout21->setSpacing(6);

    //  QLabel * TextLabel12_2 = new QLabel( tablesFrame );
    //  TextLabel12_2->setText( i18n( "Database:" ) );
    //  Layout21->addWidget( TextLabel12_2 );

    //  m_databaseList = new KComboBox( tablesFrame );
    //  Layout21->addWidget( m_databaseList );

    //  m_connectButton = new QPushButton( tablesFrame, "m_connectButton" );
    //  m_connectButton->setText( i18n( "&Connect" ) );
    //  Layout21->addWidget( m_connectButton );

    //   tablesFrameLayout->addLayout( Layout21, 0, 0 );

    m_SelectTableLabel = new QLabel(tablesFrame);
    m_SelectTableLabel->setText(i18n("Select tables:"));
    tablesFrameLayout->addWidget(m_SelectTableLabel, 1, 0);

    m_tableView = new QListWidget(tablesFrame);
    tablesFrameLayout->addWidget(m_tableView, 2, 0);

    m_tableStatus = new QLabel(tablesFrame);
    m_tableStatus->setText(" ");
    tablesFrameLayout->addWidget(m_tableStatus, 3, 0);

    tablesFrameLayout->setRowStretch(2, 1);

    m_table = new KPageWidgetItem(tablesFrame, i18n("Tables"));
    main->addPage(m_table);

    // columns page

    QFrame *columnsFrame = new QFrame(this);
    QGridLayout *columnsFrameLayout = new QGridLayout(columnsFrame);

    QLabel *TextLabel11_2 = new QLabel(columnsFrame);
    TextLabel11_2->setText(i18n("Select columns:"));
    columnsFrameLayout->addWidget(TextLabel11_2, 0, 0);

    m_columnView = new QTreeWidget(columnsFrame);
    m_columnView->setColumnCount(3);
    m_columnView->setHeaderLabels(QStringList() << i18n("Column") << i18n("Table") << "Data Type");

    columnsFrameLayout->addWidget(m_columnView, 1, 0);

    m_columnsStatus = new QLabel(columnsFrame);
    m_columnsStatus->setText(" ");
    columnsFrameLayout->addWidget(m_columnsStatus, 2, 0);

    columnsFrameLayout->setRowStretch(1, 1);

    m_columns = new KPageWidgetItem(columnsFrame, i18n("Columns"));
    main->addPage(m_columns);

    // options page

    QFrame *optionsFrame = new QFrame(this);
    QGridLayout *optionsFrameLayout = new QGridLayout(optionsFrame);

    m_andBox = new QRadioButton(optionsFrame);
    m_andBox->setText(i18n("Match all of the following (AND)"));
    m_andBox->setChecked(true);
    optionsFrameLayout->addWidget(m_andBox, 0, 0, 1, 3);

    m_orBox = new QRadioButton(optionsFrame);
    m_orBox->setText(i18n("Match any of the following (OR)"));
    optionsFrameLayout->addWidget(m_orBox, 1, 0, 1, 3);

    m_columns_1 = new KComboBox(optionsFrame);
    optionsFrameLayout->addWidget(m_columns_1, 2, 0);

    m_operator_1 = new KComboBox(optionsFrame);
    m_operator_1->insertItem(0, i18n("equals"));
    m_operator_1->insertItem(1, i18n("not equal"));
    m_operator_1->insertItem(2, i18n("in"));
    m_operator_1->insertItem(3, i18n("not in"));
    m_operator_1->insertItem(4, i18n("like"));
    m_operator_1->insertItem(5, i18n("greater"));
    m_operator_1->insertItem(6, i18n("lesser"));
    m_operator_1->insertItem(7, i18n("greater or equal"));
    m_operator_1->insertItem(8, i18n("less or equal"));
    optionsFrameLayout->addWidget(m_operator_1, 2, 1);

    m_operatorValue_1 = new KLineEdit(optionsFrame);
    optionsFrameLayout->addWidget(m_operatorValue_1, 2, 2);

    m_columns_2 = new KComboBox(optionsFrame);
    m_columns_2->setEditable(false);
    optionsFrameLayout->addWidget(m_columns_2, 3, 0);

    m_operator_2 = new KComboBox(optionsFrame);
    m_operator_2->insertItem(0, i18n("equals"));
    m_operator_2->insertItem(1, i18n("not equal"));
    m_operator_2->insertItem(2, i18n("in"));
    m_operator_2->insertItem(3, i18n("not in"));
    m_operator_2->insertItem(4, i18n("like"));
    m_operator_2->insertItem(5, i18n("greater"));
    m_operator_2->insertItem(6, i18n("lesser"));
    optionsFrameLayout->addWidget(m_operator_2, 3, 1);

    m_operatorValue_2 = new KLineEdit(optionsFrame);
    optionsFrameLayout->addWidget(m_operatorValue_2, 3, 2);

    m_columns_3 = new KComboBox(optionsFrame);
    optionsFrameLayout->addWidget(m_columns_3, 4, 0);

    m_operator_3 = new KComboBox(optionsFrame);
    m_operator_3->insertItem(0, i18n("equals"));
    m_operator_3->insertItem(1, i18n("not equal"));
    m_operator_3->insertItem(2, i18n("in"));
    m_operator_3->insertItem(3, i18n("not in"));
    m_operator_3->insertItem(4, i18n("like"));
    m_operator_3->insertItem(5, i18n("greater"));
    m_operator_3->insertItem(6, i18n("lesser"));
    optionsFrameLayout->addWidget(m_operator_3, 4, 1);

    m_operatorValue_3 = new KLineEdit(optionsFrame);
    optionsFrameLayout->addWidget(m_operatorValue_3, 4, 2);

    QLabel *TextLabel19 = new QLabel(optionsFrame);
    TextLabel19->setText(i18n("Sorted by"));
    optionsFrameLayout->addWidget(TextLabel19, 5, 0);

    m_columnsSort_1 = new KComboBox(optionsFrame);
    optionsFrameLayout->addWidget(m_columnsSort_1, 5, 1);

    m_sortMode_1 = new KComboBox(optionsFrame);
    m_sortMode_1->insertItem(0, i18n("Ascending"));
    m_sortMode_1->insertItem(1, i18n("Descending"));
    optionsFrameLayout->addWidget(m_sortMode_1, 5, 2);

    QLabel *TextLabel19_2 = new QLabel(optionsFrame);
    TextLabel19_2->setText(i18n("Sorted by"));
    optionsFrameLayout->addWidget(TextLabel19_2, 6, 0);

    m_columnsSort_2 = new KComboBox(optionsFrame);
    optionsFrameLayout->addWidget(m_columnsSort_2, 6, 1);

    m_sortMode_2 = new KComboBox(optionsFrame);
    m_sortMode_2->insertItem(0, i18n("Ascending"));
    m_sortMode_2->insertItem(1, i18n("Descending"));
    optionsFrameLayout->addWidget(m_sortMode_2, 6, 2);

    QSpacerItem *spacer = new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    optionsFrameLayout->addItem(spacer, 7, 1);
    QSpacerItem *spacer_2 = new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    optionsFrameLayout->addItem(spacer_2, 7, 0);

    m_distinct = new QCheckBox(optionsFrame);
    m_distinct->setText(i18n("Distinct"));
    optionsFrameLayout->addWidget(m_distinct, 7, 2);

    optionsFrameLayout->setRowStretch(8, 1);

    m_options = new KPageWidgetItem(optionsFrame, i18n("Query Options"));
    main->addPage(m_options);

    // result page

    QFrame *resultFrame = new QFrame(this);
    QGridLayout *resultFrameLayout = new QGridLayout(resultFrame);

    QLabel *TextLabel17 = new QLabel(resultFrame);
    TextLabel17->setText(i18n("SQL query:"));
    resultFrameLayout->addWidget(TextLabel17, 0, 0);

    m_sqlQuery = new KTextEdit(resultFrame);
    m_sqlQuery->setReadOnly(true);
    resultFrameLayout->addWidget(m_sqlQuery, 1, 0);

    QFrame *Frame12 = new QFrame(resultFrame);
    Frame12->setFrameShape(QFrame::StyledPanel);
    Frame12->setFrameShadow(QFrame::Raised);
    QGridLayout *Frame12Layout = new QGridLayout(Frame12);

    m_startingRegion = new QRadioButton(Frame12);
    m_startingRegion->setText(i18n("Insert in region"));
    Frame12Layout->addWidget(m_startingRegion, 0, 0);

    m_region = new KLineEdit(Frame12);
    Frame12Layout->addWidget(m_region, 0, 1);

    m_startingCell = new QRadioButton(Frame12);
    m_startingCell->setText(i18n("Starting in cell"));
    m_startingCell->setChecked(true);
    Frame12Layout->addWidget(m_startingCell, 1, 0);

    m_cell = new KLineEdit(Frame12);
    Frame12Layout->addWidget(m_cell, 1, 1);

    resultFrameLayout->addWidget(Frame12, 2, 0);

    resultFrameLayout->setRowStretch(1, 1);

    m_result = new KPageWidgetItem(resultFrame, i18n("Result"));
    main->addPage(m_result);

    // signals and slots connections
    connect(m_orBox, &QAbstractButton::clicked, this, &DatabaseDialog::orBox_clicked);
    connect(m_andBox, &QAbstractButton::clicked, this, &DatabaseDialog::andBox_clicked);
    connect(m_startingCell, &QAbstractButton::clicked, this, &DatabaseDialog::startingCell_clicked);
    connect(m_startingRegion, &QAbstractButton::clicked, this, &DatabaseDialog::startingRegion_clicked);
    connect(m_driver, QOverload<int>::of(&KComboBox::activated), this, &DatabaseDialog::databaseDriverChanged);
    connect(m_host, &QLineEdit::textChanged, this, &DatabaseDialog::databaseHostChanged);
    connect(m_databaseName, &QLineEdit::textChanged, this, &DatabaseDialog::databaseNameChanged);
    connect(this, &KoDialog::user1Clicked, this, &DatabaseDialog::onNext);
    connect(this, &KoDialog::user2Clicked, this, &DatabaseDialog::onBack);

    QStringList str = QSqlDatabase::drivers();
    m_driver->insertItems(0, QSqlDatabase::drivers());

    databaseDriverChanged(0);

    setTargetRect(selection->lastRange());
    enableButtons();
}

DatabaseDialog::~DatabaseDialog()
{
    // no need to delete child widgets, Qt does it all for us
    if (m_dbConnection.isValid())
        m_dbConnection.close();
}

void DatabaseDialog::setTargetRect(const QRect &rect)
{
    m_targetRect = rect;
    m_cell->setText(CellBase::name(m_targetRect.left(), m_targetRect.top()));
    m_region->setText(Region(m_targetRect).name());
}

void DatabaseDialog::enableButtons()
{
    bool btnBack = false, btnNext = false, btnApply = false;

    // Back button
    if (m_currentPage)
        btnBack = true;

    // Next button
    if (m_currentPage == eDatabase) {
        if (!m_driver->currentText().isEmpty() && !m_databaseName->text().isEmpty() && !m_host->text().isEmpty())
            btnNext = true;
    } else {
        btnNext = true;
    }

    // Finish button
    if (m_currentPage == eResult) {
        btnNext = false;
        btnApply = true;
    }

    enableButton(User1, btnNext);
    enableButton(User2, btnBack);
    enableButtonApply(btnApply);
}

void DatabaseDialog::onSelectionChanged(Selection *sel)
{
    setTargetRect(sel->lastRange());
}

void DatabaseDialog::switchPage(int id)
{
    if (id > eResult)
        --m_currentPage;
    if (id < eDatabase)
        ++m_currentPage;

    switch (id) {
    case eDatabase:
        main->setCurrentPage(m_database);
        break;

    case eTables:
        main->setCurrentPage(m_table);
        break;

    case eColumns:
        main->setCurrentPage(m_columns);
        break;

    case eOptions:
        main->setCurrentPage(m_options);
        break;

    case eResult:
        main->setCurrentPage(m_result);
        break;

    default:
        break;
    }

    enableButtons();
}

void DatabaseDialog::onNext()
{
    switch (m_currentPage) {
    case eDatabase:
        if (!databaseDoNext())
            return;
        break;

    case eTables:
        if (!tablesDoNext())
            return;
        break;

    case eColumns:
        if (!columnsDoNext())
            return;
        break;

    case eOptions:
        if (!optionsDoNext())
            return;
        break;

    case eResult:
        // there is nothing to do here
        break;

    default:
        break;
    }

    ++m_currentPage;

    switchPage(m_currentPage);
}

void DatabaseDialog::onBack()
{
    --m_currentPage;

    switchPage(m_currentPage);
}

void DatabaseDialog::onApply()
{
    Sheet *sheet = m_selection->activeSheet();
    int top;
    int left;
    int width = -1;
    int height = -1;
    if (m_startingRegion->isChecked()) {
        Region region = sheet->map()->regionFromName(m_region->text(), sheet);
        if (region.isValid() && region.firstSheet() != sheet) {
            KMessageBox::error(this, i18n("You cannot specify a table here."));
            m_region->setFocus();
            m_region->selectAll();
            return;
        }

        if (!region.isValid()) {
            KMessageBox::error(this, i18n("You have to specify a valid region."));
            m_region->setFocus();
            m_region->selectAll();
            return;
        }

        top = region.firstRange().top();
        left = region.firstRange().left();
        width = region.firstRange().width();
        height = region.firstRange().height();
    } else {
        const Region region = sheet->map()->regionFromName(m_cell->text(), sheet);
        if (region.isValid() && region.firstSheet() != sheet) {
            KMessageBox::error(this, i18n("You cannot specify a table here."));
            m_cell->setFocus();
            m_cell->selectAll();
            return;
        }
        //    if ( point.pos.x() < 1 || point.pos.y() < 1 )
        if (!region.isValid()) {
            KMessageBox::error(this, i18n("You have to specify a valid cell."));
            m_cell->setFocus();
            m_cell->selectAll();
            return;
        }
        top = region.firstRange().topLeft().y();
        left = region.firstRange().topLeft().x();
    }

    QSqlQuery query(m_dbConnection);

    if (!query.exec(m_queryString)) {
        KMessageBox::error(this, i18n("Executing query failed."));
        return;
    }

    if (query.size() == 0) {
        KMessageBox::error(this, i18n("You did not get any results with this query."));
        return;
    }

    int y = 0;
    int count = m_columns_1->count();
    if (width != -1) {
        if (count > width)
            count = width;
    }

    if (height == -1) {
        height = 0;
        if (query.first()) {
            if (query.isValid())
                ++height;
        }
        while (query.next()) {
            if (query.isValid())
                ++height;
        }
    }

    ValueStorage vs;
    Value val(vs, QSize(count, height));

    if (query.first()) {
        if (query.isValid()) {
            for (int i = 0; i < count; ++i)
                val.setElement(i, y, Value(query.value(i).toString()));
            ++y;
        }
    }

    if (y != height) {
        while (query.next()) {
            if (!query.isValid())
                continue;

            for (int i = 0; i < count; ++i)
                val.setElement(i, y, Value(query.value(i).toString()));
            ++y;

            if (y == height)
                break;
        }
    }

    DataManipulator *command = new DataManipulator();
    command->setText(kundo2_i18n("Insert Data From Database"));
    command->setParsing(true);
    command->setSheet(sheet);
    command->setValue(val);
    command->add(Region(left, top, count, height, sheet));
    command->execute(m_selection->canvas());

    m_selection->initialize(*command, sheet);
    m_selection->emitModified();

    accept();
}

bool DatabaseDialog::databaseDoNext()
{
    m_dbConnection = QSqlDatabase::addDatabase(m_driver->currentText());

    if (m_dbConnection.isValid()) {
        m_dbConnection.setDatabaseName(m_databaseName->text());
        m_dbConnection.setHostName(m_host->text());

        if (!m_username->text().isEmpty())
            m_dbConnection.setUserName(m_username->text());

        if (!m_password->text().isEmpty())
            m_dbConnection.setPassword(m_password->text());

        if (!m_port->text().isEmpty()) {
            bool ok = false;
            int port = m_port->text().toInt(&ok);
            if (!ok) {
                KMessageBox::error(this, i18n("The port must be a number"));
                return false;
            }
            m_dbConnection.setPort(port);
        }

        m_databaseStatus->setText(i18n("Connecting to database..."));
        if (m_dbConnection.open()) {
            m_databaseStatus->setText(i18n("Connected. Retrieving table information..."));
            QStringList tableList(m_dbConnection.tables());

            if (tableList.isEmpty()) {
                KMessageBox::error(this, i18n("This database contains no tables"));
                m_databaseStatus->setText(" ");
                return false;
            }

            m_tableView->clear();

            for (int i = 0; i < tableList.size(); ++i) {
                QListWidgetItem *item = new QListWidgetItem(tableList[i]);
                item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
                item->setCheckState(Qt::Unchecked);
                m_tableView->addItem(item);
            }

            m_tableView->setEnabled(true);
            m_tableView->sortItems();
            m_databaseStatus->setText(" ");
        } else {
            QSqlError error = m_dbConnection.lastError();
            QString errorMsg;
            QString err1 = error.driverText();
            QString err2 = error.databaseText();
            if (!err1.isEmpty()) {
                errorMsg.append(error.driverText());
                errorMsg.append('\n');
            }
            if (!err2.isEmpty() && err1 != err2) {
                errorMsg.append(error.databaseText());
                errorMsg.append('\n');
            }

            KMessageBox::error(this, errorMsg);
            m_databaseStatus->setText(" ");
            return false;
        }
    } else {
        KMessageBox::error(this, i18n("Driver could not be loaded"));
        m_databaseStatus->setText(" ");
        return false;
    }

    return true;
}

bool DatabaseDialog::tablesDoNext()
{
    m_databaseStatus->setText(i18n("Retrieving meta data of tables..."));
    QStringList tables;

    {
        for (int i = 0; i < m_tableView->count(); ++i) {
            QListWidgetItem *item = m_tableView->item(i);
            if (item->checkState() == Qt::Checked) {
                tables.append(item->text());
            }
        }
    }

    if (tables.empty()) {
        KMessageBox::error(this, i18n("You have to select at least one table."));
        return false;
    }

    m_columnView->clear();
    QSqlRecord info;
    for (int i = 0; i < (int)tables.size(); ++i) {
        info = m_dbConnection.record(tables[i]);
        for (int j = 0; j < (int)info.count(); ++j) {
            QString name = info.fieldName(j);
            QSqlField field = info.field(name);
            QTreeWidgetItem *checkItem = new QTreeWidgetItem(QStringList() << name << tables[i] << QVariant::typeToName(field.type()));

            checkItem->setFlags(checkItem->flags() | Qt::ItemIsUserCheckable);
            checkItem->setCheckState(0, Qt::Unchecked);
            m_columnView->addTopLevelItem(checkItem);
        }
    }
    // Sort by names, then by tables. This ensures that the column names are sorted within each table.
    m_columnView->sortItems(0, Qt::AscendingOrder);
    m_columnView->sortItems(1, Qt::AscendingOrder);

    return true;
}

bool DatabaseDialog::columnsDoNext()
{
    QStringList columns;
    for (int row = 0; row < m_columnView->topLevelItemCount(); ++row) {
        QTreeWidgetItem *item = m_columnView->topLevelItem(row);
        if (item->checkState(0) == Qt::Checked) {
            columns.append(item->text(1) + '.' + item->text(0));
        }
    }

    if (columns.empty()) {
        KMessageBox::error(this, i18n("You have to select at least one column."));
        return false;
    }

    m_columns_1->clear();
    m_columns_2->clear();
    m_columns_3->clear();
    m_columns_1->insertItems(0, columns);
    m_columns_2->insertItems(0, columns);
    m_columns_3->insertItems(0, columns);
    m_columnsSort_1->clear();
    m_columnsSort_2->clear();
    m_columnsSort_1->insertItem(0, i18n("None"));
    m_columnsSort_2->insertItem(0, i18n("None"));
    m_columnsSort_1->insertItems(1, columns);
    m_columnsSort_2->insertItems(2, columns);

    return true;
}

QString DatabaseDialog::getWhereCondition(QString const &column, QString const &value, int op)
{
    QString wherePart;

    switch (op) {
    case 0:
        wherePart += column + " = ";
        break;
    case 1:
        wherePart += "NOT " + column + " = ";
        break;
    case 2:
        wherePart += column + " IN ";
        break;
    case 3:
        wherePart += "NOT " + column + " IN ";
        break;
    case 4:
        wherePart += column + " LIKE ";
        break;
    case 5:
        wherePart += column + " > ";
        break;
    case 6:
        wherePart += column + " < ";
        break;
    case 7:
        wherePart += column + " >= ";
        break;
    case 8:
        wherePart += column + " <= ";
        break;
    }

    if (op != 2 && op != 3) {
        QString val;
        bool ok = false;
        value.toDouble(&ok);

        if (!ok) {
            if (value[0] != '\'')
                val = '\'';

            val += value;

            if (value[value.length() - 1] != '\'')
                val += '\'';
        } else
            val = value;

        wherePart += val;
    } else { // "in" & "not in"
        QString val;
        if (value[0] != '(')
            val = '(';
        val += value;
        if (value[value.length() - 1] != ')')
            val += ')';
        wherePart += val;
    }

    return wherePart;
}

QString DatabaseDialog::exchangeWildcards(QString const &value)
{
    QString str(value);
    int p = str.indexOf('*');
    while (p > -1) {
        str.replace(p, 1, '%');
        p = str.indexOf('*');
    }

    p = str.indexOf('?');
    while (p > -1) {
        str.replace(p, 1, '_');
        p = str.indexOf('?');
    }
    return str;
}

bool DatabaseDialog::optionsDoNext()
{
    if (m_operator_1->currentIndex() == 4) {
        if ((m_operatorValue_1->text().indexOf('*') != -1) || (m_operatorValue_1->text().indexOf('?') != -1)) {
            // xgettext: no-c-format
            int res = KMessageBox::warningTwoActions(this,
                                                     i18n("'*' or '?' are not valid wildcards in SQL. "
                                                          "The proper replacements are '%' or '_'. Do you want to replace them?"),
                                                     {},
                                                     KGuiItem(i18nc("@action:button", "Replace")),
                                                     KStandardGuiItem::cancel());

            if (res == KMessageBox::PrimaryAction)
                m_operatorValue_1->setText(exchangeWildcards(m_operatorValue_1->text()));
        }
    }

    if (m_operator_2->currentIndex() == 4) {
        if ((m_operatorValue_2->text().indexOf('*') != -1) || (m_operatorValue_2->text().indexOf('?') != -1)) {
            // xgettext: no-c-format
            int res = KMessageBox::warningTwoActions(this,
                                                     i18n("'*' or '?' are not valid wildcards in SQL. "
                                                          "The proper replacements are '%' or '_'. Do you want to replace them?"),
                                                     {},
                                                     KGuiItem(i18nc("@action:button", "Replace")),
                                                     KStandardGuiItem::cancel());

            if (res == KMessageBox::PrimaryAction)
                m_operatorValue_2->setText(exchangeWildcards(m_operatorValue_2->text()));
        }
    }

    if (m_operator_3->currentIndex() == 4) {
        if ((m_operatorValue_3->text().indexOf('*') != -1) || (m_operatorValue_3->text().indexOf('?') != -1)) {
            // xgettext: no-c-format
            int res = KMessageBox::warningTwoActions(this,
                                                     i18n("'*' or '?' are not valid wildcards in SQL. "
                                                          "The proper replacements are '%' or '_'. Do you want to replace them?"),
                                                     {},
                                                     KGuiItem(i18nc("@action:button", "Replace")),
                                                     KStandardGuiItem::cancel());

            if (res == KMessageBox::PrimaryAction)
                m_operatorValue_3->setText(exchangeWildcards(m_operatorValue_3->text()));
        }
    }

    QString query("SELECT ");

    if (m_distinct->isChecked())
        query += "DISTINCT ";

    int i;
    int l = m_columns_1->count() - 1;
    for (i = 0; i < l; ++i) {
        query += m_columns_1->itemText(i) + ", ";
    }
    query += m_columns_1->itemText(l) +

        "\nFROM ";

    bool b = false;
    for (int i = 0; i < m_tableView->count(); ++i) {
        QListWidgetItem *item = m_tableView->item(i);
        if (item->checkState() == Qt::Checked) {
            if (b)
                query += ", ";
            b = true;
            query += item->text();
        }
    }

    if ((!m_operatorValue_1->text().isEmpty()) || (!m_operatorValue_2->text().isEmpty()) || (!m_operatorValue_3->text().isEmpty()))
        query += "\nWHERE ";

    bool added = false;
    if (!m_operatorValue_1->text().isEmpty()) {
        query += getWhereCondition(m_columns_1->currentText(), m_operatorValue_1->text(), m_operator_1->currentIndex());
        added = true;
    }

    if (!m_operatorValue_2->text().isEmpty()) {
        if (added)
            query += (m_andBox->isChecked() ? " AND " : " OR ");

        query += getWhereCondition(m_columns_2->currentText(), m_operatorValue_2->text(), m_operator_2->currentIndex());
        added = true;
    }

    if (!m_operatorValue_3->text().isEmpty()) {
        if (added)
            query += (m_andBox->isChecked() ? " AND " : " OR ");

        query += getWhereCondition(m_columns_3->currentText(), m_operatorValue_3->text(), m_operator_3->currentIndex());
    }

    if ((m_columnsSort_1->currentIndex() != 0) || (m_columnsSort_2->currentIndex() != 0)) {
        query += "\nORDER BY ";
        bool added = false;
        if (m_columnsSort_1->currentIndex() != 0) {
            added = true;
            query += m_columnsSort_1->currentText();
            if (m_sortMode_1->currentIndex() == 1)
                query += " DESC ";
        }

        if (m_columnsSort_2->currentIndex() != 0) {
            if (added)
                query += ", ";

            query += m_columnsSort_2->currentText();
            if (m_sortMode_2->currentIndex() == 1)
                query += " DESC ";
        }
    }

    m_queryString = query;
    m_sqlQuery->setText(query);

    return true;
}

void DatabaseDialog::orBox_clicked()
{
    m_andBox->setChecked(false);
    m_orBox->setChecked(true);
}

void DatabaseDialog::andBox_clicked()
{
    m_andBox->setChecked(true);
    m_orBox->setChecked(false);
}

void DatabaseDialog::startingCell_clicked()
{
    m_startingCell->setChecked(true);
    m_startingRegion->setChecked(false);
}

void DatabaseDialog::startingRegion_clicked()
{
    m_startingCell->setChecked(false);
    m_startingRegion->setChecked(true);
}

void DatabaseDialog::connectButton_clicked()
{
    warnSheets << "DatabaseDialog::connectButton_clicked(): Not implemented yet!";
}

void DatabaseDialog::databaseNameChanged(const QString &)
{
    enableButtons();
}

void DatabaseDialog::databaseHostChanged(const QString &)
{
    enableButtons();
}

void DatabaseDialog::databaseDriverChanged(int)
{
}

#endif // QT_NO_SQL
