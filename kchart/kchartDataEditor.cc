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
  
   rowsLA = new QLabel( i18n("# Rows:" ), page );
   rowsLA->resize( rowsLA->sizeHint() );
   QWhatsThis::add(rowsLA, rowwhatsthis);
   rowsSB = new QSpinBox( page );
   rowsSB->resize( rowsSB->sizeHint() );
   QToolTip::add(rowsSB, i18n("Number of active data rows"));
   QWhatsThis::add(rowsSB, rowwhatsthis);
  
   colsLA = new QLabel( i18n("# Columns:" ), page );
   colsLA->resize( colsLA->sizeHint() );
   QWhatsThis::add(colsLA, colwhatsthis);
   colsSB = new QSpinBox( page );
   colsSB->resize( colsSB->sizeHint() );
   QToolTip::add(colsSB, i18n("Number of active data columns"));
   QWhatsThis::add(colsSB, colwhatsthis);

   QVBoxLayout  *topLayout = new QVBoxLayout( page );
   topLayout->addWidget(m_table);

   QHBoxLayout  *rowColLayout = new QHBoxLayout(  );
   rowColLayout->addWidget(rowsLA);
   rowColLayout->addWidget(rowsSB);
   rowColLayout->addWidget(colsLA);
   rowColLayout->addWidget(colsSB);
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
    rowsSB->setValue(rowsCount);
    colsSB->setValue(colsCount);
    m_table->setNumRows(rowsCount);
    m_table->setNumCols(colsCount);
#endif
    m_table->setNumRows(rowsCount + 1);
    m_table->setNumCols(colsCount + 1);

    for (unsigned int row = 0; row < rowsCount; row++)
        for (unsigned int col = 0; col < colsCount; col++) {
            kdDebug(35001) << "Set dialog cell for " << row << "," << col << endl;
            KoChart::Value t = dat->cell(row,col);
            // fill it in from the part
            if (t.hasValue()) {
                if( t.isDouble() ) {
                    //_widget->fillCell(row, col, t.doubleValue());
		    m_table->setText(row + 1, col + 1, 
				     QString("%1").arg(t.doubleValue()));
		}
                else if( t.isString() )
                    kdDebug(35001) << "I cannot handle strings in the table yet"
                                   << endl;
                else
                    ; // nothing on purpose
            }
        }
}


// Get the data from the data editor and put it into the chart.
//
void kchartDataEditor::getData( KoChart::Data* dat )
{
#if 0
    int  numRows = _widget->usedRows()
    int  numCols = _widget->usedCols()
#else
    int  numRows = rowsSB->value();
    int  numCols = colsSB->value();
#endif

#if 0
    // Make sure that the data table is not smaller than the used data
    if( static_cast<int>( dat->rows() ) < _widget->usedRows() ||
        static_cast<int>( dat->cols() ) < _widget->usedCols() )
	dat->expand( _widget->usedRows(), _widget->usedCols() );

    dat->setUsedRows( _widget->usedRows() );
    dat->setUsedCols( _widget->usedCols() );
#else
    // Make sure that the data table is not smaller than the used data
    if( static_cast<int>( dat->rows() ) < numRows
	|| static_cast<int>( dat->cols() ) < numCols )
	dat->expand( numRows, numCols );

    dat->setUsedRows( numRows );
    dat->setUsedCols( numCols );
#endif

    for (int row = 0;row < numRows; row++) {
        for (int col = 0;col < numCols; col++) {
            // m_pData->setYValue( row, col, _widget->getCell(row,col) );
            KoChart::Value t;
            //double val =  _widget->getCell(row,col);

	    QString tmp = m_table->text(row + 1, col + 1);
	    bool    ok;
	    double  val = tmp.toLong(&ok);
	    if (!ok)
		val = 0.0;

	    t = KoChart::Value( val );
            kdDebug(35001) << "Set cell for " << row << "," << col << endl;
            dat->setCell(row,col,t);
            //   maxY = _widget->getCell(row,col) > maxY ? _widget->getCell(row,col) : maxY;
        }
    }
}

void kchartDataEditor::setLegend( const QStringList &legend )
{
    QHeader  *rowHeader = m_table->verticalHeader();
    int       row;

#if 0
    int  numRows = _widget->usedRows()
#else
    int  numRows = rowsSB->value();
#endif

    rowHeader->setLabel(0, "");
    for (row = 0;row < numRows; row++) {
	kdDebug(35001) << "Set row header for row " << row << ": " << endl;
;
        if( legend[row].isNull() ) {
	    rowHeader->setLabel(row + 1, QString("%1").arg(row + 1));
	    m_table->setText(row + 1, 0, QString("Data Set %1").arg(row + 1));

	}
	else {
            QString tmp=legend[row];
            //_widget->fillY(row,tmp);
	    //rowHeader->setLabel(row, tmp);
	    m_table->setText(row + 1, 0, tmp);
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
    int  numRows = rowsSB->value();
#endif

    for( int row = 0; row < numRows; row++ ) {
	//params->setLegendText( row, _widget->getY( row ) );
	params->setLegendText( row, m_table->text(row + 1, 0) );
    }
}


void kchartDataEditor::setXLabel( const QStringList & xlbl )
{
    QHeader  *colHeader = m_table->horizontalHeader();

#if 0
    int  numCols = _widget->usedCols()
#else
    int  numCols = colsSB->value();
#endif

    colHeader->setLabel(0, "");
    for (int col = 0;col < numCols; col++) {
	colHeader->setLabel(col + 1, QString("%1").arg(col + 1));
        if( xlbl[col].isNull() ) {
	    //colHeader->setLabel(col, QString("Value %1").arg(col + 1));
	    m_table->setText(0, col + 1, QString("Value %1").arg(col + 1));
	}
	else {
            QString tmp=xlbl[col];
            //_widget->fillX(col,tmp);
	    //colHeader->setLabel(col, tmp);
	    m_table->setText(0, col + 1, tmp);
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
    int  numCols = colsSB->value();
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

}  //KChart namespace
