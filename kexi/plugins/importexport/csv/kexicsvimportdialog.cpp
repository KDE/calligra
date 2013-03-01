/* This file is part of the KDE project
   Copyright (C) 2005-2012 Jarosław Staniek <staniek@kde.org>
   Copyright (C) 2012 Oleg Kukharchuk <oleg.kuh@gmail.com>

   This work is based on kspread/dialogs/kspread_dlg_csv.cc
   and will be merged back with Calligra Libraries.

   Copyright (C) 2002-2003 Norbert Andres <nandres@web.de>
   Copyright (C) 2002-2003 Ariya Hidayat <ariya@kde.org>
   Copyright (C) 2002 Laurent Montel <montel@kde.org>
   Copyright (C) 1999 David Faure <faure@kde.org>

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

#include "kexicsvimportdialog.h"

#include <QButtonGroup>
#include <QCheckBox>
#include <QClipboard>
#include <QLabel>
#include <QLineEdit>
#include <QMimeSource>
#include <QPushButton>
#include <QRadioButton>
#include <QStandardItemModel>
#include <QItemSelectionModel>
#include <QHeaderView>
#include <QTableView>
#include <QFileDialog>
#include <QPainter>
#include <QTextCodec>
#include <QTimer>
#include <QFontMetrics>
#include <QVBoxLayout>
#include <QKeyEvent>
#include <QEvent>
#include <QTextStream>
#include <QGridLayout>
#include <QPixmap>
#include <QToolTip>
#include <QStackedWidget>
#include <QSplitter>
#include <QTreeView>

#include <kapplication.h>
#include <kdebug.h>
#include <kdialog.h>
#include <kfiledialog.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kglobalsettings.h>
#include <kcharsets.h>
#include <knuminput.h>
#include <KProgressDialog>

#include <KoIcon.h>

#include <kexiutils/identifier.h>
#include <kexiutils/utils.h>
#include <core/kexi.h>
#include <core/kexiproject.h>
#include <core/kexipart.h>
#include <core/kexipartinfo.h>
#include <core/KexiMainWindowIface.h>
#include <core/kexiguimsghandler.h>
#include <core/KexiWindow.h>
#include <db/connection.h>
#include <db/tableschema.h>
#include <db/transaction.h>
#include <db/tristate.h>
#include <db/utils.h>
#include <widget/kexicharencodingcombobox.h>
#include <widget/KexiFileWidget.h>
#include <kexiutils/KexiCommandLinkButton.h>
#include <widget/KexiNameWidget.h>
#include <widget/navigator/KexiProjectNavigator.h>
#include <widget/navigator/KexiProjectTreeView.h>
#include <kexidb/dbobjectnamevalidator.h>
#include <widget/fields/KexiFieldListView.h>
#include <widget/fields/KexiFieldListModel.h>

#include "kexicsvwidgets.h"
#include <kexi_global.h>

#define _IMPORT_ICON koIconNeededWithSubs("change to file_import or so", "file_import","table")

//! @internal
class KexiCSVImportStatic
{
public:
    KexiCSVImportStatic()
     : types(QVector<KexiDB::Field::Type>()
        << KexiDB::Field::Text
        << KexiDB::Field::Integer
        << KexiDB::Field::Double
        << KexiDB::Field::Boolean
        << KexiDB::Field::Date
        << KexiDB::Field::Time
        << KexiDB::Field::DateTime)
    {
        typeNames.insert(KexiDB::Field::Text, KexiDB::Field::typeGroupName(KexiDB::Field::TextGroup));
        typeNames.insert(KexiDB::Field::Integer, KexiDB::Field::typeGroupName(KexiDB::Field::IntegerGroup));
        typeNames.insert(KexiDB::Field::Double, KexiDB::Field::typeGroupName(KexiDB::Field::FloatGroup));
        typeNames.insert(KexiDB::Field::Boolean, KexiDB::Field::typeName(KexiDB::Field::Boolean));
        typeNames.insert(KexiDB::Field::Date, KexiDB::Field::typeName(KexiDB::Field::Date));
        typeNames.insert(KexiDB::Field::Time, KexiDB::Field::typeName(KexiDB::Field::Time));
        typeNames.insert(KexiDB::Field::DateTime, KexiDB::Field::typeName(KexiDB::Field::DateTime));
        for (int i = 0; i < types.size(); ++i) {
            indicesForTypes.insert(types[i], i);
        }
    }

    const QVector<KexiDB::Field::Type> types;
    QHash<KexiDB::Field::Type, QString> typeNames;
    QHash<KexiDB::Field::Type, int> indicesForTypes;
};

K_GLOBAL_STATIC(KexiCSVImportStatic, kexiCSVImportStatic)

#define MAX_ROWS_TO_PREVIEW 100 //max 100 rows is reasonable
#define MAX_BYTES_TO_PREVIEW 10240 //max 10KB is reasonable
#define MAX_CHARS_TO_SCAN_WHILE_DETECTING_DELIMITER 4096
#define MINIMUM_YEAR_FOR_100_YEAR_SLIDING_WINDOW 1930
#define PROGRESS_STEP_MS (1000/5) // 5 updates per second

class KexiCSVImportDialogModel : public QStandardItemModel
{
public:
    KexiCSVImportDialogModel(QObject *parent)
        : QStandardItemModel(parent)
        , m_1stRowForFieldNames(true)
    {
    }

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const
    {

        QVariant value = QStandardItemModel::data(index, role);

        switch (role) {
        case Qt::FontRole:
            if (index.row() == 0) {
                QFont f(value.value<QFont>());
                f.setBold(true);
                return qVariantFromValue(f);
            }
        case Qt::EditRole:
        case Qt::DisplayRole:
            if (index.row() == 0) {
                if (!m_columnNames[index.column()].isEmpty() && m_1stRowForFieldNames) {
                    return m_columnNames[index.column()];
                } else {
                    return i18n("Column %1", index.column() + 1);
                }
            }
        }
        return value;
    }

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const
    {
        QVariant value = QStandardItemModel::headerData(section, orientation, role);
        if (orientation == Qt::Vertical && role == Qt::DisplayRole) {
            if (section == 0) {
                return i18nc("@title:row", "Column name") + "  ";
            } else {
                return QString::number(section);
            }
        }
        return value;
    }

    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole)
    {
        if (index.row() == 0 && m_1stRowForFieldNames && role == Qt::EditRole) {
            m_columnNames[index.column()] = value.toString();
            return true;
        }
        return QStandardItemModel::setData(index, value, role);
    }

    void setColumnCount(int col)
    {
        if (m_columnNames.size()<col) {
            m_columnNames.resize(col);
        }
        QStandardItemModel::setColumnCount(col);
    }

    bool firstRowForFieldNames() const
    {
        return m_1stRowForFieldNames;
    }

    void setFirstRowForFieldNames(bool flag)
    {
        m_1stRowForFieldNames = flag;
    }

private:
    bool m_1stRowForFieldNames;
    QVector<QString> m_columnNames;
};

//! Helper used to temporary disable keyboard and mouse events
void installRecursiveEventFilter(QObject *filter, QObject *object)
{
    object->installEventFilter(filter);
    QList<QObject*> list(object->children());
    foreach(QObject *obj, list) {
        installRecursiveEventFilter(filter, obj);
    }
}

static bool shouldSaveRow(int row, bool firstRowForFieldNames)
{
    return row > (firstRowForFieldNames ? 1 : 0);
}

// --

class KexiCSVImportDialog::Private
{
public:
    Private() {}
    ~Private() {
        qDeleteAll(m_uniquenessTest);
    }

    void clearDetectedTypes() {
        m_detectedTypes.clear();
    }

    void clearUniquenessTests() {
        qDeleteAll(m_uniquenessTest);
        m_uniquenessTest.clear();
    }

    KexiDB::Field::Type detectedType(int col) const
    {
        return m_detectedTypes.value(col, KexiDB::Field::InvalidType);
    }

    void setDetectedType(int col, KexiDB::Field::Type type)
    {
        if (m_detectedTypes.count() <= col) {
            for (int i = m_detectedTypes.count(); i < col; ++i) { // append missing bits
                m_detectedTypes.append(KexiDB::Field::InvalidType);
            }
            m_detectedTypes.append(type);
        }
        else {
            m_detectedTypes[col] = type;
        }
    }

    QList<int>* uniquenessTest(int col) const
    {
        return m_uniquenessTest.value(col);
    }

    void setUniquenessTest(int col, QList<int>* test)
    {
        if (m_uniquenessTest.count() <= col) {
            for (int i = m_uniquenessTest.count(); i < col; ++i) { // append missing bits
                m_uniquenessTest.append(0);
            }
            m_uniquenessTest.append(test);
        }
        else {
            m_uniquenessTest[col] = test;
        }
    }
private:
    //! vector of detected types
    //! @todo more types
    QList<KexiDB::Field::Type> m_detectedTypes;

    //! m_detectedUniqueColumns[i]==true means that i-th column has unique values
    //! (only for numeric type)
    QList< QList<int>* > m_uniquenessTest;
};

// --

KexiCSVImportDialog::KexiCSVImportDialog(Mode mode, QWidget * parent)
        : KAssistantDialog(parent),
        m_cancelled(false),
        m_adjustRows(true),
        m_startline(0),
        m_textquote(QString(KEXICSV_DEFAULT_FILE_TEXT_QUOTE)[0]),
        m_mode(mode),
        m_columnsAdjusted(false),
        m_firstFillTableCall(true),
        m_blockUserEvents(false),
        m_primaryKeyColumn(-1),
        m_dialogCancelled(false),
        m_conn(0),
        m_fieldsListModel(0),
        m_destinationTableSchema(0),
        m_implicitPrimaryKeyAdded(false),
        m_allRowsLoadedInPreview(false),
        m_stoppedAt_MAX_BYTES_TO_PREVIEW(false),
        m_stringNo("no"),
        m_stringI18nNo(i18n("no")),
        m_stringFalse("false"),
        m_stringI18nFalse(i18n("false")),
        m_newTable(false),
        m_partItemForSavedTable(0),
        m_importInProgress(false),
        m_importCancelled(false),
        d(new Private)
{
   // setWindowFlags(windowFlags() | Qt::WStyle_Maximize | Qt::WStyle_SysMenu);
    setWindowTitle( mode == File
        ? i18n("Import CSV Data From File"): i18n("Paste CSV Data From Clipboard") );
    setWindowIcon(_IMPORT_ICON);
//! @todo use "Paste CSV Data From Clipboard" caption for mode==Clipboard
    //setButtons((mode == File ? User1 : (ButtonCode)0) | Ok | Cancel);
    //setDefaultButton(Ok);
    setObjectName("KexiCSVImportDialog");
    setSizeGripEnabled(true);
    adjustSize();
    KDialog::centerOnScreen(this);

    setButtons(KDialog::Help | KDialog::User1 | KDialog::User2 | KDialog::User3 | KDialog::Apply | KDialog::Cancel);
    setButtonGuiItem(KDialog::Apply, KGuiItem(i18n("&Import..."), _IMPORT_ICON));
    setButtonGuiItem(KDialog::User1, KStandardGuiItem::open());
    setButtonText( KDialog::User2, i18nc("Opposite to Back", "Next") );
    setButtonIcon( KDialog::User2, KStandardGuiItem::forward(KStandardGuiItem::UseRTL).icon() );
    setButtonGuiItem(KDialog::User3, KStandardGuiItem::back(KStandardGuiItem::UseRTL));
    setButtonGuiItem(KDialog::Help, KStandardGuiItem::configure());

    showButton(KDialog::Apply, false);
    showButton(KDialog::User1, false);
    showButton(KDialog::User3, false);

    KConfigGroup importExportGroup(KGlobal::config()->group("ImportExport"));
    m_maximumRowsForPreview = importExportGroup.readEntry(
                                  "MaximumRowsForPreviewInImportDialog", MAX_ROWS_TO_PREVIEW);
    m_maximumBytesForPreview = importExportGroup.readEntry(
                                   "MaximumBytesForPreviewInImportDialog", MAX_BYTES_TO_PREVIEW);
    m_minimumYearFor100YearSlidingWindow = importExportGroup.readEntry(
        "MinimumYearFor100YearSlidingWindow", MINIMUM_YEAR_FOR_100_YEAR_SLIDING_WINDOW);

    m_pkIcon = koSmallIcon("key");

// m_encoding = QString::fromLatin1(KGlobal::locale()->encoding());
// m_trimmedInTextValuesChecked = true;

    if (m_mode == File) {
        showButton(KDialog::Help, false);
        createFileOpenPage();
    } else if (m_mode == Clipboard) {
        QString subtype("plain");
        m_clipboardData = QApplication::clipboard()->text(subtype, QClipboard::Clipboard);
        /* debug
            for (int i=0;QApplication::clipboard()->data(QClipboard::Clipboard)->format(i);i++)
              kDebug() << i << ": "
                << QApplication::clipboard()->data(QClipboard::Clipboard)->format(i);
        */

    } else {
        return;
    }


    m_file = 0;
    m_inputStream = 0;

    createOptionsPage();
    createImportMethodPage();
    createTableNamePage();
    createImportPage();

    /** @todo reuse Clipboard too! */

    /*if ( m_mode == Clipboard )
      {
      setCaption( i18n( "Inserting From Clipboard" ) );
      QMimeSource * mime = QApplication::clipboard()->data();
      if ( !mime )
      {
        KMessageBox::information( this, i18n("There is no data in the clipboard.") );
        m_cancelled = true;
        return;
      }

      if ( !mime->provides( "text/plain" ) )
      {
        KMessageBox::information( this, i18n("There is no usable data in the clipboard.") );
        m_cancelled = true;
        return;
      }
      m_fileArray = QByteArray(mime->encodedData( "text/plain" ) );
      }
      else if ( mode == File )
      {*/
    m_dateRegExp = QRegExp("(\\d{1,4})([/\\-\\.])(\\d{1,2})([/\\-\\.])(\\d{1,4})");
    m_timeRegExp1 = QRegExp("(\\d{1,2}):(\\d{1,2}):(\\d{1,2})");
    m_timeRegExp2 = QRegExp("(\\d{1,2}):(\\d{1,2})");
    m_fpNumberRegExp1 = QRegExp("[\\-]{0,1}\\d*[,\\.]\\d+");
    // E notation, e.g. 0.1e2, 0.1e+2, 0.1e-2, 0.1E2, 0.1E+2, 0.1E-2
    m_fpNumberRegExp2 = QRegExp("[\\-]{0,1}\\d*[,\\.]\\d+[Ee][+-]{0,1}\\d+");
    QString caption(i18n("Open CSV Data File"));


    m_loadingProgressDlg = 0;
   /* if (m_mode == File) {
        m_loadingProgressDlg = new KProgressDialog(
            this, i18n("Loading CSV Data"),
            i18n("Loading CSV Data from \"%1\"...", QDir::toNativeSeparators(m_fname)));
        m_loadingProgressDlg->setObjectName("m_loadingProgressDlg");
        m_loadingProgressDlg->setModal(true);
        m_loadingProgressDlg->progressBar()->setMaximum(m_maximumRowsForPreview);
        m_loadingProgressDlg->show();
    }
*/
    if (m_mode == Clipboard) {
        m_infoLbl->setIcon(koIconName("edit-paste"));
    }
    //updateRowCountInfo();

    m_tableView->setSelectionMode(QAbstractItemView::NoSelection);

    connect(m_formatCombo, SIGNAL(activated(int)),
            this, SLOT(formatChanged(int)));
    connect(m_delimiterWidget, SIGNAL(delimiterChanged(const QString&)),
            this, SLOT(delimiterChanged(const QString&)));
    connect(m_startAtLineSpinBox, SIGNAL(valueChanged(int)),
            this, SLOT(startlineSelected(int)));
    connect(m_comboQuote, SIGNAL(activated(int)),
            this, SLOT(textquoteSelected(int)));
    connect(m_tableView->selectionModel(), SIGNAL(currentChanged(QModelIndex, QModelIndex)),
            this, SLOT(currentCellChanged(QModelIndex, QModelIndex)));
    connect(m_ignoreDuplicates, SIGNAL(stateChanged(int)),
            this, SLOT(ignoreDuplicatesChanged(int)));
    connect(m_1stRowForFieldNames, SIGNAL(stateChanged(int)),
            this, SLOT(slot1stRowForFieldNamesChanged(int)));

    connect(this, SIGNAL(helpClicked()), this, SLOT(optionsButtonClicked()));
    connect(this, SIGNAL(applyClicked()), this, SLOT(import()));

    connect(this, SIGNAL(currentPageChanged(KPageWidgetItem*, KPageWidgetItem*)),
            this, SLOT(slotCurrentPageChanged(KPageWidgetItem*,KPageWidgetItem*)));

    installRecursiveEventFilter(this, this);
    if ( m_mode == Clipboard )
        initLater();
}

