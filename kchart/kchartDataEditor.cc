#include "kchartDataEditor.h"
#include "kchartDataEditor.moc"
#include "klocale.h"

kchartDataEditor::kchartDataEditor() :
  KDialog(0,i18n("KChart Data Editor"),true) {
  _widget = new SheetDlg(this,"SheetWidget");
  _widget->setGeometry(0,0,520,400);
  _widget->show();
  resize(520,400);
  setMaximumSize(size());
  setMinimumSize(size());

}

void kchartDataEditor::setData(KChartData* dat) {
  for (int row = 0;row != dat->rows();row++)
    for (int col = 0; col !=dat->cols(); col++) {
      cerr << "Set dialog cell for " << row << "," << col << "\n";
      KChartValue t = dat->cell(row,col);
      // fill it in from the part
      if (t.exists) {
	switch(t.value.type()) {
	case QVariant::Double:
	  _widget->fillCell(row, col, t.value.doubleValue());
	  break;
	case QVariant::String:
	  cerr << "A string in the table I cannot handle this yet\n";
	  break;
	default:
	  break;
	}
      }
    }
}

void kchartDataEditor::getData(KChartData* dat) {
    for (int row = 0;row < _widget->rows();row++) {
      for (int col = 0;col < _widget->cols();col++) {
	// m_pData->setYValue( row, col, _widget->getCell(row,col) );
	KChartValue t; 
	double val =  _widget->getCell(row,col);
	if( ( row >= _widget->usedRows() )  ||
	    ( col >= _widget->usedCols() ) )
	    t.exists = false;
	else
	    t.exists= true;
	t.value.setValue(val);
	cerr << "Set cell for " << row << "," << col << "\n";
	dat->setCell(row,col,t);
	//   maxY = _widget->getCell(row,col) > maxY ? _widget->getCell(row,col) : maxY;
      }
    }

}
