#include <qlabel.h>
#include <qspinbox.h>
#include <qlayout.h>
#include <qwhatsthis.h>
#include <qtooltip.h>

#include <klocale.h>
#include <kdebug.h>
#include <kmessagebox.h>

#include "kdchart/KDChartAxisParams.h"
#include "kchart_params.h"

#include "kchartDataEditor.h"
#include "kchartDataEditor.moc"

namespace KChart
{


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

    // Create the Rows setting
    m_rowsLA = new QLabel( i18n("# Rows:" ), page );
    m_rowsLA->resize( m_rowsLA->sizeHint() );
    m_rowsSB = new QSpinBox( page );
    m_rowsSB->resize( m_rowsSB->sizeHint() );
    m_rowsSB->setMinValue(1);

    // Create the columns setting
    m_colsLA = new QLabel( i18n("# Columns:" ), page );
    m_colsLA->resize( m_colsLA->sizeHint() );
    m_colsSB = new QSpinBox( page );
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
    connect(m_rowsSB, SIGNAL(valueChanged(int)), 
	    this,     SLOT(setRows(int)));
    connect(m_colsSB, SIGNAL(valueChanged(int)), 
	    this,     SLOT(setCols(int)));

    // At first, assume that any shrinking of the table is a mistake.
    // A confirmation dialog will make sure that the user knows what
    // (s)he is doing.
    m_userWantsToShrink = false;

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
			       "First row and column are headers."));
    QWhatsThis::add(m_table, i18n("<p>This table represents the complete data"
    " for the chart.<br><br> Each row is one data set of values."
    " The name of such a data set can be changed in the first column (on the left)"
    " of the table. In a line diagram each row is one line. In a ring diagram each row"
    " is one slice. <br><br> Each column represents one value of each data set."
    " Just like rows you can also change the name of each value in the"
    " first row (at the top) of the table. In a bar diagram the number of columns"
    " defines the number of value sets. In a ring diagram each column is one ring.</p>"));

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

    // Initiate widgets with the correct rows and columns.
    m_rowsSB->setValue(rowsCount);
    m_colsSB->setValue(colsCount);
    m_table->setNumRows(rowsCount + 1);
    m_table->setNumCols(colsCount + 1);

    // Fill the data from the chart into the editor.
    for (unsigned int row = 0; row < rowsCount; row++) {
        for (unsigned int col = 0; col < colsCount; col++) {
            KoChart::Value t = dat->cell(row, col);

            // Fill it in from the part.
            if (t.hasValue()) {
                if ( t.isDouble() ) {
		    m_table->setText(row + 1, col + 1, 
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
    for (unsigned int col = 1; col < colsCount + 1; col++) 
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

    // Get all the data.
    for (int row = 0;row < numRows; row++) {
        for (int col = 0;col < numCols; col++) {
            KoChart::Value t;

	    // Get the text and convert to double.
	    QString tmp = m_table->text(row + 1, col + 1);
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
    for (row = 0; row < numRows; row++) {
	rowHeader->setLabel(row + 1, QString("%1").arg(row + 1));

        if ( rowLabels[row].isNull() )
	    m_table->setText(row + 1, 0, "");
	else
	    m_table->setText(row + 1, 0, rowLabels[row]);
    }
}


// Get the row labels from the data editor.
//
void kchartDataEditor::getRowLabels(QStringList &rowLabels)
{
    int  numRows = m_rowsSB->value();
    int  row;

    rowLabels.clear();
    for (row = 0; row < numRows; row++) {
	rowLabels << m_table->text(row + 1, 0);
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
    for (col = 0; col < numCols; col++) {
	colHeader->setLabel(col + 1, QString("%1").arg(col + 1));

        if( colLabels[col].isNull() )
	    m_table->setText(0, col + 1, "");
	else
	    m_table->setText(0, col + 1, colLabels[col]);
    }
}


// Get the column labels from the data editor.
//
void kchartDataEditor::getColLabels(QStringList &colLabels)
{
    int  numCols = m_colsSB->value();
    int  col;

    colLabels.clear();
    for (col = 0; col < numCols; col++) {
	colLabels << m_table->text(0, col + 1);
    }
}


// FIXME: This function is obsolete and will be removed soon.

void kchartDataEditor::setLegend( const QStringList &legend )
{
    QHeader  *rowHeader = m_table->verticalHeader();
    int       row;

    int  numRows = m_rowsSB->value();

    rowHeader->setLabel(0, "");
    for (row = 0;row < numRows; row++) {
	kdDebug(35001) << "Set row header for row " << row << ": " << endl;

	rowHeader->setLabel(row + 1, QString("%1").arg(row + 1));
        if ( legend[row].isNull() ) {
	    m_table->setText(row + 1, 0, QString("Data Set %1").arg(row + 1));
	}
	else {
            QString tmp=legend[row];
	    m_table->setText(row + 1, 0, tmp);
	    kdDebug(35001) << tmp << endl;
        }
    }

    // The empty row below the filled in ones.
    //rowHeader->setLabel(row, QString("Data Set %1").arg(row + 1));
}


// FIXME: This function is obsolete and will be removed soon.

void kchartDataEditor::getLegend( KChartParams* params )
{
    int  numRows = m_rowsSB->value();

    for( int row = 0; row < numRows; row++ ) {
	params->setLegendText( row, m_table->text(row + 1, 0) );
    }
}


// FIXME: This function is obsolete and will be removed soon.

void kchartDataEditor::setXLabel( const QStringList & xlbl )
{
    QHeader  *colHeader = m_table->horizontalHeader();

    kdDebug(35001) << "setXLabel called." << endl;
    int  numCols = m_colsSB->value();

    for (int col = 0;col < numCols; col++) {
        if ( xlbl[col].isNull() )
	    kdDebug(35001) << "(NULL)" << endl;
	else
	    kdDebug(35001) << xlbl[col] << endl;;
    }

    colHeader->setLabel(0, "");
    for (int col = 0;col < numCols; col++) {
	colHeader->setLabel(col + 1, QString("%1").arg(col + 1));
        if ( xlbl[col].isNull() ) {
	    m_table->setText(0, col + 1, QString("Value %1").arg(col + 1));
	}
	else {
	    m_table->setText(0, col + 1, xlbl[col]);
        }
    }
}


// FIXME: This function is obsolete and will be removed soon.

void kchartDataEditor::getXLabel( KChartParams* params )
{
    KDChartAxisParams bottomparms = params->axisParams( KDChartAxisParams::AxisPosBottom );
    // Temporarily store all values in a list, so we don't need to
    // overwrite exisiting entries, if new list is empty
    QStringList  newListStringLong;
    QStringList  newListStringShort;

    int  numCols = m_colsSB->value();

    bool filled = FALSE;
    for ( int col = 0; col < numCols; col++ )
    {
	QString tmp = m_table->text(0, col + 1);
        if( !tmp.isEmpty() )
            filled = TRUE;
        newListStringLong.append( tmp );
        newListStringShort.append( tmp.left( 3 ) );
    }
    
    //Only change default value if at least one xlabel entry is filled.
    if ( filled ) {
        *longLabels = newListStringLong;
        *shortLabels = newListStringShort;

        bottomparms.setAxisLabelStringLists( longLabels, shortLabels );
        params->setAxisParams( KDChartAxisParams::AxisPosBottom, bottomparms );
    }
    else {
        longLabels->clear();
        shortLabels->clear();
    }
}


// ================================================================
//                              Slots


// Ask user to make sure that (s)he really wants to remove a row or
// column.
//

static int askUserForConfirmation()
{
    return KMessageBox::warningContinueCancel(0,
        i18n("You are about to shrink the data table. "
	     "This may lead to loss of existing data in the table "
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

    int  oldNumRows = m_table->numRows();
    if (rows + 1 > m_table->numRows()) {
	m_table->setNumRows(rows + 1);

	    // Set the numerical label for the new rows.
	for (int row = oldNumRows; row < rows + 1; row++)
	    m_table->verticalHeader()->setLabel(row, QString("%1").arg(row));
    }
    else if (rows + 1 < m_table->numRows()) {
	// Check that the user really wants to shrink the table.
	if (!m_userWantsToShrink
	    && askUserForConfirmation() == KMessageBox::Cancel) {

	    // The user aborts.  Reset the number of rows and return.
	    m_rowsSB->setValue(m_table->numRows() - 1);
	    return;
	}
	
	// Record the fact that the user knows what (s)he is doing.
	m_userWantsToShrink = true;

	// Do the actual shrinking.
	m_table->setNumRows(rows + 1);
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

    if (cols + 1 > m_table->numCols()) {
	int  oldNumCols = m_table->numCols();

	m_table->setNumCols(cols + 1);

	// Set the width and numerical label for the new columns.
	// Note that this need not be an increase of just one, but can
	// be any number of new columns.
	for (int col = oldNumCols; col < cols + 1; col++) {
	    m_table->setColumnWidth(col, COLUMNWIDTH);
	    m_table->horizontalHeader()->setLabel(col, 
						  QString("%1").arg(col));
	}
    }
    else if (cols + 1 < m_table->numCols()) {
	// Check that the user really wants to shrink the table.
	if (!m_userWantsToShrink
	    && askUserForConfirmation() == KMessageBox::Cancel) {

	    // The user aborts.  Reset the number of rows and return.
	    m_colsSB->setValue(m_table->numCols() - 1);
	    return;
	}
	
	// Record the fact that the user knows what (s)he is doing.
	m_userWantsToShrink = true;

	// Do the actual shrinking.
	m_table->setNumCols(cols + 1);
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
