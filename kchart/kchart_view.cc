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
#include "kchartparams.h"
#include "kchartBarConfigDialog.h"

#include <qpainter.h>
#include <kaction.h>
#include <kglobal.h>

//#include "sheetdlg.h"

KChartView::KChartView( KChartPart* part, QWidget* parent, const char* name )
    : KoView( part, parent, name )
{
    setInstance( KChartFactory::global() );
    setXMLFile( "kchart.rc" );

    m_wizard = new KAction( tr("Customize with &wizard"),
			    KChartBarIcon("wizard"), 0,
			    this, SLOT( wizard() ),
			    actionCollection(), "wizard");
    m_edit = new KAction( tr("&Edit data"), KChartBarIcon("pencil"), 0,
			 this, SLOT( edit() ),
                         actionCollection(), "edit");
    m_config = new KAction( tr( "&Config" ), KChartBarIcon( "config" ), 0,
			    this, SLOT( config() ),
			    actionCollection(), "config" );
    m_loadconfig = new KAction( tr("Load config"), KChartBarIcon("loadconfig"),
				0,
				this, SLOT( loadConfig() ),
				actionCollection(), "loadconfig");
    m_saveconfig = new KAction( tr("Save config"), KChartBarIcon("saveconfig"),
				0,
				this, SLOT( saveConfig() ),
				actionCollection(), "saveconfig");
    m_chartpie = new KAction( tr("Pie"), KChartBarIcon("cakes"),
				0,
				this, SLOT( pieChart() ),
				actionCollection(), "piechart");
    m_chartline = new KAction( tr("Line"), KChartBarIcon("lines"),
				0,
				this, SLOT( lineChart() ),
				actionCollection(), "linechart");
    m_chartareas = new KAction( tr("Areas"), KChartBarIcon("areas"),
				0,
				this, SLOT( areasChart() ),
				actionCollection(), "areaschart");
    m_chartbars = new KAction( tr("Bars"), KChartBarIcon("bars"),
 				0,
				this, SLOT( barsChart() ),
				actionCollection(), "barschart");



    // initialize the configuration
    //    loadConfig();

    // make sure there is always some test data
    createTempData();
}

void KChartView::paintEvent( QPaintEvent* /*ev*/ )
{
    QPainter painter;
    painter.begin( this );

    // ### TODO: Scaling

    // Let the document do the drawing
    // PENDING(kalle) Do double-buffering if we are a widget
    //    part()->paintEverything( painter, ev->rect(), FALSE, this );
    // paint everything
    koDocument()->paintEverything( painter, rect(), FALSE, this );


    painter.end();
}

void KChartView::updateReadWrite( bool readwrite )
{
#warning TODO 
} 

void KChartView::createTempData()
{
    int row, col;
    int nbrow,nbcol;

    KChartData *dat = ((KChartPart*)koDocument())->data();

    // initialize some data, if there is none
    nbrow=4;
    nbcol=4;
    if (dat->rows() == 0)
    {
	cerr << "Initialize with some data!!!\n";
	dat->expand(4,4);
	for (row = 0;row < nbrow;row++)
	    for (col = 0;col < nbcol;col++)
	    {
		//	  _widget->fillCell(row,col,row+col);
		KChartValue t;
		t.exists= true;
		t.value.setValue((double)row+col);
		cerr << "Set cell for " << row << "," << col << "\n";
		dat->setCell(row,col,t);
	    }
	//      _dlg->exec();
    }

    KChartParameters *params = ((KChartPart*)koDocument())->params();
    if(params->legend.isEmpty())
    	{
        for(unsigned int i=0;i<dat->rows();i++)
                {
                QString tmp;
                tmp="Legend "+tmp.setNum(i);
                params->legend+=tmp;
                }
        }

    if(params->xlbl.isEmpty())
    	{
        for(unsigned int i=0;i<dat->cols();i++)
                {
                QString tmp;
                tmp="Year 200"+tmp.setNum(i);
    	        params->xlbl+=tmp;
                }
    	}

QArray<int> tmpExp(dat->cols()*dat->rows());
QArray<bool> tmpMissing(dat->cols()*dat->rows());

for(unsigned int i=0; i<(dat->cols()*dat->rows()); ++i )
  {
  tmpExp[i]=0;
  tmpMissing[i]=FALSE;
  }
if(params->missing.isEmpty())
	{
  	params->missing=tmpMissing;
  	}
if(params->explode.isEmpty())
	{
  	params->explode=tmpExp;
	}

}


