#include "kchartDataEditor.h"
#include "kchartDataEditor.moc"
#include "klocale.h"
#include <kdebug.h>

kchartDataEditor::kchartDataEditor() :
  KDialog(0,"KChart Data Editor",true) {
      setCaption(i18n("KChart Data Editor"));
  _widget = new SheetDlg(this,"SheetWidget");
  _widget->setGeometry(0,0,520,400);
  _widget->show();
  resize(520,400);
  setMaximumSize(size());
  setMinimumSize(size());

}

void kchartDataEditor::setData(KoChart::Data* dat) {
  _widget->setUsedRows( dat->usedRows() );
  _widget->setUsedCols( dat->usedCols() );

  for (unsigned int row = 0;row != dat->rows();row++)
    for (unsigned int col = 0; col !=dat->cols(); col++) {
      kdDebug(35001) << "Set dialog cell for " << row << "," << col << endl;
      KoChart::Value t = dat->cell(row,col);
      // fill it in from the part
      if (t.isValid()) {
        switch(t.type()) {
        case QVariant::Double:
          _widget->fillCell(row, col, t.toDouble());
          break;
        case QVariant::String:
          kdDebug(35001) << "A string in the table I cannot handle this yet"
                         << endl;
          break;
        default:
          break;
        }
      }
    }
}

void kchartDataEditor::getData(KoChart::Data* dat) {
  // Make sure that the data table is not smaller than the used data
  if( dat->rows() < _widget->usedRows() ||
	  dat->cols() < _widget->usedCols() )
	dat->expand( _widget->usedRows(), _widget->usedCols() );

  dat->setUsedRows( _widget->usedRows() );
  dat->setUsedCols( _widget->usedCols() );

    for (int row = 0;row < _widget->rows();row++) {
      for (int col = 0;col < _widget->cols();col++) {
        // m_pData->setYValue( row, col, _widget->getCell(row,col) );
        KoChart::Value t;
        double val =  _widget->getCell(row,col);
        if( ( row >= _widget->rows() )  ||
            ( col >= _widget->cols() ) )
        { /*t.exists = false; */ }
        else
            t = QVariant( val );
        kdDebug(35001) << "Set cell for " << row << "," << col << endl;
        dat->setCell(row,col,t);
        //   maxY = _widget->getCell(row,col) > maxY ? _widget->getCell(row,col) : maxY;
      }
    }
}

void kchartDataEditor::setLegend(QStringList legend)
{
for (int row = 0;row < _widget->rows();row++)
        {
        if(!legend[row].isNull())
                {
                QString tmp=legend[row];
                _widget->fillY(row,tmp);
                }
        }
}

void kchartDataEditor::getLegend(KChartParameters* params)
{
params->legend.clear();
for (int row = 0;row < _widget->rows();row++)
        {
        if(! (row >= _widget->usedRows()) )
                {
                params->legend+=_widget->getY(row);
                }

        }

}

void kchartDataEditor::setXLabel(QStringList xlbl)
{
for (int col = 0;col < _widget->cols();col++)
        {
        if(!xlbl[col].isNull())
                {
                QString tmp=xlbl[col];
                _widget->fillX(col,tmp);
                }
        }
}

void kchartDataEditor::getXLabel(KChartParameters* params)
{

params->xlbl.clear();
for (int col = 0;col < _widget->cols();col++)
        {
        if(! (col >= _widget->usedCols()) )
                {
                params->xlbl+=_widget->getX(col);
                }

        }
}