KexiCSVImportDialog::~KexiCSVImportDialog()
{
    delete m_file;
    delete m_inputStream;
    delete d;
}

void KexiCSVImportDialog::next()
{
    KPageWidgetItem *curPage = currentPage();

    if (curPage == m_openFilePage) {
        m_fname = m_openFileWidget->highlightedFile();
        if (m_fname.isEmpty()) {
            KMessageBox::sorry(this, i18nc("@info", "Select source filename."));
            return;
        }
        if (!openData()) {
            return;
        }
    } else if (curPage == m_optionsPage) {
        const uint numRows(m_table->rowCount());
        if (numRows == 0)
            return; //impossible

        if (numRows == 1) {
            if (KMessageBox::No == KMessageBox::questionYesNo(this,
                i18n("Data set contains no rows. Do you want to import empty table?")))
            return;
        }
    } else if (curPage == m_tableNamePage) {
        KexiGUIMessageHandler msg;
        KexiProject *project = KexiMainWindowIface::global()->project();
        if (!project) {
            msg.showErrorMessage(i18n("No project available."));
            return;
        }

        m_conn = project->dbConnection();
        if (!m_conn) {
            msg.showErrorMessage(i18n("No database connection available."));
            return;
        }
        if (m_newTable) {
            m_partItemForSavedTable->setCaption(m_newTableWidget->captionText());
            m_partItemForSavedTable->setName(m_newTableWidget->nameText());

            KexiPart::Part *part = Kexi::partManager().partForClass("org.kexi-project.table");
            KexiDB::SchemaData tmp;
            tristate res = m_conn->loadObjectSchemaData(
                    project->idForClass(part->info()->partClass()),
                    m_newTableWidget->nameText(),
                    tmp);
            if (res == true) {
                KMessageBox::information(this,
                        "<p>"
                        + part->i18nMessage("Object \"%1\" already exists.", 0)
                        .subs(m_newTableWidget->nameText()).toString()
                        + "</p><p>" + i18n("Please choose other name.") + "</p>"
                        );
                return;
            }
        } else {
            m_partItemForSavedTable = m_tablesList->selectedPartItem();
        }
    }
    KAssistantDialog::next();
}

void KexiCSVImportDialog::slotShowSchema(KexiPart::Item *item)
{
    kDebug()<<"after emit";
    if (!item) {
        return;
    }

    enableButton(KDialog::User2, true);
    KexiDB::TableOrQuerySchema *tableOrQuery = new KexiDB::TableOrQuerySchema(
            KexiMainWindowIface::global()->project()->dbConnection(),
            item->identifier()
            );
    m_tableCaptionLabel->setText(tableOrQuery->captionOrName());
    m_tableNameLabel->setText(tableOrQuery->name());
    m_rowCountLabel->setText(QString::number(KexiDB::rowCount(*tableOrQuery)));
    m_colCountLabel->setText(QString::number(tableOrQuery->fieldCount()));

    delete m_fieldsListModel;
    m_fieldsListModel = new KexiFieldListModel(m_fieldsListView, ShowDataTypes);
    m_fieldsListModel->setSchema(tableOrQuery);
    m_fieldsListView->setModel(m_fieldsListModel);
}

