#include "kchartDataEditor.h"
#include "kchartDataEditor.moc"
#include <klocale.h>
#include <kdebug.h>

#include "kdchart/KDChartAxisParams.h"
#include "kchart_params.h"

#if 0
kchartDataEditor::kchartDataEditor() :
    KDialog(0,"KChart Data Editor",true)
{
    setCaption(i18n("KChart Data Editor"));
    _widget = new SheetDlg(this,"SheetWidget");
    _widget->setGeometry(0,0,520,400);
    _widget->show();
    resize(520,400);
    setMaximumSize(size());
    setMinimumSize(size());

}
#endif


kchartDataEditor::kchartDataEditor(QWidget* parent) :
    KDialog(parent,"KChart Data Editor",true)
{
    setCaption(i18n("KChart Data Editor"));
    _widget = new SheetDlg(this,"SheetWidget");
    _widget->setGeometry(0,0,520,400);
    _widget->show();
    resize(520,400);
    setMaximumSize(size());
    setMinimumSize(size());

}

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

    _widget->setUsedRows( dat->usedRows() );
    _widget->setUsedCols( dat->usedCols() );

    for (unsigned int row = 0;row != rowsCount;row++)
        for (unsigned int col = 0; col != colsCount; col++) {
            kdDebug(35001) << "Set dialog cell for " << row << "," << col << endl;
            KoChart::Value t = dat->cell(row,col);
            // fill it in from the part
            if (t.hasValue()) {
                if( t.isDouble() )
                    _widget->fillCell(row, col, t.doubleValue());
                else if( t.isString() )
                    kdDebug(35001) << "I cannot handle strings in the table yet"
                                   << endl;
                else
                    ; // nothing on purpose
            }
        }
}


void kchartDataEditor::getData( KoChart::Data* dat )
{
    // Make sure that the data table is not smaller than the used data
    if( static_cast<int>( dat->rows() ) < _widget->usedRows() ||
        static_cast<int>( dat->cols() ) < _widget->usedCols() )
	dat->expand( _widget->usedRows(), _widget->usedCols() );

    dat->setUsedRows( _widget->usedRows() );
    dat->setUsedCols( _widget->usedCols() );

    for (int row = 0;row < _widget->usedRows();row++) {
        for (int col = 0;col < _widget->usedCols();col++) {
            // m_pData->setYValue( row, col, _widget->getCell(row,col) );
            KoChart::Value t;
            double val =  _widget->getCell(row,col);
            if( ( row >= _widget->usedRows() )  ||
                ( col >= _widget->usedCols() ) )
                { /*t.exists = false; */ }
            else
                t = KoChart::Value( val );
            kdDebug(35001) << "Set cell for " << row << "," << col << endl;
            dat->setCell(row,col,t);
            //   maxY = _widget->getCell(row,col) > maxY ? _widget->getCell(row,col) : maxY;
        }
    }
}

void kchartDataEditor::setLegend( const QStringList &legend )
{
    for (int row = 0;row < _widget->rows();row++) {
        if( !legend[row].isNull() ) {
            QString tmp=legend[row];
            _widget->fillY(row,tmp);
        }
    }
}


void kchartDataEditor::getLegend( KChartParams* params )
{
    for( int row = 0; row < _widget->rows(); row++ ) {
        if(! (row >= _widget->usedRows()) ) {
            params->setLegendText( row, _widget->getY( row ) );
        }
    }
}


void kchartDataEditor::setXLabel( const QStringList & xlbl )
{
    for (int col = 0;col < _widget->cols();col++) {
        if( !xlbl[col].isNull() ) {
            QString tmp=xlbl[col];
            _widget->fillX(col,tmp);
        }
    }
}

void kchartDataEditor::getXLabel( KChartParams* params )
{
    KDChartAxisParams bottomparms = params->axisParams( KDChartAxisParams::AxisPosBottom );
    //Temporarily store all values in a list, so we don't need to overwrite exisiting entries, if new list is empty
    QStringList newListStringLong;
    QStringList newListStringShort;

    bool filled = FALSE;
    for( int col = 0; col < _widget->cols(); col++ )
    {
        if( !_widget->getX( col ).isEmpty() )
            filled = TRUE;
        newListStringLong.append( _widget->getX( col ) );
        newListStringShort.append( _widget->getX( col ).left( 3 ) );
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

