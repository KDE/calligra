#include <qlabel.h>
#include <qspinbox.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qregexp.h>
#include <qwhatsthis.h>
#include <qtooltip.h>
#include <qvalidator.h>

#include <kinputdialog.h>
#include <klocale.h>
#include <kdebug.h>
#include <kmessagebox.h>

#include "kdchart/KDChartAxisParams.h"
#include "kchart_params.h"

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
//                    Class kchartDataEditor


#define COLUMNWIDTH  80

kchartDataEditor::kchartDataEditor(QWidget* parent) :
    KDialogBase(parent, "dataeditor", true, i18n("KChart Data Editor"), 
		KDialogBase::Ok|KDialogBase::Cancel|KDialogBase::Apply,
		KDialogBase::Ok, true)
{
    //setCaption(i18n("KChart Data Editor"));

    QWidget      *page = new QWidget( this );
    setMainWidget(page);

    // Create the main table.
    m_table = new QTable(page);
    m_table->setSelectionMode(QTable::NoSelection);
	m_table->setFocus();

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

    // Start the layout.  The table is at the top.
    QVBoxLayout  *topLayout = new QVBoxLayout( page );
    topLayout->addWidget(m_table);

    // Then, a horizontal layer with the rows and columns settings
    QHBoxLayout  *rowColLayout = new QHBoxLayout(  );
    rowColLayout->addWidget(m_rowsLA);
    rowColLayout->addWidget(m_rowsSB);
    rowColLayout->addWidget(m_colsLA);
    rowColLayout->addWidget(m_colsSB);
    rowColLayout->addStretch(1);
    rowColLayout->setMargin(10);

    topLayout->addLayout(rowColLayout);
    topLayout->setStretchFactor(m_table, 1);

    // Connect signals from the spin boxes.
    connect(m_rowsSB, SIGNAL(valueChangedSpecial(int)), 
	    this,     SLOT(setRows(int)));
    connect(m_colsSB, SIGNAL(valueChangedSpecial(int)), 
	    this,     SLOT(setCols(int)));

    connect(m_table->horizontalHeader(), SIGNAL(clicked(int)), 
	    this,                        SLOT(column_clicked(int)) );
    connect(m_table->verticalHeader(),   SIGNAL(clicked(int)),
	    this,                        SLOT(row_clicked(int)) );

    connect(m_table, SIGNAL(valueChanged(int, int)),
	    this,    SLOT(tableChanged(int, int)) );

    // At first, assume that any shrinking of the table is a mistake.
    // A confirmation dialog will make sure that the user knows what
    // (s)he is doing.
    m_userWantsToShrink = false;

    // The data is not modified at the start.
    m_modified          = false;

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
    QToolTip::add(m_table, i18n("Chart data, each row is a data set. "
			       "Headers can be changed by clicking on them."));
    QWhatsThis::add(m_table, i18n("<p>This table represents the complete data"
    " for the chart.<br><br> Each row is one data set of values."
    " The name of such a data set can be changed in the column header (on the left)"
    " of the table. In a line diagram each row is one line. In a ring diagram each row"
    " is one slice. <br><br> Each column represents one value of each data set."
    " Just like rows you can also change the name of each value in the"
    " column headers (at the top) of the table.  In a bar diagram the number"
    " of columns defines the number of value sets.  In a ring diagram each"
    " column is one ring.</p>"));
}