void KexiCSVImportDialog::slotCurrentPageChanged(KPageWidgetItem *page, KPageWidgetItem *prev)
{
    enableButton(KDialog::User2, (page == m_saveMethodPage ? false : true));
    showButton(KDialog::Apply, (page == m_importPage ? true : false));
    showButton(KDialog::Help, (page == m_optionsPage ? true : false));
    showButton(KDialog::User2, (page == m_importPage ? false : true));
    showButton(KDialog::User3, (page == m_openFilePage ? false : true));

    if (page == m_saveMethodPage && prev == m_tableNamePage && m_partItemForSavedTable) {
        if (m_newTable) {
            KexiMainWindowIface::global()->project()->deleteUnstoredItem(m_partItemForSavedTable);
        }
        m_partItemForSavedTable = 0;
    }
    if(page == m_optionsPage){
        if (m_mode == File) {
            m_loadingProgressDlg = new KProgressDialog(
                    this, i18nc("@title:window", "Loading CSV Data"),
                    i18nc("@info", "Loading CSV Data from <filename>%1</filename>...", QDir::toNativeSeparators(m_fname)));
            m_loadingProgressDlg->setObjectName("m_loadingProgressDlg");
            m_loadingProgressDlg->setModal(true);
            m_loadingProgressDlg->progressBar()->setMaximum(m_maximumRowsForPreview);
            m_loadingProgressDlg->show();
        }
        // delimiterChanged(detectedDelimiter); // this will cause fillTable()
        m_detectDelimiter = true;
        m_columnsAdjusted = false;
        fillTable();
        delete m_loadingProgressDlg;
        m_loadingProgressDlg = 0;
        if (m_dialogCancelled) {
            // m_loadingProgressDlg->hide();
            // m_loadingProgressDlg->close();
            QTimer::singleShot(0, this, SLOT(reject()));
            return;
        }

        currentCellChanged(m_table->index(0,0), QModelIndex());

        if (m_loadingProgressDlg)
            m_loadingProgressDlg->hide();
        m_tableView->setFocus();
    } else if (page == m_saveMethodPage) {
        m_newTableButton->setFocus();
    } else if (page == m_tableNamePage) {
        if (m_newTable && !m_partItemForSavedTable) {
            KexiGUIMessageHandler msg;
            KexiProject *project = KexiMainWindowIface::global()->project();
            //get suggested name based on the file name
            QString suggestedName;
            if (m_mode == File) {
                suggestedName = KUrl(m_fname).fileName();
                //remove extension
                if (!suggestedName.isEmpty()) {
                    const int idx = suggestedName.lastIndexOf(".");
                    if (idx != -1) {
                         suggestedName = suggestedName.mid(0, idx).simplified();
                    }
                }
            }
            KexiPart::Part *part = Kexi::partManager().partForClass("org.kexi-project.table");
            if (!part) {
                msg.showErrorMessage(&Kexi::partManager());
                return;
            }
            //-new part item
            m_partItemForSavedTable = project->createPartItem(part->info(), suggestedName);
            if (!m_partItemForSavedTable) {
                 msg.showErrorMessage(project);
                 return;
            }
            m_newTableWidget->setCaptionText(m_partItemForSavedTable->caption());
            m_newTableWidget->setNameText(m_partItemForSavedTable->name());
            m_newTableWidget->captionLineEdit()->setFocus();
            m_newTableWidget->captionLineEdit()->selectAll();
        } else if (!m_newTable) {
            KexiPart::Item *i = m_tablesList->selectedPartItem();
            if (!i) {
                enableButton(KDialog::User2, false);
            }
            slotShowSchema(i);
        }
    } else if (page == m_importPage) {
        m_fromLabel->setFileName(m_fname);
        m_toLabel->setLabelText(i18n("To table:")+"\n\n"+m_partItemForSavedTable->caption());
        m_importingProgressBar->hide();
        m_importProgressLabel->hide();

    }
 }

void KexiCSVImportDialog::createFileOpenPage()
{
    m_openFileWidget = new KexiFileWidget(
        KUrl("kfiledialog:///CSVImportExport"), //startDir
        KexiFileWidget::Custom | KexiFileWidget::Opening,
        this);
    m_openFileWidget->setObjectName("m_openFileWidget");
    m_openFileWidget->setAdditionalFilters(csvMimeTypes().toSet());
    m_openFileWidget->setDefaultExtension("csv");
    connect(m_openFileWidget, SIGNAL(fileSelected(const KUrl &)), this, SLOT(next()));
    m_openFilePage = new KPageWidgetItem(m_openFileWidget, i18n("Select file name for import"));
    addPage(m_openFilePage);
}

void KexiCSVImportDialog::createOptionsPage()
{
    QWidget *m_optionsWidget = new QWidget(this);
    //setMainWidget(plainPage);

    QVBoxLayout *lyr = new QVBoxLayout(m_optionsWidget);

    m_infoLbl = new KexiCSVInfoLabel(
        m_mode == File ? i18n("Preview of data from file:")
        : i18n("Preview of data from clipboard"),
        m_optionsWidget, m_mode == File /*showFnameLine*/
    );
    lyr->addWidget(m_infoLbl);

    QWidget* page = new QFrame(m_optionsWidget);
    QGridLayout *glyr = new QGridLayout(page);
    lyr->addWidget(page);

    // Delimiter: comma, semicolon, tab, space, other
    m_delimiterWidget = new KexiCSVDelimiterWidget(true /*lineEditOnBottom*/, page);
    glyr->addWidget(m_delimiterWidget, 1, 0, 2, 1);

    QLabel *delimiterLabel = new QLabel(i18n("Delimiter:"), page);
    delimiterLabel->setBuddy(m_delimiterWidget);
    delimiterLabel->setAlignment(Qt::AlignLeft | Qt::AlignBottom);
    glyr->addWidget(delimiterLabel, 0, 0, 1, 1);

    // Format: number, text...
//! @todo Object and Currency types
    m_formatCombo = new KComboBox(page);
    m_formatCombo->setObjectName("m_formatCombo");

    for (int i = 0; i < kexiCSVImportStatic->types.size(); ++i) {
        m_formatCombo->addItem(kexiCSVImportStatic->typeNames.value(kexiCSVImportStatic->types[i]));
    }

    glyr->addWidget(m_formatCombo, 1, 1, 1, 1);

    m_formatLabel = new QLabel(page);
    m_formatLabel->setBuddy(m_formatCombo);
    m_formatLabel->setAlignment(Qt::AlignLeft | Qt::AlignBottom);
    glyr->addWidget(m_formatLabel, 0, 1);

    m_primaryKeyField = new QCheckBox(i18n("Primary key"), page);
    m_primaryKeyField->setObjectName("m_primaryKeyField");
    glyr->addWidget(m_primaryKeyField, 2, 1);
    connect(m_primaryKeyField, SIGNAL(toggled(bool)), this, SLOT(slotPrimaryKeyFieldToggled(bool)));

    m_comboQuote = new KexiCSVTextQuoteComboBox(page);
    glyr->addWidget(m_comboQuote, 1, 2);

    TextLabel2 = new QLabel(i18n("Text quote:"), page);
    TextLabel2->setBuddy(m_comboQuote);
    TextLabel2->setObjectName("TextLabel2");
    TextLabel2->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    TextLabel2->setAlignment(Qt::AlignLeft | Qt::AlignBottom);
    glyr->addWidget(TextLabel2, 0, 2);

    m_startAtLineSpinBox = new KIntSpinBox(page);
    m_startAtLineSpinBox->setObjectName("m_startAtLineSpinBox");
    m_startAtLineSpinBox->setMinimum(1);
    m_startAtLineSpinBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_startAtLineSpinBox->setMinimumWidth(
        QFontMetrics(m_startAtLineSpinBox->font()).width("8888888"));
    glyr->addWidget(m_startAtLineSpinBox, 1, 3);

    m_startAtLineLabel = new QLabel(page);
    m_startAtLineLabel->setBuddy(m_startAtLineSpinBox);
    m_startAtLineLabel->setObjectName("m_startAtLineLabel");
    m_startAtLineLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    m_startAtLineLabel->setAlignment(Qt::AlignLeft | Qt::AlignBottom);
    glyr->addWidget(m_startAtLineLabel, 0, 3);

    m_ignoreDuplicates = new QCheckBox(page);
    m_ignoreDuplicates->setObjectName("m_ignoreDuplicates");
    m_ignoreDuplicates->setText(i18n("Ignore duplicated delimiters"));
    glyr->addWidget(m_ignoreDuplicates, 2, 2, 1, 2);

    m_1stRowForFieldNames = new QCheckBox(page);
    m_1stRowForFieldNames->setObjectName("m_1stRowForFieldNames");
    m_1stRowForFieldNames->setText(i18n("First row contains column names"));
    glyr->addWidget(m_1stRowForFieldNames, 3, 2, 1, 2);

    QSpacerItem* spacer_2 = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Preferred);
    glyr->addItem(spacer_2, 0, 4, 4, 1);
    glyr->setColumnStretch(4, 2);

    m_tableView = new QTableView(m_optionsWidget);
    m_table = new KexiCSVImportDialogModel(m_tableView);
    m_table->setObjectName("m_table");
    m_tableView->setModel(m_table);
    lyr->addWidget(m_tableView);

    QSizePolicy spolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    spolicy.setHorizontalStretch(1);
    spolicy.setVerticalStretch(1);
    m_tableView->setSizePolicy(spolicy);

    m_optionsPage=new KPageWidgetItem(m_optionsWidget, i18n("Import Options"));
    addPage(m_optionsPage);
}

void KexiCSVImportDialog::createImportMethodPage()
{
    m_saveMethodWidget = new QWidget(this);
    QGridLayout *l = new QGridLayout(m_saveMethodWidget);

    m_newTableButton = new KexiCommandLinkButton(i18nc("@action:button", "New table"), 
            i18nc("CSV import: data will be appended to a new table", "Data will be appended to a new table"), m_saveMethodWidget);
    m_newTableButton->setArrowVisible(true);
    m_existentTableButton = new KexiCommandLinkButton(i18nc("@action:button", "Existing table"), 
            i18nc("CSV import: data will be appended to existing table", "Data will be appended to existing table"), m_saveMethodWidget);
    m_existentTableButton->setArrowVisible(true);
    l->addWidget(m_newTableButton, 0, 0, 1, 1);
    l->addWidget(m_existentTableButton, 1, 0, 1, 1);

    QSpacerItem *hSpacer = new QSpacerItem(200, 20, QSizePolicy::Preferred, QSizePolicy::Minimum);
    QSpacerItem *vSpacer = new QSpacerItem(20, 200, QSizePolicy::Minimum, QSizePolicy::Expanding);

    l->addItem(hSpacer, 1, 1, 1, 1);
    l->addItem(vSpacer, 2, 0, 1, 1);

    m_saveMethodPage = new KPageWidgetItem(m_saveMethodWidget, i18n("Choose Method of Saving Imported Data"));
    addPage(m_saveMethodPage);

    connect(m_newTableButton, SIGNAL(clicked()), this, SLOT(slotCommandLinkClicked()));
    connect(m_existentTableButton, SIGNAL(clicked()), this, SLOT(slotCommandLinkClicked()));
}

