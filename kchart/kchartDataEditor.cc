#include <qlabel.h>
#include <qspinbox.h>
#include <qlayout.h>
#include <qwhatsthis.h>
#include <qtooltip.h>

#include <klocale.h>
#include <kdebug.h>

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

   m_table = new QTable(page);

   QString rowwhatsthis = i18n("<p><b>Sets the number of rows in the data table."
			       "</b><br><br>Each row represents one data set.</p>");
   QString colwhatsthis = i18n("<p><b>Sets the number of columns in the data table."
			       "</b><br><br>The number of columns defines the number of data values in each data set (row).</p>");
  
   m_rowsLA = new QLabel( i18n("# Rows:" ), page );
   m_rowsLA->resize( m_rowsLA->sizeHint() );
   QWhatsThis::add(m_rowsLA, rowwhatsthis);
   m_rowsSB = new QSpinBox( page );
   m_rowsSB->resize( m_rowsSB->sizeHint() );
   QToolTip::add(m_rowsSB, i18n("Number of active data rows"));
   QWhatsThis::add(m_rowsSB, rowwhatsthis);
  
   m_colsLA = new QLabel( i18n("# Columns:" ), page );
   m_colsLA->resize( m_colsLA->sizeHint() );
   QWhatsThis::add(m_colsLA, colwhatsthis);
   m_colsSB = new QSpinBox( page );
   m_colsSB->resize( m_colsSB->sizeHint() );
   QToolTip::add(m_colsSB, i18n("Number of active data columns"));
   QWhatsThis::add(m_colsSB, colwhatsthis);

   QVBoxLayout  *topLayout = new QVBoxLayout( page );
   topLayout->addWidget(m_table);

   QHBoxLayout  *rowColLayout = new QHBoxLayout(  );
   rowColLayout->addWidget(m_rowsLA);
   rowColLayout->addWidget(m_rowsSB);
   rowColLayout->addWidget(m_colsLA);
   rowColLayout->addWidget(m_colsSB);
   rowColLayout->addStretch(1);
   rowColLayout->setMargin(10);

   topLayout->addLayout(rowColLayout);
   topLayout->setStretchFactor(m_table, 1);

#if 0
    _widget = new SheetDlg(this,"SheetWidget");
    _widget->setGeometry(0,0,520,400);
    _widget->show();

    resize(520,800);
    setMaximumSize(size());
    setMinimumSize(size());
#endif

    connect(this, SIGNAL(applyClicked()), this, SLOT(apply()));
}

// Set the data in the data editor.
//
// The data is taken from the KDChart data.  This method is never
// called when the chart is a part of a spreadsheet.
//
void kchartDataEditor::setData( KoChart::Data* dat )
{
    unsigned int rowsCount, colsCount;
    if ( dat->usedRows() == 0 && dat->usedCols() == 0) { // Data from KSpread
        rowsCount = dat->rows();
        colsCount = dat->cols();
    }
    else {
        rowsCount = dat->usedRows();
        colsCount = dat->usedCols();
    }

#if 0
    _widget->setUsedRows( rowsCount );
    _widget->setUsedCols( colsCount );
#else
    m_rowsSB->setValue(rowsCount);
    m_colsSB->setValue(colsCount);
    m_table->setNumRows(rowsCount);
    m_table->setNumCols(colsCount);
#endif
    m_table->setNumRows(rowsCount + 1);
    m_table->setNumCols(colsCount + 1);

    for (unsigned int row = 0; row < rowsCount; row++) {
        for (unsigned int col = 0; col < colsCount; col++) {
            kdDebug(35001) << "Set dialog cell for " << row << "," << col << endl;
            KoChart::Value t = dat->cell(row,col);
            // fill it in from the part
            if (t.hasValue()) {
                if( t.isDouble() ) {
#if 0
		    _widget->fillCell(row, col, t.doubleValue());
#else
		    m_table->setText(row + 1, col + 1, 
				     QString("%1").arg(t.doubleValue()));
#endif
		}
                else if( t.isString() )
                    kdDebug(35001) << "I cannot handle strings in the table yet"
                                   << endl;
                else
                    ; // nothing on purpose
            }
        }
    }

    for (unsigned int col = 1; col < colsCount + 1; col++) 
      m_table->setColumnWidth(col, COLUMNWIDTH);
    resize(600,300);

    kdDebug(35001) << "Column width: " << m_table->columnWidth(1) << endl;
}


// Get the data from the data editor and put it into the chart.
//
void kchartDataEditor::getData( KoChart::Data* dat )
{
#if 0
    int  numRows = _widget->usedRows()
    int  numCols = _widget->usedCols()
#else
    int  numRows = m_rowsSB->value();
    int  numCols = m_colsSB->value();
#endif

#if 0
    // Make sure that the data table is not smaller than the used data
    if( static_cast<int>( dat->rows() ) < _widget->usedRows() ||
        static_cast<int>( dat->cols() ) < _widget->usedCols() )
	dat->expand( _widget->usedRows(), _widget->usedCols() );
#else
    // Make sure that the data table for the chart is not smaller than
    // the data in the editor.
    if( static_cast<int>( dat->rows() ) < numRows
	|| static_cast<int>( dat->cols() ) < numCols )
	dat->expand( numRows, numCols );
#endif

    dat->setUsedRows( numRows );
    dat->setUsedCols( numCols );

    for (int row = 0;row < numRows; row++) {
        for (int col = 0;col < numCols; col++) {
            KoChart::Value t;
#if 0
            double val =  _widget->getCell(row,col);
#else
	    QString tmp = m_table->text(row + 1, col + 1);
	    bool    ok;
	    double  val = tmp.toLong(&ok);
	    if (!ok)
		val = 0.0;
#endif
	    t = KoChart::Value( val );
            kdDebug(35001) << "Set cell for " << row << "," << col << endl;
            dat->setCell(row,col,t);
        }
    }
}