void KChartView::edit()
{
  kchartDataEditor ed;
  KChartParameters* params=((KChartPart*)koDocument())->params();

  KChartData *dat = (( (KChartPart*)koDocument())->data());
  ed.setData(dat);
  ed.setLegend(params->legend);
  ed.setXLabel(params->xlbl);
  if (ed.exec() != QDialog::Accepted) {
    return;
  }
  ed.getData(dat);
  ed.getLegend(params);
  ed.getXLabel(params);
  repaint();
}

void KChartView::wizard()
{
    qDebug("Wizard called");
    kchartWizard *wiz =
      new kchartWizard((KChartPart*)koDocument(), this, "KChart Wizard", true);
    qDebug("Executed. Now, display it");
    wiz->exec();
    repaint();
    qDebug("Ok, executed...");
}


void KChartView::config()
{
    // open a config dialog depending on the chart type
    KChartParameters* params = ((KChartPart*)koDocument())->params();

    switch( params->type ) {
    case KCHARTTYPE_3DBAR:
    case KCHARTTYPE_BAR: {
	KChartBarConfigDialog* d = new KChartBarConfigDialog( params, this );
	d->exec();
	delete d;
	break;
    }
    case KCHARTTYPE_LINE:
    case KCHARTTYPE_AREA:
    case KCHARTTYPE_HILOCLOSE:
    case KCHARTTYPE_COMBO_LINE_BAR:			/* aka: VOL[ume] */
    case KCHARTTYPE_COMBO_HLC_BAR:
    case KCHARTTYPE_COMBO_LINE_AREA:
    case KCHARTTYPE_COMBO_HLC_AREA:
    case KCHARTTYPE_3DHILOCLOSE:
    case KCHARTTYPE_3DCOMBO_LINE_BAR:
    case KCHARTTYPE_3DCOMBO_LINE_AREA:
    case KCHARTTYPE_3DCOMBO_HLC_BAR:
    case KCHARTTYPE_3DCOMBO_HLC_AREA:
    case KCHARTTYPE_3DAREA:
    case KCHARTTYPE_3DLINE:
    case KCHARTTYPE_3DPIE:
    case KCHARTTYPE_2DPIE:
        {
        //qDebug( "Sorry, not implemented: no config dialog for this chart type" );
        KChartConfigDialog* d = new KChartConfigDialog( params, this );
	d->exec();
	delete d;
        break;
        }
    default:
	qDebug( "Unknown chart type" );
    }
    repaint();
}


void KChartView::saveConfig() {
    qDebug("Save config...");
    ((KChartPart*)koDocument())->saveConfig( KGlobal::config() );
}

void KChartView::loadConfig() {
    qDebug("Load config...");
    KGlobal::config()->reparseConfiguration();
    ((KChartPart*)koDocument())->loadConfig( KGlobal::config() );
    //refresh chart when you load config
    repaint();
}

void KChartView::pieChart() {
KChartParameters* params = ((KChartPart*)koDocument())->params();
params->type=KCHARTTYPE_2DPIE;
repaint();
}

void KChartView::lineChart() {
KChartParameters* params = ((KChartPart*)koDocument())->params();
params->type=KCHARTTYPE_3DLINE;
repaint();
}

void KChartView::barsChart() {
KChartParameters* params = ((KChartPart*)koDocument())->params();
params->type=KCHARTTYPE_3DBAR;
repaint();
}

void KChartView::areasChart() {
KChartParameters* params = ((KChartPart*)koDocument())->params();
params->type=KCHARTTYPE_3DAREA;
repaint();
}

#include "kchart_view.moc"