void KexiCSVImportDialog::createTableNamePage()
{
    m_tableNameWidget = new QStackedWidget(this);
    m_tableNameWidget->setObjectName("m_tableNameWidget");
    QWidget *page1=new QWidget(m_tableNameWidget);
    m_newTableWidget = new KexiNameWidget(QString(), page1);
    m_newTableWidget->addNameSubvalidator(new KexiDB::ObjectNameValidator(
                KexiMainWindowIface::global()->project()->dbConnection()->driver()));
    QVBoxLayout *l=new QVBoxLayout(page1);
    l->addWidget(m_newTableWidget);
    l->addStretch(1);
    m_tableNameWidget->addWidget(page1);

    QSplitter *page2 = new QSplitter(m_tableNameWidget);
    QWidget *tableDetailsWidget = new QWidget(page2);
    m_tablesList = new KexiProjectNavigator(page2, KexiProjectNavigator::NoFeatures);
    QString errorString;
    m_tablesList->setProject(KexiMainWindowIface::global()->project(), "org.kexi-project.table", &errorString, false);
    connect (m_tablesList, SIGNAL(openOrActivateItem(KexiPart::Item*, Kexi::ViewMode)),
            this, SLOT(next()));
    connect (m_tablesList, SIGNAL(selectionChanged(KexiPart::Item*)),
            this, SLOT(slotShowSchema(KexiPart::Item*)));
    page2->addWidget(m_tablesList);

    QLabel *captionLbl = new QLabel(i18nc("@label", "Caption:"), tableDetailsWidget);
    QLabel *nameLbl = new QLabel(i18nc("@label", "Name:"), tableDetailsWidget);
    QLabel *rowCntLbl = new QLabel(i18nc("@label", "Row count:"), tableDetailsWidget);
    QLabel *colCntLbl = new QLabel(i18nc("@label", "Column count:"), tableDetailsWidget);

    m_tableNameLabel = new QLabel(tableDetailsWidget);
    m_tableCaptionLabel = new QLabel(tableDetailsWidget);
    m_rowCountLabel = new QLabel(tableDetailsWidget);
    m_colCountLabel = new QLabel(tableDetailsWidget);

    QGridLayout *gridLayout = new QGridLayout(tableDetailsWidget);
    gridLayout->addWidget(captionLbl, 0, 0);
    gridLayout->addWidget(m_tableCaptionLabel, 0, 1);
    gridLayout->addWidget(nameLbl, 1, 0);
    gridLayout->addWidget(m_tableNameLabel, 1, 1);
    gridLayout->addWidget(rowCntLbl, 2, 0);
    gridLayout->addWidget(m_rowCountLabel, 2, 1);
    gridLayout->addWidget(colCntLbl, 3, 0);
    gridLayout->addWidget(m_colCountLabel, 3, 1);

    m_fieldsListView = new QTreeView(tableDetailsWidget);
    m_fieldsListView->setItemsExpandable(false);
    m_fieldsListView->setRootIsDecorated(false);

    gridLayout->addWidget(m_fieldsListView, 4, 0, 4, 2);
    gridLayout->setRowStretch(4, 1);

    page2->addWidget(tableDetailsWidget);
    m_tableNameWidget->addWidget(page2);
    m_tableNamePage = new KPageWidgetItem(m_tableNameWidget, i18nc("@label", "Choose Name of Destination Table"));
    addPage(m_tableNamePage);
}

void KexiCSVImportDialog::createImportPage()
{

    m_importWidget = new QWidget(this);
    m_fromLabel = new KexiCSVInfoLabel(m_mode == File ? i18n("From CSV file:") : i18n("From Clipboard"), m_importWidget, m_mode == File);
    m_fromLabel->separator()->hide();
    if (m_mode != File) {
        m_fromLabel->setIcon(koIconName("edit-paste"));
    }
    m_toLabel = new KexiCSVInfoLabel(i18n("To table:"), m_importWidget, true);
    KexiPart::Info *partInfo = Kexi::partManager().infoForClass("org.kexi-project.table");
    m_toLabel->setIcon(partInfo->itemIconName());

    m_importProgressLabel = new QLabel(m_importWidget);
    m_importingProgressBar = new QProgressBar(m_importWidget);
    QVBoxLayout *l = new QVBoxLayout(m_importWidget);
    l->addWidget(m_fromLabel);
    l->addWidget(m_toLabel);
    l->addWidget(m_importProgressLabel);
    l->addWidget(m_importingProgressBar);
    l->addStretch(1);
    m_importingProgressBar->hide();
    m_importProgressLabel->hide();
    m_importPage = new KPageWidgetItem(m_importWidget, i18n("Importing..."));
    addPage(m_importPage);
}

void KexiCSVImportDialog::slotCommandLinkClicked()
{
    if (m_tableNameWidget) {
        m_newTable = (sender() == m_newTableButton ? true : false);
        m_tableNameWidget->setCurrentIndex(sender() == m_newTableButton ? 0 : 1);
        next();
    }
}

void KexiCSVImportDialog::initLater()
{
    if (!openData())
        return;

// delimiterChanged(detectedDelimiter); // this will cause fillTable()
    m_columnsAdjusted = false;
    fillTable();
    delete m_loadingProgressDlg;
    m_loadingProgressDlg = 0;
    if (m_dialogCancelled) {
//  m_loadingProgressDlg->hide();
        // m_loadingProgressDlg->close();
        QTimer::singleShot(0, this, SLOT(reject()));
        return;
    }

    currentCellChanged(m_table->index(0,0), QModelIndex());

// updateGeometry();
    adjustSize();
    KDialog::centerOnScreen(this);

    if (m_loadingProgressDlg)
        m_loadingProgressDlg->hide();
    show();
    m_tableView->setFocus();
}

bool KexiCSVImportDialog::openData()
{
    if (m_mode != File) //data already loaded, no encoding stuff needed
        return true;

    delete m_inputStream;
    m_inputStream = 0;
    if (m_file) {
        m_file->close();
        delete m_file;
    }
    m_file = new QFile(m_fname);
    if (!m_file->open(QIODevice::ReadOnly)) {
        m_file->close();
        delete m_file;
        m_file = 0;
        KMessageBox::sorry(this, i18n("Cannot open input file <nobr>\"%1\"</nobr>.",
                                      QDir::toNativeSeparators(m_fname)));
        enableButtonOk(false);
        m_cancelled = true;
        if (parentWidget())
            parentWidget()->raise();
        return false;
    }
    return true;
}

bool KexiCSVImportDialog::cancelled() const
{
    return m_cancelled;
}

void KexiCSVImportDialog::fillTable()
{
    KexiUtils::WaitCursor wc(true);
    repaint();
    m_blockUserEvents = true;
    enableButtonCancel(true);
    KexiUtils::WaitCursor wait;

    if (m_table->rowCount() > 0) //to accept editor
        m_tableView->setCurrentIndex(QModelIndex());

    int row, column, maxColumn;
    QString field;

    m_table->clear();
    d->clearDetectedTypes();
    d->clearUniquenessTests();

    if (true != loadRows(field, row, column, maxColumn, true))
        return;

    // file with only one line without EOL
    if (field.length() > 0) {
        setText(row - m_startline, column, field, true);
        ++row;
        field.clear();
    }

    adjustRows(row - m_startline - (m_1stRowForFieldNames->isChecked() ? 1 : 0));

    maxColumn = qMax(maxColumn, column);
    m_table->setColumnCount(maxColumn);

    for (column = 0; column < m_table->columnCount(); ++column) {
//  QString header = m_table->horizontalHeader()->label(column);
//  if (header != i18n("Text") && header != i18n("Number") &&
//   header != i18n("Date") && header != i18n("Currency"))
//  const int detectedType = m_detectedTypes[column+1];
//  m_table->horizontalHeader()->setLabel(column, m_typeNames[ detectedType ]); //i18n("Text"));
        updateColumnText(column);
        if (!m_columnsAdjusted)
            m_tableView->resizeColumnToContents(column);
    }
    m_columnsAdjusted = true;

    if (m_primaryKeyColumn >= 0 && m_primaryKeyColumn < m_table->columnCount()) {
        if (KexiDB::Field::Integer != d->detectedType(m_primaryKeyColumn)) {
            m_primaryKeyColumn = -1;
        }
    }

    m_tableView->setCurrentIndex(m_table->index(0, 0));
    currentCellChanged(m_table->index(0, 0), QModelIndex());
    if (m_primaryKeyColumn != -1)
        m_table->setData(m_table->index(0, m_primaryKeyColumn), m_pkIcon, Qt::DecorationRole);

    const int count = qMax(0, m_table->rowCount() - 1 + m_startline);
    m_allRowsLoadedInPreview = count < m_maximumRowsForPreview && !m_stoppedAt_MAX_BYTES_TO_PREVIEW;
    if (count > 1) {
        if (m_allRowsLoadedInPreview) {
            m_startAtLineSpinBox->setMaximum(count);
            m_startAtLineSpinBox->setValue(m_startline + 1);
        }
        m_startAtLineSpinBox->setEnabled(true);
        m_startAtLineLabel->setText(
            m_allRowsLoadedInPreview ?
            i18n("Start at line (1-%1):", count)
            : i18n("Start at line:") //we do not know what's real count
        );
        m_startAtLineLabel->setEnabled(true);
    }
    else { // no data
        m_startAtLineSpinBox->setMaximum(1);
        m_startAtLineSpinBox->setValue(1);
        m_startAtLineSpinBox->setEnabled(false);
        m_startAtLineLabel->setText(i18n("Start at line:"));
        m_startAtLineLabel->setEnabled(false);
    }
    updateRowCountInfo();

    m_blockUserEvents = false;
    repaint();
}