void kchartDataEditor::setRowLabels(const QStringList &rowLabels)
{
    QHeader  *rowHeader = m_table->verticalHeader();
    int       row;
    int       numRows = m_rowsSB->value();

    rowHeader->setLabel(0, "");
    for (row = 0; row < numRows; row++) {
	rowHeader->setLabel(row + 1, QString("%1").arg(row + 1));

        if( rowLabels[row].isNull() )
	    m_table->setText(row + 1, 0, "");
	else
	    m_table->setText(row + 1, 0, rowLabels[row]);
    }
}

void kchartDataEditor::getRowLabels(QStringList &rowLabels)
{
    int  numRows = m_rowsSB->value();
    int  row;

    rowLabels.clear();
    for (row = 0; row < numRows; row++) {
	rowLabels << m_table->text(row + 1, 0);
    }
}

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

void kchartDataEditor::getColLabels(QStringList &colLabels)
{
    int  numCols = m_colsSB->value();
    int  col;

    colLabels.clear();
    for (col = 0; col < numCols; col++) {
	colLabels << m_table->text(0, col + 1);
    }
}


void kchartDataEditor::setLegend( const QStringList &legend )
{
    QHeader  *rowHeader = m_table->verticalHeader();
    int       row;

#if 0
    int  numRows = _widget->usedRows()
#else
    int  numRows = m_rowsSB->value();
#endif

    rowHeader->setLabel(0, "");
    for (row = 0;row < numRows; row++) {
	kdDebug(35001) << "Set row header for row " << row << ": " << endl;

	rowHeader->setLabel(row + 1, QString("%1").arg(row + 1));
        if( legend[row].isNull() ) {
	    m_table->setText(row + 1, 0, QString("Data Set %1").arg(row + 1));
	}
	else {
            QString tmp=legend[row];
#if 0
            _widget->fillY(row,tmp);
#else
	    m_table->setText(row + 1, 0, tmp);
#endif
	    kdDebug(35001) << tmp << endl;
        }
    }

    // The empty row below the filled in ones.
    //rowHeader->setLabel(row, QString("Data Set %1").arg(row + 1));
}


void kchartDataEditor::getLegend( KChartParams* params )
{
#if 0
    int  numRows = _widget->usedRows()
#else
    int  numRows = m_rowsSB->value();
#endif

    for( int row = 0; row < numRows; row++ ) {
#if 0
	params->setLegendText( row, _widget->getY( row ) );
#else
	params->setLegendText( row, m_table->text(row + 1, 0) );
#endif
    }
}


void kchartDataEditor::setXLabel( const QStringList & xlbl )
{
    QHeader  *colHeader = m_table->horizontalHeader();

    kdDebug(35001) << "setXLabel called." << endl;
#if 0
    int  numCols = _widget->usedCols()
#else
    int  numCols = m_colsSB->value();
#endif
    for (int col = 0;col < numCols; col++) {
        if( xlbl[col].isNull() )
	    kdDebug(35001) << "(NULL)" << endl;
	else
	    kdDebug(35001) << xlbl[col] << endl;;
    }

    colHeader->setLabel(0, "");
    for (int col = 0;col < numCols; col++) {
	colHeader->setLabel(col + 1, QString("%1").arg(col + 1));
        if( xlbl[col].isNull() ) {
	    //colHeader->setLabel(col, QString("Value %1").arg(col + 1));
	    m_table->setText(0, col + 1, QString("Value %1").arg(col + 1));
	}
	else {
            QString tmp=xlbl[col];
#if 0
            _widget->fillX(col,tmp);
#else
	    m_table->setText(0, col + 1, tmp);
#endif
        }
    }
}

void kchartDataEditor::getXLabel( KChartParams* params )
{
    KDChartAxisParams bottomparms = params->axisParams( KDChartAxisParams::AxisPosBottom );
    //Temporarily store all values in a list, so we don't need to overwrite exisiting entries, if new list is empty
    QStringList newListStringLong;
    QStringList newListStringShort;

#if 0
    int  numCols = _widget->usedCols()
#else
    int  numCols = m_colsSB->value();
#endif

    bool filled = FALSE;
    for( int col = 0; col < numCols; col++ )
    {
	QString tmp = m_table->text(0, col + 1);
        if( !tmp.isEmpty() )
            filled = TRUE;
        newListStringLong.append( tmp );
        newListStringShort.append( tmp.left( 3 ) );
    }
    
    //Only change default value if at least one xlabel entry is filled.
    if( filled )
    {
        *longLabels = newListStringLong;
        *shortLabels = newListStringShort;
        bottomparms.setAxisLabelStringLists( longLabels, shortLabels );
        params->setAxisParams( KDChartAxisParams::AxisPosBottom, bottomparms );
    }
    else
    {
        longLabels->clear();
        shortLabels->clear();
    }
}


// ================================================================


void kchartDataEditor::apply()
{
    kdDebug(35001) << "Apply called" << endl;
}


}  //KChart namespace
