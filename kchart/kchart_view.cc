/**
 * $Id$
 *
 * Kalle Dalheimer <kalle@kde.org>
 */

#include "kchart_view.h"
#include "kchart_global.h"
#include "kchart_part.h"
#include "kchartWizard.h"
#include "kchartDataEditor.h"

#include <qpainter.h>
#include <kaction.h>
#include <kglobal.h>

//#include "sheetdlg.h"

QFont *theKChartTinyFont = NULL; 
QFont *theKChartSmallFont = NULL; 
QFont *theKChartMediumFont = NULL; 
QFont *theKChartLargeFont = NULL; 
QFont *theKChartGiantFont = NULL; 
// i don't really understand this stuff, but I need it to start 
// kchart. Probably some intermediate solution

KChartView::KChartView( KChartPart* part, QWidget* parent, const char* name )
    : ContainerView( part, parent, name )
{
    m_wizard = new KAction( tr("Customize with &wizard"),
			    KChartBarIcon("wizard"), 0,
			    this, SLOT( wizard() ),
			    actionCollection(), "wizard");
    m_edit = new KAction( tr("&Edit data"), KChartBarIcon("pencil"), 0,
			 this, SLOT( edit() ),
                         actionCollection(), "edit");
    m_loadconfig = new KAction( tr("Load config"), KChartBarIcon("loadconfig"),
				0,
				this, SLOT( loadConfig() ),
				actionCollection(), "loadconfig");
    m_saveconfig = new KAction( tr("Save config"), KChartBarIcon("saveconfig"),
				0,
				this, SLOT( saveConfig() ),
				actionCollection(), "saveconfig");
    // initialize the configuration
    //    loadConfig();
}

void KChartView::paintEvent( QPaintEvent* ev )
{
    QPainter painter;
    painter.begin( this );

    // ### TODO: Scaling

    // Let the document do the drawing
    // PENDING(kalle) Do double-buffering if we are a widget
	part()->paintEverything( painter, ev->rect(), FALSE, this );

    painter.end();
}

void KChartView::edit()
{
  kchartDataEditor ed;
  KChartData *dat = (( (KChartPart*)part())->data());
  ed.setData(dat);
  if (ed.exec() != QDialog::Accepted) {
    return;
  }
  ed.getData(dat);
  

  /*
    qDebug("EDIT called");
    QDialog *_dlg = new QDialog(0,"SheetDlg",true);
    SheetDlg *_widget = new SheetDlg(_dlg,"SheetWidget");
    _widget->setGeometry(0,0,520,400);
    _widget->show();
    _dlg->resize(520,400);
    _dlg->setMaximumSize(_dlg->size());
    _dlg->setMinimumSize(_dlg->size());
    
    // maybe the call for initRandomData here
    cerr << "Now display \n";
    KChartData *dat = (( (KChartPart*)part())->data());
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
    cerr << "Here comes the dialog!\n";
    // OK pressed
  
    if (_dlg->exec() != QDialog::Accepted) {
      return;
    }
    //KChartData *m_pData = new KChartData(_widget->rows());
      for (col = 0;col < _widget->cols();col++) {
      m_pData->setXValue( col, (const char*)_widget->getX(col));
      }
    for (int row = 0;row < _widget->rows();row++) {
      for (int col = 0;col < _widget->cols();col++) {
	// m_pData->setYValue( row, col, _widget->getCell(row,col) );
	KChartValue t; 
	double val =  _widget->getCell(row,col);
	t.exists= true;
	t.value.setValue(val);
	cerr << "Set cell for " << row << "," << col << "\n";
	dat->setCell(row,col,t);
	//   maxY = _widget->getCell(row,col) > maxY ? _widget->getCell(row,col) : maxY;
      }
    }
      maxY++;
      
      m_pDoc->setChartData(m_pData);
      m_pDoc->chart().setYMaxValue(maxY);
      m_pDoc->chart().setYTicksNum(maxY);
      m_pDoc->chart().repaintChart( this );    
    */ 
      
  // delete dialog
    //  delete _widget; _widget = 0;
    //delete _dlg; _dlg = 0;
}

void KChartView::wizard()
{
    qDebug("Wizard called");
    kchartWizard *wiz =
      new kchartWizard((KChartPart*)part(), this, "KChart Wizard", true);
    qDebug("Executed. Now, display it");
    wiz->exec();
    qDebug("Ok, executed...");
}

void KChartView::saveConfig() {
    qDebug("Save config...");
    ((KChartPart*)part())->saveConfig( KGlobal::config() );
}

void KChartView::loadConfig() {
    qDebug("Load config...");
    KGlobal::config()->reparseConfiguration();
    ((KChartPart*)part())->loadConfig( KGlobal::config() );
}


#include "kchart_view.moc"