QString KexiCSVImportDialog::detectDelimiterByLookingAtFirstBytesOfFile(
    QTextStream& inputStream)
{
    // try to detect delimiter
    // \t has priority, then ; then ,
    const qint64 origOffset = inputStream.pos();
    QChar c, prevChar = 0;
    int detectedDelimiter = 0;
    bool insideQuote = false;

    //characters by priority
    const int CH_TAB_AFTER_QUOTE = 500;
    const int CH_SEMICOLON_AFTER_QUOTE = 499;
    const int CH_COMMA_AFTER_QUOTE = 498;
    const int CH_TAB = 200; // \t
    const int CH_SEMICOLON = 199; // ;
    const int CH_COMMA = 198; // ,

    QList<int> tabsPerLine, semicolonsPerLine, commasPerLine;
    int tabs = 0, semicolons = 0, commas = 0;
    int line = 0;
    bool wasChar13 = false; // true if previous x was '\r'
    for (uint i = 0; !inputStream.atEnd() && i < MAX_CHARS_TO_SCAN_WHILE_DETECTING_DELIMITER; i++) {
        (*m_inputStream) >> c; // read one char
        if (prevChar == '"') {
            if (c != '"') //real quote (not double "")
                insideQuote = !insideQuote;
        }
        if (insideQuote) {
            prevChar = c;
            continue;
        }
        if (c == ' ')
            continue;
        if (wasChar13 && c == '\n') {
            wasChar13 = false;
            continue; // previous x was '\r', eat '\n'
        }
        wasChar13 = c == '\r';
        if (c == '\n' || c == '\r') {//end of line
            //remember # of tabs/semicolons/commas in this line
            tabsPerLine += tabs;
            tabs = 0;
            semicolonsPerLine += semicolons;
            semicolons = 0;
            commasPerLine += commas;
            commas = 0;
            line++;
        } else if (c == '\t') {
            tabs++;
            detectedDelimiter = qMax(prevChar == '"' ? CH_TAB_AFTER_QUOTE : CH_TAB, detectedDelimiter);
        } else if (c == ';') {
            semicolons++;
            detectedDelimiter = qMax(prevChar == '"' ? CH_SEMICOLON_AFTER_QUOTE : CH_SEMICOLON, detectedDelimiter);
        } else if (c == ',') {
            commas++;
            detectedDelimiter = qMax(prevChar == '"' ? CH_COMMA_AFTER_QUOTE : CH_COMMA, detectedDelimiter);
        }
        prevChar = c;
    }

    inputStream.seek(origOffset); //restore orig. offset

    //now, try to find a delimiter character that exists the same number of times in all the checked lines
    //this detection method has priority over others
    QList<int>::ConstIterator it;
    if (tabsPerLine.count() > 1) {
        tabs = tabsPerLine.isEmpty() ? 0 : tabsPerLine.first();
        for (it = tabsPerLine.constBegin(); it != tabsPerLine.constEnd(); ++it) {
            if (tabs != *it)
                break;
        }
        if (tabs > 0 && it == tabsPerLine.constEnd())
            return "\t";
    }
    if (semicolonsPerLine.count() > 1) {
        semicolons = semicolonsPerLine.isEmpty() ? 0 : semicolonsPerLine.first();
        for (it = semicolonsPerLine.constBegin(); it != semicolonsPerLine.constEnd(); ++it) {
            if (semicolons != *it)
                break;
        }
        if (semicolons > 0 && it == semicolonsPerLine.constEnd())
            return ";";
    }
    if (commasPerLine.count() > 1) {
        commas = commasPerLine.first();
        for (it = commasPerLine.constBegin(); it != commasPerLine.constEnd(); ++it) {
            if (commas != *it)
                break;
        }
        if (commas > 0 && it == commasPerLine.constEnd())
            return ",";
    }
    //now return the winning character by looking at CH_* symbol
    if (detectedDelimiter == CH_TAB_AFTER_QUOTE || detectedDelimiter == CH_TAB)
        return "\t";
    if (detectedDelimiter == CH_SEMICOLON_AFTER_QUOTE || detectedDelimiter == CH_SEMICOLON)
        return ";";
    if (detectedDelimiter == CH_COMMA_AFTER_QUOTE || detectedDelimiter == CH_COMMA)
        return ",";

    return KEXICSV_DEFAULT_FILE_DELIMITER; //<-- default
}

tristate KexiCSVImportDialog::loadRows(QString &field, int &row, int &column, int &maxColumn,
                                       bool inGUI)
{
    enum { S_START, S_QUOTED_FIELD, S_MAYBE_END_OF_QUOTED_FIELD, S_END_OF_QUOTED_FIELD,
           S_MAYBE_NORMAL_FIELD, S_NORMAL_FIELD
         } state = S_START;
    field.clear();
    const bool ignoreDups = m_ignoreDuplicates->isChecked();
    bool lastCharDelimiter = false;
    bool nextRow = false;
    row = column = 1;
    m_prevColumnForSetText = 0;
    maxColumn = 0;
    QChar x;
    const bool hadInputStream = m_inputStream != 0;
    delete m_inputStream;
    if (m_mode == Clipboard) {
        m_inputStream = new QTextStream(&m_clipboardData, QIODevice::ReadOnly);
        if (!hadInputStream)
            m_delimiterWidget->setDelimiter(KEXICSV_DEFAULT_CLIPBOARD_DELIMITER);
    } else {
        m_file->seek(0); //always seek at 0 because loadRows() is called many times
        m_inputStream = new QTextStream(m_file);
        QTextCodec *codec = KGlobal::charsets()->codecForName(m_options.encoding);
        if (codec) {
            m_inputStream->setCodec(codec); //QTextCodec::codecForName("CP1250"));
        }
        if (m_detectDelimiter) {
            const QString delimiter(detectDelimiterByLookingAtFirstBytesOfFile(*m_inputStream));
            if (m_delimiterWidget->delimiter() != delimiter)
                m_delimiterWidget->setDelimiter(delimiter);
        }
    }
    const QChar delimiter(m_delimiterWidget->delimiter()[0]);
    m_stoppedAt_MAX_BYTES_TO_PREVIEW = false;
    if (m_importingProgressBar) {
        m_elapsedTimer.start();
        m_elapsedMs = m_elapsedTimer.elapsed();
    }
    int offset = 0;
    bool wasChar13 = false; // true if previous x was '\r'
    for (;; ++offset) {
//disabled: this breaks wide spreadsheets
// if (column >= m_maximumRowsForPreview)
//  return true;
        if (m_importingProgressBar && (offset % 0x100) == 0
            && (m_elapsedMs + PROGRESS_STEP_MS) < m_elapsedTimer.elapsed())
        {
            //update progr. bar dlg on final exporting
            m_elapsedMs = m_elapsedTimer.elapsed();
            m_importingProgressBar->setValue(offset);
            qApp->processEvents();
            if (m_importCancelled) {
                return ::cancelled;
            }
        }
        if (m_inputStream->atEnd()) {
            if (x != '\n' && x != '\r') {
                x = '\n'; // simulate missing \n at end
                wasChar13 = false;
            }
            else {
                break; // finish!
            }
        }
        else {
            (*m_inputStream) >> x; // read one char
        }

        if (wasChar13 && x == '\n') {
            wasChar13 = false;
            continue; // previous x was '\r', eat '\n'
        }
        wasChar13 = x == '\r';
        if (offset == 0 && x.unicode() == 0xfeff) {
            // Ignore BOM, the "Byte Order Mark"
            // (http://en.wikipedia.org/wiki/Byte_Order_Mark, // http://www.unicode.org/charts/PDF/UFFF0.pdf)
            // Probably fixed in Qt4.
            continue;
        }

        switch (state) {
        case S_START :
            if (x == m_textquote) {
                state = S_QUOTED_FIELD;
            } else if (x == delimiter) {
                field.clear();
                if ((ignoreDups == false) || (lastCharDelimiter == false))
                    ++column;
                lastCharDelimiter = true;
            } else if (x == '\n' || x == '\r') {
                if (!inGUI) {
                    //fill remaining empty fields (database wants them explicitly)
                    for (int additionalColumn = column; additionalColumn <= maxColumn; additionalColumn++) {
                        setText(row - m_startline, additionalColumn, QString(), inGUI);
                    }
                }
                nextRow = true;
                if (ignoreDups && lastCharDelimiter) {
                    // we're ignoring repeated delimiters so remove any extra trailing delimiters
                    --column;
                }
                maxColumn = qMax(maxColumn, column);
                column = 1;
                m_prevColumnForSetText = 0;
            } else {
                field += x;
                state = S_MAYBE_NORMAL_FIELD;
            }
            break;
        case S_QUOTED_FIELD :
            if (x == m_textquote) {
                state = S_MAYBE_END_OF_QUOTED_FIELD;
            }
            /*allow \n inside quoted fields
                  else if (x == '\n')
                  {
                    setText(row - m_startline, column, field, inGUI);
                    field = "";
                    if (x == '\n')
                    {
                      nextRow = true;
                      maxColumn = qMax( maxColumn, column );
                      column = 1;
                    }
                    else
                    {
                      if ((ignoreDups == false) || (lastCharDelimiter == false))
                        ++column;
                      lastCharDelimiter = true;
                    }
                    state = S_START;
                  }*/
            else {
                field += x;
            }
            break;
        case S_MAYBE_END_OF_QUOTED_FIELD :
            if (x == m_textquote) {
                field += x; //no, this was just escaped quote character
                state = S_QUOTED_FIELD;
            } else if (x == delimiter || x == '\n' || x == '\r') {
                setText(row - m_startline, column, field, inGUI);
                field.clear();
                if (x == '\n' || x == '\r') {
                    nextRow = true;
                    maxColumn = qMax(maxColumn, column);
                    column = 1;
                    m_prevColumnForSetText = 0;
                } else {
                    if ((ignoreDups == false) || (lastCharDelimiter == false))
                        ++column;
                    lastCharDelimiter = true;
                }
                state = S_START;
            } else {
                state = S_END_OF_QUOTED_FIELD;
            }
            break;
        case S_END_OF_QUOTED_FIELD :
            if (x == delimiter || x == '\n' || x == '\r') {
                setText(row - m_startline, column, field, inGUI);
                field.clear();
                if (x == '\n' || x == '\r') {
                    nextRow = true;
                    maxColumn = qMax(maxColumn, column);
                    column = 1;
                    m_prevColumnForSetText = 0;
                } else {
                    if ((ignoreDups == false) || (lastCharDelimiter == false))
                        ++column;
                    lastCharDelimiter = true;
                }
                state = S_START;
            } else {
                state = S_END_OF_QUOTED_FIELD;
            }
            break;
        case S_MAYBE_NORMAL_FIELD :
            if (x == m_textquote) {
                field.clear();
                state = S_QUOTED_FIELD;
                break;
            }
        case S_NORMAL_FIELD :
            if (x == delimiter || x == '\n' || x == '\r') {
                setText(row - m_startline, column, field, inGUI);
                field.clear();
                if (x == '\n' || x == '\r') {
                    nextRow = true;
                    maxColumn = qMax(maxColumn, column);
                    column = 1;
                    m_prevColumnForSetText = 0;
                } else {
                    if ((ignoreDups == false) || (lastCharDelimiter == false))
                        ++column;
                    lastCharDelimiter = true;
                }
                state = S_START;
            } else {
                field += x;
            }
        }
        if (x != delimiter)
            lastCharDelimiter = false;

        if (nextRow) {
            if (!inGUI && !shouldSaveRow(row - m_startline, m_1stRowForFieldNames->isChecked())) {
                // do not save to the database 1st row if it contains column names
                m_tmpValues.clear();
            } else if (!saveRow(inGUI))
                return false;
            ++row;
        }

        if (m_firstFillTableCall && row == 2
                && !m_1stRowForFieldNames->isChecked() && m_table->firstRowForFieldNames()) {
            m_table->clear();
            m_firstFillTableCall = false; //this trick is allowed only once, on startup
            m_1stRowForFieldNames->setChecked(true); //this will reload table
            m_blockUserEvents = false;
            repaint();
            return false;
        }

        if (!m_importingProgressBar && row % 20 == 0) {
            qApp->processEvents();
            //only for GUI mode:
            if (!m_firstFillTableCall && m_loadingProgressDlg && m_loadingProgressDlg->wasCancelled()) {
                delete m_loadingProgressDlg;
                m_loadingProgressDlg = 0;
                m_dialogCancelled = true;
                reject();
                return false;
            }
        }

        if (!m_firstFillTableCall && m_loadingProgressDlg) {
            m_loadingProgressDlg->progressBar()->setValue(qMin(m_maximumRowsForPreview, row));
        }

        if (inGUI && row > (m_maximumRowsForPreview + (m_table->firstRowForFieldNames() ? 1 : 0))) {
            kDebug() << "loading stopped at row #" << m_maximumRowsForPreview;
            break;
        }
        if (nextRow) {
            nextRow = false;
            //additional speedup: stop processing now if too many bytes were loaded for preview
            //kDebug() << offset;
            if (inGUI && offset >= m_maximumBytesForPreview && row >= 2) {
                m_stoppedAt_MAX_BYTES_TO_PREVIEW = true;
                return true;
            }
        }
    }
    return true;
}