// Set the data in the data editor.
//
// The data is taken from the KDChart data.  This method is never
// called when the chart is a part of a spreadsheet.
//
void kchartDataEditor::setData( KoChart::Data* dat )
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
    // Initiate widgets with the correct rows and columns.
    m_rowsSB->setValue(rowsCount);
    m_colsSB->setValue(colsCount);
    m_table->setNumRows(rowsCount);
    m_table->setNumCols(colsCount);

    // Fill the data from the chart into the editor.
    for (unsigned int row = 0; row < rowsCount; row++) {
        for (unsigned int col = 0; col < colsCount; col++) {
            KoChart::Value t = dat->cell(row, col);

            // Fill it in from the part.
            if (t.hasValue()) {
                if ( t.isDouble() ) {
		    m_table->setText(row, col, 
				     QString("%1").arg(t.doubleValue()));
		}
                else if ( t.isString() )
                    kdDebug(35001) << "I cannot handle strings in the table yet"
                                   << endl;
                else
                    ; // nothing on purpose
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
void kchartDataEditor::getData( KoChart::Data* dat )
{
    int  numRows = m_rowsSB->value();
    int  numCols = m_colsSB->value();

    // Make sure that the data table for the chart is not smaller than
    // the data in the editor.
    if ( static_cast<int>( dat->rows() ) < numRows
	|| static_cast<int>( dat->cols() ) < numCols )
	dat->expand( numRows, numCols );

    dat->setUsedRows( numRows );
    dat->setUsedCols( numCols );

	// Empty table
	if ( numRows==1 && numCols==1 && m_table->horizontalHeader()->label(0).isEmpty()
			&& m_table->verticalHeader()->label(0).isEmpty()
			&& m_table->text(0, 0).isEmpty() ) {
		dat->expand(0,0);
		return;
	}

    // Get all the data.
    for (int row = 0;row < numRows; row++) {
        for (int col = 0;col < numCols; col++) {
            KoChart::Value t;

	    // Get the text and convert to double.
	    QString tmp = m_table->text(row, col);
	    bool    ok;
	    double  val = tmp.toDouble(&ok);
	    if (!ok)
		val = 0.0;

	    // and do the actual setting.
	    t = KoChart::Value( val );
            dat->setCell(row,col,t);
        }
    }
}


// Set the row labels in the data editor.
//
void kchartDataEditor::setRowLabels(const QStringList &rowLabels)
{
    QHeader  *rowHeader = m_table->verticalHeader();
    int       row;
    int       numRows = m_rowsSB->value();

    rowHeader->setLabel(0, "");
	if ( numRows==1 && m_colsSB->value()==1 && m_table->text(0, 0).isEmpty() )
		return;
    for (row = 0; row < numRows; row++) {
	rowHeader->setLabel(row, rowLabels[row]);
    }
}


// Get the row labels from the data editor.
//
void kchartDataEditor::getRowLabels(QStringList &rowLabels)
{
    QHeader  *rowHeader = m_table->verticalHeader();
    int  numRows = m_rowsSB->value();
    int  row;

    rowLabels.clear();
    for (row = 0; row < numRows; row++) {
      rowLabels << rowHeader->label(row);
    }
}


// Set the column labels in the data editor.
//
void kchartDataEditor::setColLabels(const QStringList &colLabels)
{
    QHeader  *colHeader = m_table->horizontalHeader();
    int       col;

    int  numCols = m_colsSB->value();

    colHeader->setLabel(0, "");
	if ( m_rowsSB->value()==1 && numCols==1 && m_table->text(0, 0).isEmpty() )
		return;
    for (col = 0; col < numCols; col++) {
	colHeader->setLabel(col, colLabels[col]);
    }
}


// Get the column labels from the data editor.
//
void kchartDataEditor::getColLabels(QStringList &colLabels)
{
    QHeader  *colHeader = m_table->horizontalHeader();
    int  numCols = m_colsSB->value();
    int  col;

    colLabels.clear();
    for (col = 0; col < numCols; col++) {
      colLabels << colHeader->label(col);
    }
}


// ================================================================
//                              Slots


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

void kchartDataEditor::test()
{
  kdDebug() << "hej!" << endl;
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
void kchartDataEditor::column_clicked(int column)
{
	bool ok;
    QString name = KInputDialog::getText(i18n("Column name"), 
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
    QString name = KInputDialog::getText(i18n("Row name"),
					 i18n("Type a new row name:"), 
					 m_table->verticalHeader()->label(row),
					 &ok, this, 0, new QRegExpValidator(QRegExp(".*"), this) );

    // Rename the row.
    if ( ok ) {
    m_table->verticalHeader()->setLabel(row, name);
	m_modified = true;
    }
}


void  kchartDataEditor::tableChanged(int row, int col)
{
    m_modified = true;
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
