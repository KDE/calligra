/**
 * $Id$
 *
 * Kalle Dalheimer <kalle@kde.org>
 */

#include "kchart_view.h"
#include "kchart_global.h"
#include "kchart_part.h"
#include "kchartWizard.h"

#include <qpainter.h>
#include <kaction.h>
#include <kglobal.h>

#include "sheetdlg.h"

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
    m_cut = new KAction( tr("&Cut"), KChartBarIcon("editcut"), 0, this, SLOT( cut() ),
                         actionCollection(), "cut");
    m_wizard = new KAction( tr("Customize with &wizard"),
			    KChartBarIcon("wizard"), 0,
			    this, SLOT( wizard() ),
			    actionCollection(), "wizard");
    m_edit = new KAction( tr("&Edit"), KChartBarIcon("editcut"), 0,
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

void KChartView::cut()
{
    qDebug("CUT called");
}

void KChartView::edit()
{
    qDebug("EDIT called");
    QDialog *_dlg = new QDialog(0,"SheetDlg",true);
    SheetDlg *_widget = new SheetDlg(_dlg,"SheetWidget");
    _widget->setGeometry(0,0,520,400);
    _widget->show();
    _dlg->resize(520,400);
    _dlg->setMaximumSize(_dlg->size());
    _dlg->setMinimumSize(_dlg->size());
    
    // fill cells
    int col,row;
    /*
    double maxY = 0;
    for (row = 0;row < 4;row++)
      for (col = 0;col < 4;col++) {
	_widget->fillCell(row,col,row+col);
      }
    _dlg->exec();
    */

    KChartData *dat = ((KChartPart*)part())->data();

    // initialize some data, if there is none
    if (dat->rows() == 0) {
      cerr << "Initialize with some data!!!\n";
      dat->expand(4,4);
      for (row = 0;row < 2;row++)
	for (col = 0;col < 2;col++) {
	  //	  _widget->fillCell(row,col,row+col);
	  KChartValue t; 
	  t.exists= true;
	  t.value.setValue(row+col);
	  cerr << "Set cell for " << row << "," << col << "\n";
	  dat->setCell(row,col,t);
	}
      //      _dlg->exec();
    }

    cerr << "Now display \n";

    for (row = 0;row != dat->rows();row++)
      for (col = 0; col !=dat->cols(); col++) {
	cerr << "Set dialog cell for " << row << "," << col << "\n";
	KChartValue t = dat->cell(row,col);
	// fill it in from the part
	if (t.exists) {
	  switch(t.value.type()) {
	  case QVariant::Int:
	    _widget->fillCell(row, col, t.value.intValue());
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
    _dlg->exec();
    cerr << "exec done?\n";

    /*
    for (col = 0;(unsigned int)col <= m_pDoc->chartData()->maxPos();col++)
      _widget->fillX(col,m_pDoc->chartData()->xValue(col));

    // OK pressed

  if (_dlg->exec() == QDialog::Accepted)
    {
      KChartData *m_pData = new KChartData(_widget->rows());

      for (col = 0;col < _widget->cols();col++)
	m_pData->setXValue( col, (const char*)_widget->getX(col));

      for (row = 0;row < _widget->rows();row++)
	{
	  for (col = 0;col < _widget->cols();col++)
	    {
	      m_pData->setYValue( row, col, _widget->getCell(row,col) );
	      maxY = _widget->getCell(row,col) > maxY ? _widget->getCell(row,col) : maxY;
	    }
	}

      maxY++;
       
      m_pDoc->setChartData(m_pData);
      m_pDoc->chart().setYMaxValue(maxY);
      m_pDoc->chart().setYTicksNum(maxY);
      m_pDoc->chart().repaintChart( this );    
    } 
    */
  
  // delete dialog
  delete _widget; _widget = 0;
  delete _dlg; _dlg = 0;
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