void KexiCSVImportDialog::updateColumnText(int col)
{
    KexiDB::Field::Type detectedType = d->detectedType(col);
//2008-05-22 if (detectedType==_FP_NUMBER_TYPE)
//2008-05-22  detectedType=_NUMBER_TYPE; //we're simplifying that for now
//2008-05-22 else
    if (detectedType == KexiDB::Field::InvalidType) {
        d->setDetectedType(col, KexiDB::Field::Text); //entirely empty column
        detectedType = KexiDB::Field::Text;
    }

    m_table->setHeaderData(col, Qt::Horizontal,
        i18n("Column %1", col + 1) + "  \n(" + kexiCSVImportStatic->typeNames[detectedType].toLower() + ")  ");
    m_tableView->horizontalHeader()->adjustSize();

    //check uniqueness
    QList<int> *list = d->uniquenessTest(col);
    if (m_primaryKeyColumn == -1 && list && !list->isEmpty()) {
        qSort(*list);
        QList<int>::ConstIterator it = list->constBegin();
        int prevValue = *it;
        ++it;
        for (; it != list->constEnd() && prevValue != (*it); ++it)
            prevValue = (*it);
        if (it != list->constEnd()) {
            //duplicates:
            list->clear();
        }
        else {
            //a candidate for PK (autodetected)!
            m_primaryKeyColumn = col;
        }
    }
    if (list) //not needed now: conserve memory
        list->clear();
}

void KexiCSVImportDialog::detectTypeAndUniqueness(int row, int col, const QString& text)
{
    int intValue;
    KexiDB::Field::Type type = d->detectedType(col);
    if (row == 1 || type != KexiDB::Field::Text) {
        bool found = false;
        if (text.isEmpty() && type == KexiDB::Field::InvalidType)
            found = true; //real type should be found later
        //detect type because it's 1st row or all prev. rows were not text
        //-FP number? (trying before "number" type is a must)
        if (!found && (row == 1 || type == KexiDB::Field::Integer || type == KexiDB::Field::Double
                                || type == KexiDB::Field::InvalidType))
        {
            bool ok = text.isEmpty() || m_fpNumberRegExp1.exactMatch(text) || m_fpNumberRegExp2.exactMatch(text);
            if (ok && (row == 1 || type == KexiDB::Field::InvalidType))
            {
                d->setDetectedType(col, KexiDB::Field::Double);
                found = true; //yes
            }
        }
        //-number?
        if (!found && (row == 1 || type == KexiDB::Field::Integer || type == KexiDB::Field::InvalidType)) {
            bool ok = text.isEmpty();//empty values allowed
            if (!ok)
                intValue = text.toInt(&ok);
            if (ok && (row == 1 || type == KexiDB::Field::InvalidType)) {
                d->setDetectedType(col, KexiDB::Field::Integer);
                found = true; //yes
            }
        }
        //-date?
        if (!found && (row == 1 || type == KexiDB::Field::Date || type == KexiDB::Field::InvalidType)) {
            if ((row == 1 || type == KexiDB::Field::InvalidType)
                    && (text.isEmpty() || m_dateRegExp.exactMatch(text))) {
                d->setDetectedType(col, KexiDB::Field::Date);
                found = true; //yes
            }
        }
        //-time?
        if (!found && (row == 1 || type == KexiDB::Field::Time || type == KexiDB::Field::InvalidType)) {
            if ((row == 1 || type == KexiDB::Field::InvalidType)
                 && (text.isEmpty() || m_timeRegExp1.exactMatch(text) || m_timeRegExp2.exactMatch(text)))
            {
                d->setDetectedType(col, KexiDB::Field::Time);
                found = true; //yes
            }
        }
        //-date/time?
        if (!found && (row == 1 || type == KexiDB::Field::Time || type == KexiDB::Field::InvalidType)) {
            if (row == 1 || type == KexiDB::Field::InvalidType) {
                bool detected = text.isEmpty();
                if (!detected) {
                    const QStringList dateTimeList(text.split(" "));
                    bool ok = dateTimeList.count() >= 2;
//! @todo also support ISODateTime's "T" separator?
//! @todo also support timezones?
                    if (ok) {
                        //try all combinations
                        QString datePart(dateTimeList[0].trimmed());
                        QString timePart(dateTimeList[1].trimmed());
                        ok = m_dateRegExp.exactMatch(datePart)
                             && (m_timeRegExp1.exactMatch(timePart) || m_timeRegExp2.exactMatch(timePart));
                    }
                    detected = ok;
                }
                if (detected) {
                    d->setDetectedType(col, KexiDB::Field::DateTime);
                    found = true; //yes
                }
            }
        }
        if (!found && type == KexiDB::Field::InvalidType && !text.isEmpty()) {
            //eventually, a non-emptytext after a while
            d->setDetectedType(col, KexiDB::Field::Text);
            found = true; //yes
        }
        //default: text type (already set)
    }

    type = d->detectedType(col);
    kDebug() << type;

    if (type == KexiDB::Field::Integer) {
        // check uniqueness for this value
        QList<int> *list = d->uniquenessTest(col);
        if (text.isEmpty()) {
            if (list) {
                list->clear(); // empty value cannot be in PK
            }
        }
        else {
            if (!list) {
                list = new QList<int>();
                d->setUniquenessTest(col, list);
            }
            list->append(intValue);
        }
    }
}

QDate KexiCSVImportDialog::buildDate(int y, int m, int d) const
{
    if (y < 100) {
        if ((1900 + y) >= m_minimumYearFor100YearSlidingWindow)
            return QDate(1900 + y, m, d);
        else
            return QDate(2000 + y, m, d);
    }
    return QDate(y, m, d);
}

bool KexiCSVImportDialog::parseDate(const QString& text, QDate& date)
{
    if (!m_dateRegExp.exactMatch(text))
        return false;
    //dddd - dd - dddd
    //1    2 3  4 5    <- pos
    const int d1 = m_dateRegExp.cap(1).toInt(), d3 = m_dateRegExp.cap(3).toInt(), d5 = m_dateRegExp.cap(5).toInt();
    switch (m_options.dateFormat) {
    case KexiCSVImportOptions::DMY: date = buildDate(d5, d3, d1); break;
    case KexiCSVImportOptions::YMD: date = buildDate(d1, d3, d5); break;
    case KexiCSVImportOptions::MDY: date = buildDate(d5, d1, d3); break;
    case KexiCSVImportOptions::AutoDateFormat:
        if (m_dateRegExp.cap(2) == "/") { //probably separator for american format mm/dd/yyyy
            date = buildDate(d5, d1, d3);
        } else {
            if (d5 > 31) //d5 == year
                date = buildDate(d5, d3, d1);
            else //d1 == year
                date = buildDate(d1, d3, d5);
        }
        break;
    default:;
    }
    return date.isValid();
}

bool KexiCSVImportDialog::parseTime(const QString& text, QTime& time)
{
    time = QTime::fromString(text, Qt::ISODate); //same as m_timeRegExp1
    if (time.isValid())
        return true;
    if (m_timeRegExp2.exactMatch(text)) { //hh:mm:ss
        time = QTime(m_timeRegExp2.cap(1).toInt(),
                     m_timeRegExp2.cap(3).toInt(), m_timeRegExp2.cap(5).toInt());
        return true;
    }
    return false;
}

