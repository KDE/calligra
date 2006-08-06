#include <qcheckbox.h>
#include <qlabel.h>
#include <qspinbox.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qregexp.h>
#include <qwhatsthis.h>
#include <qtooltip.h>
#include <qvalidator.h>
#include <qpushbutton.h>

#include <kinputdialog.h>
#include <klocale.h>
#include <kdebug.h>
#include <kmessagebox.h>
#include <kiconloader.h>

#include "kdchart/KDChartAxisParams.h"
#include "kchart_params.h"
#include "kchart_factory.h"

#include "kchartDataEditor.h"
#include "kchartDataEditor.moc"

namespace KChart
{


// ================================================================
//                    Class kchartDataSpinBox


// We don't provide very much generality, since this spinbox is used
// here and here only.
//
kchartDataSpinBox::kchartDataSpinBox(QWidget *parent)
    : QSpinBox(parent)
{
    m_ignore = false;
}


kchartDataSpinBox::~kchartDataSpinBox()
{
}


void kchartDataSpinBox::stepUp()
{
    m_ignore = true;
    uint const new_value = value() + 1;

    QSpinBox::stepUp();
    setValue(new_value);

    emit valueChangedSpecial( value() );
    m_ignore = false;
}


void kchartDataSpinBox::stepDown()
{
    m_ignore = true;

    uint const new_value = value() - 1;
    QSpinBox::stepDown();
    setValue(new_value);

    emit valueChangedSpecial( value() );
    m_ignore = false;
}


bool kchartDataSpinBox::eventFilter( QObject *obj, QEvent *ev )
{
    if ( obj == editor() ) {
        if ( ev->type() == QEvent::FocusOut ) {
            //kdDebug() << "Focus out" << endl;
            setValue(editor()->text().toInt());

            // Don't emit valueChangedSpecial(int) twice when
            // stepUp/stepDown has been called
            if (!m_ignore)
                emit valueChangedSpecial( value() );
        }
    }
 
    // Pass the event on to the parent class.
    return QSpinBox::eventFilter( obj, ev );
}


// ================================================================
//                    Class kchartDataTable


// Used for the keyboard navigation
//
kchartDataTable::kchartDataTable(QWidget *parent)
    : QTable(parent)
{
}


kchartDataTable::~kchartDataTable()
{
}


bool kchartDataTable::eventFilter( QObject *obj, QEvent *ev )
{
    if (ev->type() == QEvent::KeyPress && strcmp(obj->name(), 
						 "qt_tableeditor")==0 ) {
        QKeyEvent *e = (QKeyEvent *)ev;

        switch ( e->key() ) {
            case Qt::Key_Up:
            {
                if ( currentRow() > 0 ) {
                    setCurrentCell( currentRow()-1, currentColumn() );
                    editCell(currentRow(), currentColumn() );
                    return true;
                }
                break;
            }
            case Qt::Key_Down: 
	    {
                if ( currentRow() < numRows()-1 ) {
                    setCurrentCell( currentRow()+1, currentColumn() );
                    editCell(currentRow(), currentColumn() );
                    return true;
                }
                break;
            }
            case Qt::Key_Right: 
	    {
                if ( currentColumn() < numCols()-1 ) {
                    setCurrentCell( currentRow(), currentColumn()+1 );
                    editCell(currentRow(), currentColumn() );
                    return true;
                }
                break;
            }
            case Qt::Key_Left:
            {
                if ( currentColumn() > 0 ) {
                    setCurrentCell( currentRow(), currentColumn()-1 );
                    editCell(currentRow(), currentColumn() );
                    return true;
                }
                break;
            }
        }
    }

    return QTable::eventFilter( obj, ev );
}


// ================================================================
//                    Class kchartDataEditor


#define COLUMNWIDTH  80

kchartDataEditor::kchartDataEditor(QWidget* parent) :
    KDialogBase(parent, "dataeditor", true, i18n("KChart Data Editor"), 
		KDialogBase::Ok | KDialogBase::Cancel | KDialogBase::Apply,
		KDialogBase::Ok, true)
{
    QWidget      *page = new QWidget( this );
    setMainWidget(page);

    // Create the main table.
    m_table = new kchartDataTable(page);
    m_table->setSelectionMode(QTable::NoSelection);
    m_table->setFocus();
    m_table->setRowMovingEnabled(true);
    m_table->setColumnMovingEnabled(true);

    connect( m_table, SIGNAL( currentChanged(int, int) ),
	     this,    SLOT( currentChanged(int, int) ) );
  
    // Create the Rows setting
    m_rowsLA = new QLabel( i18n("# Rows:" ), page );
    m_rowsLA->resize( m_rowsLA->sizeHint() );
    m_rowsSB = new kchartDataSpinBox( page );
    m_rowsSB->resize( m_rowsSB->sizeHint() );
    m_rowsSB->setMinValue(1);

    // Create the columns setting
    m_colsLA = new QLabel( i18n("# Columns:" ), page );
    m_colsLA->resize( m_colsLA->sizeHint() );
    m_colsSB = new kchartDataSpinBox( page );
    m_colsSB->resize( m_colsSB->sizeHint() );
    m_colsSB->setMinValue(1);
    
#if 0
    // The row/column as label checkboxes. 
    m_firstRowAsLabel = new QCheckBox( i18n( "First row as label" ), page);
    m_firstColAsLabel = new QCheckBox( i18n( "First column as label" ), page);
#endif

    // Buttons for Inserting / Removing rows & columns 
    // The icon images are taken from the standard 
    m_insertRowButton = new QPushButton( page);
    // In 2.0; this is supposed to be just KIcon("name").pixmap(32).
    m_insertRowButton->setPixmap( BarIcon( QString("insert_table_row"),
					   KIcon::SizeMedium,
					   KIcon::DefaultState,
					   KChartFactory::global() ) );
    //m_insertRowButton = new QPushButton( i18n("Insert Row") , page);
    connect( m_insertRowButton, SIGNAL( clicked() ),
	     this,              SLOT( insertRow() ) );
    
    m_removeRowButton = new QPushButton( page );
    m_removeRowButton->setPixmap( BarIcon( QString("delete_table_row"),
					   KIcon::SizeMedium,
					   KIcon::DefaultState,
					   KChartFactory::global() ) );
    //m_removeRowButton = new QPushButton( i18n("Remove Row") , page);
    connect( m_removeRowButton, SIGNAL( clicked() ),
	     this,              SLOT( removeCurrentRow() ) );
    
    m_insertColButton = new QPushButton( page );
    m_insertColButton->setPixmap( BarIcon( QString("insert_table_col"),
					   KIcon::SizeMedium,
					   KIcon::DefaultState,
					   KChartFactory::global() ) );
    //m_insertColButton = new QPushButton( i18n("Insert Column") , page);
    connect( m_insertColButton, SIGNAL( clicked() ),
	     this,              SLOT( insertColumn() ) );
    
    m_removeColButton = new QPushButton( page );
    m_removeColButton->setPixmap( BarIcon( QString("delete_table_col"),
					   KIcon::SizeMedium,
					   KIcon::DefaultState,
					   KChartFactory::global() ) );
    //m_removeColButton = new QPushButton( i18n("Remove Column") , page);
    connect( m_removeColButton, SIGNAL( clicked() ),
	     this,              SLOT( removeCurrentColumn() ) );

    // Start the layout.  The buttons are at the top.
    QVBoxLayout  *topLayout = new QVBoxLayout( page );
    
    QHBoxLayout* insertRemoveLayout = new QHBoxLayout( );
   
    insertRemoveLayout->setSpacing(5);
    insertRemoveLayout->addWidget(m_insertRowButton);
    insertRemoveLayout->addWidget(m_removeRowButton);
    insertRemoveLayout->addWidget(m_insertColButton);
    insertRemoveLayout->addWidget(m_removeColButton);
    insertRemoveLayout->addStretch(1);
    
    topLayout->addLayout(insertRemoveLayout);
    topLayout->addSpacing(10);
    
    // The table is below the buttons.
    topLayout->addWidget(m_table);

    // Then, a horizontal layer with the rows and columns settings
    QHBoxLayout  *hbl1 = new QHBoxLayout(  );
    hbl1->addWidget(m_rowsLA);
    hbl1->addWidget(m_rowsSB);
    hbl1->addSpacing(20);
    hbl1->addWidget(m_colsLA);
    hbl1->addWidget(m_colsSB);
    hbl1->addStretch(1);
    hbl1->setMargin(10);
    topLayout->addLayout(hbl1);

#if 0
    // Last, the checkboxes with "First row/column as label"
    QHBoxLayout  *hbl2 = new QHBoxLayout(  );
    hbl2->addWidget(m_firstRowAsLabel);
    hbl2->addWidget(m_firstColAsLabel);
    hbl2->addStretch(1);
    hbl2->setMargin(10);
    topLayout->addLayout(hbl2);
#endif

    topLayout->setStretchFactor(m_table, 1);
    topLayout->setStretchFactor(insertRemoveLayout,1);

    // Connect signals from the spin boxes.
    connect(m_rowsSB, SIGNAL(valueChangedSpecial(int)), 
	    this,     SLOT(setRows(int)));
    connect(m_colsSB, SIGNAL(valueChangedSpecial(int)), 
	    this,     SLOT(setCols(int)));

    
#if 0
    // -- Changed data editor to use top row and leftmost column for
    //    series names and labels so this is no longer necessary.

    connect(m_table->horizontalHeader(), SIGNAL(clicked(int)), 
	    this,                        SLOT(column_clicked(int)) );
    connect(m_table->verticalHeader(),   SIGNAL(clicked(int)),
	    this,                        SLOT(row_clicked(int)) );
#endif
  
    connect(m_table, SIGNAL(valueChanged(int, int)),
	    this,    SLOT(tableChanged(int, int)) );

    // At first, assume that any shrinking of the table is a mistake.
    // A confirmation dialog will make sure that the user knows what
    // (s)he is doing.
    m_userWantsToShrink = false;

    // The data is not modified at the start.
    m_modified          = false;

    // If the cursor starts at cell (0, 0), that is the header row and
    // col, and the user isn't allowed to remove those.
    m_removeRowButton->setEnabled( false );
    m_removeColButton->setEnabled( false );

    // Add tooltips and WhatsThis help.
    addDocs();
}


// Add Tooltips and WhatsThis help to various parts of the Data Editor.
//
void kchartDataEditor::addDocs()
{
    // The rows settings.
    QString rowwhatsthis = i18n("<p><b>Sets the number of rows in the data table."
				"</b><br><br>Each row represents one data set.</p>");
    QToolTip::add(m_rowsSB, i18n("Number of active data rows"));
    QWhatsThis::add(m_rowsLA, rowwhatsthis);
    QWhatsThis::add(m_rowsSB, rowwhatsthis);

    // The columns settings.
    QString colwhatsthis = i18n("<p><b>Sets the number of columns in the data table."
				"</b><br><br>The number of columns defines the number of data values in each data set (row).</p>");
    QToolTip::add(m_colsSB, i18n("Number of active data columns"));
    QWhatsThis::add(m_colsLA, colwhatsthis);
    QWhatsThis::add(m_colsSB, colwhatsthis);

    // The table.
    QToolTip::add(m_table, i18n("Chart data table."));
    
    //GUI
    //The QWhatsThis information below is incorrect since the way that the contents of the table
    //are displayed in the chart depends upon the data format selected (which can be 
    //either "Data in columns" (default) or "Data in rows)
    //The names of the data sets / axes labels are no longer set by clicking on the table 
    //headers - since that was slow to work with and did not allow for keyboard input. 
    //Instead the names are taken from the topmost row and leftmost column.  
    //
    //eg:       Month | Sales 
    //          Jan   | 105
    //          Feb   | 117
    //          March | 120
    //
    //The captions of the header are automatically set to the names of the cells in the topmost row
    //and leftmost column.  This means that if you have more data than will fit in the visible area,
    //you can still see the column names or row names when the table has been scrolled.
    //KSpread could use some functionality like this as well.
    
    
#if 0
    QWhatsThis::add(m_table, i18n("<p>This table contains the data"
    " for the chart.<br><br> Each row is one data set of values."
    " The name of such a data set can be changed in the column header (on the left)"
    " of the table. In a line diagram each row is one line. In a ring diagram each row"
    " is one slice. <br><br> Each column represents one value of each data set."
    " Just like rows you can also change the name of each value in the"
    " column headers (at the top) of the table.  In a bar diagram the number"
    " of columns defines the number of value sets.  In a ring diagram each"
    " column is one ring.</p>"));
#endif

    QToolTip::add( m_insertRowButton, i18n("Insert row") );
    QToolTip::add( m_removeRowButton, i18n("Delete row") );
    QToolTip::add( m_insertColButton, i18n("Insert column") );
    QToolTip::add( m_removeColButton, i18n("Delete column") );
}


// Set the data in the data editor.
//
// The data is taken from the KDChart data.  This method is never
// called when the chart is a part of a spreadsheet.
//
void kchartDataEditor::setData( KChartParams *params, KDChartTableData *dat )
{
    unsigned int  rowsCount;
    unsigned int  colsCount;

    // Get the correct number of rows and columns.
    if ( dat->usedRows() == 0 && dat->usedCols() == 0) { // Data from KSpread
        rowsCount = dat->rows();
        colsCount = dat->cols();
    }
    else {
        rowsCount = dat->usedRows();
        colsCount = dat->usedCols();
    }

    // Empty table
    if ( rowsCount==0 && colsCount==0 ) {
	m_table->setNumRows(1);
	m_table->setNumCols(1);
	resize(600, 300);
	return;
    }

    rowsCount += headerRows();
    colsCount += headerCols();

    // Initiate widgets with the correct rows and columns.
    m_rowsSB->setValue(rowsCount);
    m_colsSB->setValue(colsCount);
#if 0
    m_firstRowAsLabel->setChecked( params->firstRowAsLabel() );
    m_firstColAsLabel->setChecked( params->firstColAsLabel() );
#endif

    // Fill the data from the chart into the editor.
    m_table->setNumRows(rowsCount);
    m_table->setNumCols(colsCount);
    for (unsigned int row = headerRows(); row < rowsCount; row++) {
        for (unsigned int col = headerCols(); col < colsCount; col++) {
            QVariant t = dat->cellVal(row-headerRows(), col-headerCols());

            // Fill it in from the part.
            if (t.isValid()) {
                if ( t.type() == QVariant::Double ) {
		    m_table->setText(row, col, 
				     QString("%1").arg(t.toDouble()));
		}
                else if ( t.type() == QVariant::String )
                    kdDebug(35001) << "I cannot handle strings in the table yet"
                                   << endl;
                else {
                    // nothing on purpose
                }
            }
        }
    }

    // Set column widths.  The default is a little too wide.
    for (unsigned int col = 0; col < colsCount + 1; col++) 
	m_table->setColumnWidth(col, COLUMNWIDTH);

    // and resize the widget to a good size.
    resize(600, 300);
}


// Get the data from the data editor and put it back into the chart.
//
void kchartDataEditor::getData( KChartParams *params, KDChartTableData *dat )
{	
    //Number of rows used as headers
    int labelRows = headerRows();
    //Number of columns used as headers	
    int labelCols = headerCols();
    
    int  numRows = m_table->numRows()-labelRows;
    int  numCols = m_table->numCols()-labelCols;
	
    // Make sure that the data table for the chart is not smaller than
    // the data in the editor.
    if ( static_cast<int>( dat->rows() ) < numRows
	|| static_cast<int>( dat->cols() ) < numCols )
	dat->expand( numRows, numCols );

    dat->setUsedRows( numRows );
    dat->setUsedCols( numCols );

	// Empty table
#if 0
	if ( numRows==1 && numCols==1 && m_table->horizontalHeader()->label(0).isEmpty()
			&& m_table->verticalHeader()->label(0).isEmpty()
			&& m_table->text(0, 0).isEmpty() ) {
		dat->expand(0,0);
		return;
	}
#endif

    // Get all the data.
    for (int row = labelRows ; row < (numRows+labelRows); row++) {
        for (int col = labelCols ; col < (numCols+labelCols); col++) {

            // Get the text and convert to double.
            QString tmp = m_table->text(row, col);
            bool   bOk;
            double val = tmp.toDouble( &bOk );
            if (!bOk)
              val = 0.0;
            // and do the actual setting.
            //t = KoChart::Value( val );
            dat->setCell(row-labelRows,col-labelCols, val);
        }
    }

#if 0
    params->setFirstRowAsLabel( m_firstRowAsLabel->isChecked() );
    params->setFirstColAsLabel( m_firstColAsLabel->isChecked() );
#endif
}


// Set the row labels in the data editor.
//
void kchartDataEditor::setRowLabels(const QStringList &rowLabels)
{
#if 0
    QHeader  *rowHeader = m_table->verticalHeader();
    int       row;
    int       numRows = m_rowsSB->value();

    rowHeader->setLabel(0, "");
	if ( numRows==1 && m_colsSB->value()==1 && m_table->text(0, 0).isEmpty() )
		return;
    for (row = 0; row < numRows; row++) {
	rowHeader->setLabel(row, rowLabels[row]);
    }
#endif

    for (unsigned int i=0; i < rowLabels.count(); i++) {
        m_table->setText(i + headerRows(), 0, rowLabels[i]);    
    }
    
    updateRowHeaders();
}


int kchartDataEditor::headerRows()
{
    return 1;
}

int kchartDataEditor::headerCols()
{
    return 1;
}


// Get the row labels from the data editor.
//
void kchartDataEditor::getRowLabels(QStringList &rowLabels)
{
#if 0
    QHeader  *rowHeader = m_table->verticalHeader();
    int  numRows = m_rowsSB->value();
    int  row;

    rowLabels.clear();
    for (row = 0; row < numRows; row++) {
      rowLabels << rowHeader->label(row);
    }
#endif
    rowLabels.clear();

    for (int i=headerRows();i < m_table->numRows();i++)
    {
        rowLabels << m_table->text(i,0);
    }
}


// Set the column labels in the data editor.
//
void kchartDataEditor::setColLabels(const QStringList &colLabels)
{
#if 0
    QHeader  *colHeader = m_table->horizontalHeader();
    int       col;

    int  numCols = m_colsSB->value();

    colHeader->setLabel(0, "");
	if ( m_rowsSB->value()==1 && numCols==1 && m_table->text(0, 0).isEmpty() )
		return;
    for (col = 0; col < numCols; col++) {
	colHeader->setLabel(col, colLabels[col]);
    }
#endif

    for (unsigned int i = 0; i < colLabels.count(); i++)
    {
        m_table->setText(0,i+headerCols(),colLabels[i]);
    }
    
    updateColHeaders();
}


// Get the column labels from the data editor.
//
void kchartDataEditor::getColLabels(QStringList &colLabels)
{
#if 0
    QHeader  *colHeader = m_table->horizontalHeader();
    int  numCols = m_colsSB->value();
    int  col;

    colLabels.clear();
    for (col = 0; col < numCols; col++) {
	colLabels << colHeader->label(col);
    }
#endif
    
    colLabels.clear();

    for (int i = headerCols(); i < m_table->numCols(); i++) {
        colLabels << m_table->text(0, i); 
    }
}


// ================================================================
//                              Slots


// Slots for the buttons that insert/remove rows/columns.
//

void kchartDataEditor::removeCurrentRow()
{
    int  row = m_table->currentRow();
    
    // Can't remove the header row.
    if ( row == 0 )
	return;

    // Need at least one data row.
    if ( m_table->numRows() == 2 )
	return;

    m_table->removeRow(row);
    m_rowsSB->setValue(m_table->numRows());
    
    if (row == 0)
        updateColHeaders();   
    
    m_modified = true;
}

void kchartDataEditor::removeCurrentColumn()
{
    int  col = m_table->currentColumn();
    
    // Can't remove the header column.
    if ( col == 0 )
	return;

    // Need at least one data column.
    if ( m_table->numCols() == 2 )
	return;

    m_table->removeColumn(col);
    m_colsSB->setValue(m_table->numCols());
    
    if (col == 0)
        updateRowHeaders();
    
    m_modified = true;
}

void kchartDataEditor::insertColumn()
{
    m_table->insertColumns(m_table->currentColumn() + 1, 1);
    m_colsSB->setValue(m_table->numCols());
    updateColHeaders();

    m_modified = true;
}

void kchartDataEditor::insertRow()
{
    m_table->insertRows(m_table->currentRow() + 1, 1);
    m_rowsSB->setValue(m_table->numRows());
    updateRowHeaders();

    m_modified = true;
}    


// Ask user to make sure that (s)he really wants to remove a row or
// column.
//
static int askUserForConfirmation(QWidget *parent)
{
    return KMessageBox::warningContinueCancel(parent,
        i18n("You are about to shrink the data table and remove some values. "
	     "This will lead to loss of existing data in the table "
	     "and/or the headers.\n\n"
	     "This message will not be shown again if you click Continue"));
}


// This slot is called when the spinbox for rows is changed.
//
void kchartDataEditor::setRows(int rows)
{
    kdDebug(35001) << "setRows called: rows = " << rows << endl;;

    // Sanity check.  This should never happen since the spinbox has a
    // minvalue of 1, but just to be sure...
    if (rows < 1) {
	m_rowsSB->setValue(1);
	return;
    }

    int old_rows = m_table->numRows();
    if (rows > old_rows) {
	m_table->setNumRows(rows);

	// Default value for the new rows: empty string
	for (int i = old_rows; i < rows; i++)
	    m_table->verticalHeader()->setLabel(i, "");

	m_modified = true;
    }
    else if (rows < m_table->numRows()) {
	bool ask_user = false;

	// Check if the last row is empty.
	for (int col=0; col<m_table->numCols(); col++) {
	    if (!m_table->text(rows, col).isEmpty()) {
		ask_user = true;
		break;
	    }
	}

	// If it is not, ask if the user really wants to shrink the table.
	if ( ask_user && !m_userWantsToShrink
	    && askUserForConfirmation(this) == KMessageBox::Cancel) {

	    // The user aborts.  Reset the number of rows and return.
	    m_rowsSB->setValue(m_table->numRows());
	    return;
	}

	// Record the fact that the user knows what (s)he is doing.
	if (ask_user)
		m_userWantsToShrink = true;

	// Do the actual shrinking.
	m_table->setNumRows(rows);

	m_modified = true;
    }
}


// This slot is called when the spinbox for columns is changed.
//
void kchartDataEditor::setCols(int cols)
{
    kdDebug(35001) << "setCols called: cols = " << cols << endl;;

    // Sanity check.  This should never happen since the spinbox has a
    // minvalue of 1, but just to be sure...
    if (cols < 1) {
	m_colsSB->setValue(1);
	return;
    }
    
    int  old_cols = m_table->numCols();
    if (cols > old_cols) {
	m_table->setNumCols(cols);

	// Default value for the new columns: empty string.
	for (int i = old_cols; i < cols; i++) {
	    m_table->horizontalHeader()->setLabel(i, "");
	    m_table->setColumnWidth(i, COLUMNWIDTH);
	}

	m_modified = true;
    }
    else if (cols < m_table->numCols()) {
	bool ask_user = false;

	// Check if the last column is empty.
	for (int row=0; row<m_table->numRows(); row++) {
	    if (!m_table->text(row, cols).isEmpty()) {
		ask_user = true;
		break;
	    }
	}

	// If it is not, ask if the user really wants to shrink the table.
	if (ask_user && !m_userWantsToShrink
	    && askUserForConfirmation(this) == KMessageBox::Cancel) {

	    // The user aborts.  Reset the number of rows and return.
	    m_colsSB->setValue(m_table->numCols());
	    return;
	}

	// Record the fact that the user knows what (s)he is doing.
	if (ask_user)
	    m_userWantsToShrink = true;

	// Do the actual shrinking.
	m_table->setNumCols(cols);

	m_modified = true;
    }
}


// Get the new name for a column header.
//
#if 0	// Disabled since the first row / column now contains the labels.
void kchartDataEditor::column_clicked(int column)
{
	bool ok;
    QString name = KInputDialog::getText(i18n("Column Name"), 
					 i18n("Type a new column name:"), 
					 m_table->horizontalHeader()->label(column),
					 &ok, this, 0, new QRegExpValidator(QRegExp(".*"), this) );

    // Rename the column.
    if ( ok ) {
    m_table->horizontalHeader()->setLabel(column, name);
	m_modified = true;
    }
}


// Get the new name for a row header.
//
void kchartDataEditor::row_clicked(int row)
{
	bool ok;
    QString name = KInputDialog::getText(i18n("Row Name"),
					 i18n("Type a new row name:"), 
					 m_table->verticalHeader()->label(row),
					 &ok, this, 0, new QRegExpValidator(QRegExp(".*"), this) );

    // Rename the row.
    if ( ok ) {
    m_table->verticalHeader()->setLabel(row, name);
	m_modified = true;
    }
}
#endif


void  kchartDataEditor::tableChanged(int row, int col)
{
    if (row <= headerRows())
        updateColHeaders();
    if (col <= headerCols())
        updateRowHeaders();

    m_modified = true;
}


void  kchartDataEditor::currentChanged(int row, int col)
{
    m_removeRowButton->setEnabled( row != 0 && m_table->numRows() > 2 );
    m_removeColButton->setEnabled( col != 0 && m_table->numCols() > 2 );
}


void kchartDataEditor::updateRowHeaders()
{
    for (int i=0;i<m_table->numRows();i++)
    {
        QHeader* header=m_table->verticalHeader();
        
        QString tableCellText=m_table->text(i,0);
        
        if (tableCellText == QString::null)
            tableCellText=QString("");
        
        header->setLabel(header->mapToSection(i),tableCellText);
    }
}

void kchartDataEditor::updateColHeaders()
{
    for (int i=0;i<m_table->numCols();i++)
    {
        QHeader* header=m_table->horizontalHeader();
        
        QString tableCellText=m_table->text(0,i);
        
        if (tableCellText == QString::null)
            tableCellText=QString("");
            
        header->setLabel(header->mapToSection(i),tableCellText);
    }
}


// This is a reimplementation of a slot defined in KDialogBase.  The
// reason for the reimplementation is that we need to emit the signal
// with a pointer to this so that we can get the data.
//
void kchartDataEditor::slotApply()
{
    emit applyClicked(this);
}


}  //KChart namespace