void KexiCSVImportDialog::setText(int row, int col, const QString& text, bool inGUI)
{
    if (!inGUI) {
        if (!shouldSaveRow(row, m_1stRowForFieldNames->isChecked()))
            return; // do not care about this value if it contains column names (these were already used)

        //save text directly to database buffer
        if (m_prevColumnForSetText == 0) { //1st call
            m_tmpValues.clear();
            if (m_implicitPrimaryKeyAdded) {
                m_tmpValues << QVariant(); //id will be autogenerated here
            }
        }
        if ((m_prevColumnForSetText + 1) < col) { //skipped one or more columns
                                                  //before this: save NULLs first
            for (int i = m_prevColumnForSetText + 1; i < col; i++) {
                m_tmpValues << QVariant();
            }
        }
        m_prevColumnForSetText = col;

        const KexiDB::Field::Type detectedType = d->detectedType(col-1);
        if (detectedType == KexiDB::Field::Integer) {
            m_tmpValues << (text.isEmpty() ? QVariant() : text.toInt());
//! @todo what about time and float/double types and different integer subtypes?
        } else if (detectedType == KexiDB::Field::Double) {
            //replace ',' with '.'
            QByteArray t(text.toLatin1());
            const int textLen = t.length();
            for (int i = 0; i < textLen; i++) {
                if (t[i] == ',') {
                    t[i] = '.';
                    break;
                }
            }
            m_tmpValues << (t.isEmpty() ? QVariant() : t.toDouble());
        } else if (detectedType == KexiDB::Field::Boolean) {
            const QString t(text.trimmed().toLower());
            if (t.isEmpty())
                m_tmpValues << QVariant();
            else if (t == "0" || t == m_stringNo || t == m_stringI18nNo || t == m_stringFalse || t == m_stringI18nFalse)
                m_tmpValues << QVariant(false);
            else
                m_tmpValues << QVariant(true); //anything nonempty
        } else if (detectedType == KexiDB::Field::Date) {
            QDate date;
            if (parseDate(text, date))
                m_tmpValues << date;
            else
                m_tmpValues << QVariant();
        } else if (detectedType == KexiDB::Field::Time) {
            QTime time;
            if (parseTime(text, time))
                m_tmpValues << time;
            else
                m_tmpValues << QVariant();
        } else if (detectedType == KexiDB::Field::DateTime) {
            QStringList dateTimeList(text.split(" "));
            if (dateTimeList.count() < 2)
                dateTimeList = text.split("T"); //also support ISODateTime's "T" separator
//! @todo also support timezones?
            if (dateTimeList.count() >= 2) {
                //try all combinations
                QString datePart(dateTimeList[0].trimmed());
                QDate date;
                if (parseDate(datePart, date)) {
                    QString timePart(dateTimeList[1].trimmed());
                    QTime time;
                    if (parseTime(timePart, time))
                        m_tmpValues << QDateTime(date, time);
                    else
                        m_tmpValues << QVariant();
                } else
                    m_tmpValues << QVariant();
            } else
                m_tmpValues << QVariant();
        } else // Text type and the rest
            m_tmpValues << (m_options.trimmedInTextValuesChecked ? text.trimmed() : text);
        return;
    }
    //save text to GUI (table view)
    if (m_table->columnCount() < col) {
        m_table->setColumnCount(col);
    }
    if (!m_1stRowForFieldNames->isChecked()) {
        if ((row + m_startline) == 1) {//this row is for column name
            if (m_table->firstRowForFieldNames() && !m_1stRowForFieldNames->isChecked()) {
                QString f(text.simplified());
                if (f.isEmpty() || !f[0].isLetter()) {
                    m_table->setFirstRowForFieldNames(false);
                }
            }
        }
        row++; //1st row was for column names
    } else {
        if ((row + m_startline) == 1) {//this is for column name
            m_table->setRowCount(1);
            QString colName(text.simplified());
            if (!colName.isEmpty()) {
                if (colName.left(1) >= "0" && colName.left(1) <= "9")
                    colName.prepend(i18n("Column") + " ");
                m_table->setData(m_table->index(0, col - 1), colName);
            }
            return;
        }
    }
    if (row < 2) // skipped by the user
        return;
    if (m_table->rowCount() < row) {
//  if (m_maximumRowsForPreview >= row+100)
        m_table->setRowCount(row + 100); /* We add more rows at a time to limit recalculations */
        //else
//   m_table->setNumRows(m_maximumRowsForPreview);
        m_adjustRows = true;
    }

    m_table->setData(m_table->index(row-1 ,col-1),m_options.trimmedInTextValuesChecked ? text.trimmed() : text);

    detectTypeAndUniqueness(row - 1, col - 1, text);
}

bool KexiCSVImportDialog::saveRow(bool inGUI)
{
    if (inGUI) {
        //nothing to do
        return true;
    }
    //save db buffer
    QStringList msgList;
    QListIterator<QVariant> i(m_tmpValues);
    m_importingStatement->clearArguments();
    while (i.hasNext()) {
        QVariant tmp = i.next();
        *m_importingStatement << tmp;
        msgList << tmp.toString();
    }
    bool res = m_importingStatement->execute();
//todo: move
    int msgRes;
    if (!res) {
        msgRes = KMessageBox::warningContinueCancelList(this, i18nc("@info", "An error occurred during insert record."),
                    QStringList(msgList.join(";")),
                    QString(),
                    KStandardGuiItem::cont(),
                    KStandardGuiItem::cancel(),
                    "SkipImportErrors"
                    );
        res = (msgRes == KMessageBox::Continue ? true : false);
    }
    m_tmpValues.clear();
    m_importingStatement->clearArguments();
    return res;
// return m_conn->insertRecord(*m_destinationTableSchema, m_dbRowBuffer);
}

void KexiCSVImportDialog::adjustRows(int iRows)
{
    if (m_adjustRows) {
        m_table->setRowCount(iRows);
        m_adjustRows = false;
        for (int i = 0; i < iRows; i++)
            m_tableView->resizeRowToContents(i);
    }
}

void KexiCSVImportDialog::formatChanged(int index)
{
    if (index < 0 || index >= kexiCSVImportStatic->types.size())
        return;
    KexiDB::Field::Type type = kexiCSVImportStatic->types[index];
    d->setDetectedType(m_tableView->currentIndex().column(), type);
    m_primaryKeyField->setEnabled(KexiDB::Field::Integer == type);
    m_primaryKeyField->setChecked(m_primaryKeyColumn == m_tableView->currentIndex().column() && m_primaryKeyField->isEnabled());
    updateColumnText(m_tableView->currentIndex().column());
}

void KexiCSVImportDialog::delimiterChanged(const QString& delimiter)
{
    Q_UNUSED(delimiter);
    m_columnsAdjusted = false;
    m_detectDelimiter = false; //selected by hand: do not detect in the future
    //delayed, otherwise combobox won't be repainted
    fillTableLater();
}

void KexiCSVImportDialog::textquoteSelected(int)
{
    const QString tq(m_comboQuote->textQuote());
    if (tq.isEmpty())
        m_textquote = 0;
    else
        m_textquote = tq[0];

    kDebug() << m_textquote;

    //delayed, otherwise combobox won't be repainted
    fillTableLater();
}

void KexiCSVImportDialog::fillTableLater()
{
    m_table->setColumnCount(0);
    QTimer::singleShot(10, this, SLOT(fillTable()));
}

void KexiCSVImportDialog::startlineSelected(int startline)
{
// const int startline = line.toInt() - 1;
    if (m_startline == (startline - 1))
        return;
    m_startline = startline - 1;
    m_adjustRows = true;
    m_columnsAdjusted = false;
    fillTable();
    m_tableView->setFocus();
}

void KexiCSVImportDialog::currentCellChanged(const QModelIndex &cur, const QModelIndex &prev)
{
    if (prev.column() == cur.column() || !cur.isValid())
        return;
    const KexiDB::Field::Type type = d->detectedType(cur.column());
//2008-05-22 if (type==_FP_NUMBER_TYPE)
//2008-05-22  type=_NUMBER_TYPE; //we're simplifying that for now

    m_formatCombo->setCurrentIndex(kexiCSVImportStatic->indicesForTypes.value(type, -1));
    m_formatLabel->setText(i18n("Format for column %1:", cur.column() + 1));
    m_primaryKeyField->setEnabled(KexiDB::Field::Integer == type);
    m_primaryKeyField->blockSignals(true); //block to disable executing slotPrimaryKeyFieldToggled()
    m_primaryKeyField->setChecked(m_primaryKeyColumn == cur.column());
    m_primaryKeyField->blockSignals(false);
}

//! Used in emergency by accept()
void KexiCSVImportDialog::dropDestinationTable(KexiProject* project, KexiPart::Item* partItemForSavedTable)
{
    m_importingProgressBar->hide();
    project->deleteUnstoredItem(partItemForSavedTable);
    partItemForSavedTable = 0;
    m_conn->dropTable(m_destinationTableSchema); /*alsoRemoveSchema*/
    m_destinationTableSchema = 0;
    m_conn = 0;
}

//! Used in emergency by accept()
void KexiCSVImportDialog::raiseErrorInAccept(KexiProject* project, KexiPart::Item* partItemForSavedTable)
{
    enableButton(KDialog::Apply, true);
    project->deleteUnstoredItem(partItemForSavedTable);
    delete m_destinationTableSchema;
    m_destinationTableSchema = 0;
    m_conn = 0;
    enableButton(KDialog::User3, true);
    m_importInProgress = false;
    m_importingProgressBar->hide();
}

void KexiCSVImportDialog::accept()
{
    parentWidget()->raise();
    bool openingCancelled;
    KexiWindow *win = KexiMainWindowIface::global()->openedWindowFor(m_partItemForSavedTable);
    if (win) {
        KexiMainWindowIface::global()->closeObject(m_partItemForSavedTable);
    }
    KexiMainWindowIface::global()->openObject(m_partItemForSavedTable,
                                              Kexi::DataViewMode, openingCancelled);
    KAssistantDialog::accept();
}

void KexiCSVImportDialog::import()
{
//! @todo MOVE MOST OF THIS TO CORE/ (KexiProject?) after KexiWindow code is moved to non-gui place

    KMessageBox::enableMessage("SkipImportErrors");
    KexiGUIMessageHandler msg; //! @todo make it better integrated with main window
    KexiProject *project = KexiMainWindowIface::global()->project();
    if (!project) {
        msg.showErrorMessage(i18n("No project available."));
        return;
    }
    m_conn = project->dbConnection();
    if (!m_conn) {
        msg.showErrorMessage(i18n("No database connection available."));
        return;
    }

    if (m_newTable) {
        m_destinationTableSchema = new KexiDB::TableSchema(m_partItemForSavedTable->name());
        m_destinationTableSchema->setCaption(m_partItemForSavedTable->caption());
        m_destinationTableSchema->setDescription(m_partItemForSavedTable->description());
        const uint numCols(m_table->columnCount());

        m_implicitPrimaryKeyAdded = false;
        //add PK if user wanted it
        int msgboxResult;
        if (   m_primaryKeyColumn == -1
           && KMessageBox::No != (msgboxResult = KMessageBox::questionYesNoCancel(this,
                  i18n("No Primary Key (autonumber) has been defined.\n"
                       "Should it be automatically defined on import (recommended)?\n\n"
                       "Note: An imported table without a Primary Key may not be editable (depending on database type)."),
                   QString(),
                   KGuiItem(i18nc("Add Database Primary Key to a Table", "Add Primary Key"), koIconName("key")),
                   KGuiItem(i18nc("Do Not Add Database Primary Key to a Table", "Do Not Add")))))
        {
            if (msgboxResult == KMessageBox::Cancel) {
                raiseErrorInAccept(project, m_partItemForSavedTable);
                return; //cancel accepting
            }

            //add implicit PK field
//! @todo make this field hidden (what about e.g. pgsql?)
            m_implicitPrimaryKeyAdded = true;

            QString fieldName("id");
            QString fieldCaption("Id");

            QSet<QString> colnames;
            for (uint col = 0; col < numCols; col++)
                colnames.insert(m_table->data(m_table->index(0, col)).toString().toLower().simplified());
            if (colnames.contains(fieldName)) {
                int num = 1;
                while (colnames.contains(fieldName + QString::number(num)))
                    num++;
                fieldName += QString::number(num);
                fieldCaption += QString::number(num);
            }
            KexiDB::Field *field = new KexiDB::Field(
                fieldName,
                KexiDB::Field::Integer,
                KexiDB::Field::NoConstraints,
                KexiDB::Field::NoOptions,
                0, 0, //uint length=0, uint precision=0,
                QVariant(), //QVariant defaultValue=QVariant(),
                fieldCaption
            ); //no description and width for now
            field->setPrimaryKey(true);
            field->setAutoIncrement(true);
            m_destinationTableSchema->addField(field);
        }

        for (uint col = 0; col < numCols; col++) {
            QString fieldCaption(m_table->data(m_table->index(0, col)).toString().simplified());
            QString fieldName(KexiUtils::string2Identifier(fieldCaption));
            if (m_destinationTableSchema->field(fieldName)) {
                QString fixedFieldName;
                uint i = 2; //"apple 2, apple 3, etc. if there're many "apple" names
                do {
                    fixedFieldName = fieldName + "_" + QString::number(i);
                    if (!m_destinationTableSchema->field(fixedFieldName))
                        break;
                    i++;
                } while (true);
                fieldName = fixedFieldName;
                fieldCaption += (" " + QString::number(i));
            }
            KexiDB::Field::Type detectedType = d->detectedType(col);
//! @todo what about time and float/double types and different integer subtypes?
//! @todo what about long text?
            if (detectedType == KexiDB::Field::InvalidType) {
                detectedType = KexiDB::Field::Text;
            }
            KexiDB::Field *field = new KexiDB::Field(
                fieldName,
                detectedType,
                KexiDB::Field::NoConstraints,
                KexiDB::Field::NoOptions,
                0, 0, //uint length=0, uint precision=0,
                QVariant(), //QVariant defaultValue=QVariant(),
                fieldCaption
            ); //no description and width for now

            if ((int)col == m_primaryKeyColumn) {
                field->setPrimaryKey(true);
                field->setAutoIncrement(true);
            }
            m_destinationTableSchema->addField(field);
        }
    } else {
        m_implicitPrimaryKeyAdded = false;
        m_destinationTableSchema = m_conn->tableSchema(m_partItemForSavedTable->name());
        int firstColumn = 0;
        if (m_destinationTableSchema->field(0)->isPrimaryKey() && m_primaryKeyColumn == -1) {
            m_implicitPrimaryKeyAdded = true;
            firstColumn = 1;
        }
        if (m_destinationTableSchema->fields()->size() - firstColumn < m_table->columnCount()) {
            KMessageBox::error(this, i18n("<p>Fields count does not match!</p>"
                        "<p>Please choose another table.</p>"));
            return;
        }
    }

    m_importInProgress = true;
    enableButton(KDialog::User3, false);
    enableButton(KDialog::Apply, false);
    KexiPart::Part *part = Kexi::partManager().partForClass("org.kexi-project.table");
    if (!part) {
        msg.showErrorMessage(&Kexi::partManager());
        return;
    }

    KexiDB::Transaction transaction = m_conn->beginTransaction();
    if (transaction.isNull()) {
        msg.showErrorMessage(m_conn);
        raiseErrorInAccept(project, m_partItemForSavedTable);
        return;
    }
    KexiDB::TransactionGuard tg(transaction);

    //-create physical table
    if (m_newTable && !m_conn->createTable(m_destinationTableSchema, false /*allowOverwrite*/)) {
        msg.showErrorMessage(m_conn);
        raiseErrorInAccept(project, m_partItemForSavedTable);
        return;
    }

    m_importingStatement = m_conn->prepareStatement(
                               KexiDB::PreparedStatement::InsertStatement, *m_destinationTableSchema);
    if (!m_importingStatement) {
        msg.showErrorMessage(m_conn);
        raiseErrorInAccept(project, m_partItemForSavedTable);
        return;
    }

    if (m_file) {
        m_importProgressLabel->setText(
            i18n("Importing CSV Data from <nobr>\"%1\"</nobr> into \"%2\" table...",
                 QDir::toNativeSeparators(m_fname), m_destinationTableSchema->name()));
        m_importingProgressBar->setMaximum(QFileInfo(*m_file).size() - 1);
        m_importingProgressBar->show();
        m_importProgressLabel->show();
    }

    int row, column, maxColumn;
    QString field;

    // main job
    tristate res = loadRows(field, row, column, maxColumn, false /*!gui*/);

    if (true != res) {
        //importing cancelled or failed
        if (!res) { //do not display err msg when res == cancelled
            m_importProgressLabel->setText(i18n("Import has been canceled."));
        } else if (~res) {
            m_importProgressLabel->setText(i18n("Error occurred during import."));
        }
        raiseErrorInAccept(project, m_partItemForSavedTable);
        return;
    }

    // file with only one line without '\n'
    if (field.length() > 0) {
        setText(row - m_startline, column, field, false /*!gui*/);
        //fill remaining empty fields (database wants them explicitly)
        for (int additionalColumn = column; additionalColumn <= maxColumn; additionalColumn++) {
            setText(row - m_startline, additionalColumn, QString(), false /*!gui*/);
        }
        if (!saveRow(false /*!gui*/)) {
            msg.showErrorMessage(m_conn);
            raiseErrorInAccept(project, m_partItemForSavedTable);
            return;
        }
        ++row;
        field.clear();
    }

    if (!tg.commit()) {
        msg.showErrorMessage(m_conn);
        raiseErrorInAccept(project, m_partItemForSavedTable);
        return;
    }

    //-now we can store the item
    if (m_newTable) {
        m_partItemForSavedTable->setIdentifier(m_destinationTableSchema->id());
        project->addStoredItem(part->info(), m_partItemForSavedTable);
    }
    m_importingProgressBar->hide();
    m_importProgressLabel->setText(i18n("Data has been successfully imported to table \"%1\".",
                            m_destinationTableSchema->name()));
    m_importInProgress = false;
    kDebug()<<"IMPORT DONE";
    showButton(KDialog::User1, true);
    showButton(KDialog::Apply, false);
    setButtonGuiItem(KDialog::Cancel, KStandardGuiItem::close());
    showButton(KDialog::User2, false);
    showButton(KDialog::User3, false);
    m_conn = 0;
}

void KexiCSVImportDialog::reject()
{
    kDebug()<<"IMP_P"<<m_importInProgress;
    if (!m_importInProgress) {
        KAssistantDialog::reject();
        return;
    }
    m_importCancelled = true;
}

int KexiCSVImportDialog::getHeader(int col)
{
    QString header = m_table->horizontalHeaderItem(col)->text();

    if (header == i18nc("Text type for column", "Text"))
        return TEXT;
    else if (header == i18nc("Numeric type for column", "Number"))
        return NUMBER;
    else if (header == i18nc("Currency type for column", "Currency"))
        return CURRENCY;
    else
        return DATE;
}

QString KexiCSVImportDialog::getText(int row, int col)
{
    return m_table->item(row, col)->text();
}

void KexiCSVImportDialog::ignoreDuplicatesChanged(int)
{
    fillTable();
}

void KexiCSVImportDialog::slot1stRowForFieldNamesChanged(int state)
{
    m_adjustRows = true;
    m_table->setFirstRowForFieldNames(state); 
    if (m_1stRowForFieldNames->isChecked() && m_startline > 0 && m_startline >= (m_startAtLineSpinBox->maximum() - 1)) {
        m_startline--;
    }
    m_columnsAdjusted = false;
    fillTable();
}

void KexiCSVImportDialog::optionsButtonClicked()
{
    KexiCSVImportOptionsDialog dlg(m_options, this);
    if (QDialog::Accepted != dlg.exec())
        return;

    KexiCSVImportOptions newOptions(dlg.options());
    if (m_options != newOptions) {
        m_options = newOptions;
        if (!openData())
            return;
        fillTable();
    }
}

bool KexiCSVImportDialog::eventFilter(QObject * watched, QEvent * e)
{
    QEvent::Type t = e->type();
    // temporary disable keyboard and mouse events for time-consuming tasks
    if (m_blockUserEvents && (t == QEvent::KeyPress || t == QEvent::KeyRelease
                              || t == QEvent::MouseButtonPress || t == QEvent::MouseButtonDblClick
                              || t == QEvent::Paint))
        return true;

    if (watched == m_startAtLineSpinBox && t == QEvent::KeyPress) {
        QKeyEvent *ke = static_cast<QKeyEvent*>(e);
        if (ke->key() == Qt::Key_Enter || ke->key() == Qt::Key_Return) {
            m_tableView->setFocus();
            return true;
        }
    }
    return QDialog::eventFilter(watched, e);
}

void KexiCSVImportDialog::slotPrimaryKeyFieldToggled(bool on)
{
    Q_UNUSED(on);
    if (m_primaryKeyColumn >= 0 && m_primaryKeyColumn < m_table->columnCount()) {
        m_table->setData(m_table->index(0, m_primaryKeyColumn), QPixmap(), Qt::DecorationRole);
    }
    if (m_primaryKeyField->isChecked()) {
        m_primaryKeyColumn = m_tableView->currentIndex().column();
        m_table->setData(m_table->index(0, m_primaryKeyColumn), m_pkIcon, Qt::DecorationRole);
    }
    else {
        m_primaryKeyColumn = -1;
    }
}

void KexiCSVImportDialog::updateRowCountInfo()
{
    m_infoLbl->setFileName(m_fname);
    if (m_allRowsLoadedInPreview) {
        m_infoLbl->setCommentText(
            i18nc("row count", "(rows: %1)", m_table->rowCount() - 1 + m_startline));
        m_infoLbl->commentLabel()->setToolTip(QString());
    } else {
        m_infoLbl->setCommentText(
            i18nc("row count", "(rows: more than %1)",  m_table->rowCount() - 1 + m_startline));
        m_infoLbl->commentLabel()->setToolTip(i18n("Not all rows are visible on this preview"));
    }
}

#include "kexicsvimportdialog.moc"
