/**
 * $Id$
 *
 * Kalle Dalheimer <kalle@kde.org>
 */

#include "kchart_view.h"
#include "kchart_factory.h"
#include "kchart_part.h"
#include "kchartWizard.h"
#include "kchartDataEditor.h"
#include "kchartBarConfigDialog.h"

#include "kdchart/KDChartParams.h"

#include <qpainter.h>
#include <kaction.h>
#include <kglobal.h>
#include <klocale.h>
#include <kiconloader.h>
#include <kdebug.h>

using namespace std;

//#include "sheetdlg.h"

KChartView::KChartView( KChartPart* part, QWidget* parent, const char* name )
    : KoView( part, parent, name )
{
    setInstance( KChartFactory::global() );
    setXMLFile( "kchart.rc" );

    m_wizard = new KAction( i18n("Customize with &Wizard"),
                            "wizard", 0,
                            this, SLOT( wizard() ),
                            actionCollection(), "wizard");
    m_edit = new KAction( i18n("&Edit Data"), "edit", 0,
                          this, SLOT( edit() ),
                          actionCollection(), "edit");
    m_config = new KAction( i18n( "&Config" ), "configure", 0,
                            this, SLOT( config() ),
                            actionCollection(), "config" );
    m_loadconfig = new KAction( i18n("&Load Config"),
                                "fileopen", 0, this,
                                SLOT( loadConfig() ),
                                actionCollection(), "loadconfig");
    m_saveconfig = new KAction( i18n("&Save Config"),
                                "filesave",	0, this,
                                SLOT( saveConfig() ),
                                actionCollection(), "saveconfig");
    m_defaultconfig = new KAction( i18n("&Default Config"),
                                   "defaultconfig", 0, this,
                                   SLOT( defaultConfig() ),
                                   actionCollection(), "defaultconfig");

    m_chartpie = new KToggleAction( i18n("&Pie"), "cakes", 0, this,
                                    SLOT( pieChart() ), actionCollection(),
                                    "piechart");
    m_chartpie->setExclusiveGroup( "charttypes" );
    m_chartline = new KToggleAction( i18n("&Line"), "lines3d", 0, this,
                                     SLOT( lineChart() ), actionCollection(),
                                     "linechart");
    m_chartline->setExclusiveGroup( "charttypes" );
    m_chartareas = new KToggleAction( i18n("&Areas"), "areas3d", 0, this,
                                      SLOT( areasChart() ), actionCollection(),
                                      "areaschart");
    m_chartareas->setExclusiveGroup( "charttypes" );
    m_chartbars = new KToggleAction( i18n("&Bars"), "bars3d", 0, this,
                                     SLOT( barsChart() ), actionCollection(),
                                     "barschart");
    m_chartbars->setExclusiveGroup( "charttypes" );
    m_chartbars->setChecked( true );
    m_charthilo = new KToggleAction( i18n("&HiLo"), "hilo", 0, this,
                                     SLOT( hiLoChart() ), actionCollection(),
                                     "hilochart");
    m_charthilo->setExclusiveGroup( "charttypes" );
    m_chartring = new KToggleAction( i18n("&Ring"), "ring", 0, this,
                                     SLOT( ringChart() ), actionCollection(),
                                     "ringchart");
    m_chartring->setExclusiveGroup( "charttypes" );

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

void KChartView::updateReadWrite( bool /*readwrite*/ )
{
#ifdef __GNUC__
#warning TODO
#endif
}

void KChartView::createTempData()
{
    int row, col;
    int nbrow,nbcol;

    KoChart::Data *dat = ((KChartPart*)koDocument())->data();

    // initialize some data, if there is none
    nbrow=4;
    nbcol=4;
    if (dat->rows() == 0)
        {
            kdDebug(35001) << "Initialize with some data!!!" << endl;
            dat->expand(4,4);
            dat->setUsedCols( 4 );
            dat->setUsedRows( 4 );
            for (row = 0;row < nbrow;row++)
                for (col = 0;col < nbcol;col++)
                    {
			//	  _widget->fillCell(row,col,row+col);
                        KoChart::Value t( (double)row+col );
			kdDebug(35001) << "Set cell for " << row << "," << col << endl;
			dat->setCell(row,col,t);
                    }
            //      _dlg->exec();
        }
}


void KChartView::edit()
{
    kchartDataEditor ed;
    KDChartParams* params=((KChartPart*)koDocument())->params();

    KoChart::Data *dat = (( (KChartPart*)koDocument())->data());
    qDebug( "***Before calling editor: cols = %d, rows = %d, usedCols = %d, usedRows = %d", dat->cols(), dat->rows(), dat->usedCols(), dat->usedRows() );
    ed.setData(dat);
    if( ed.exec() != QDialog::Accepted ) {
        return;
    }
    ed.getData(dat);
    qDebug( "***Before calling editor: cols = %d, rows = %d, usedCols = %d, usedRows = %d", dat->cols(), dat->rows(), dat->usedCols(), dat->usedRows() );
    repaint();
}

void KChartView::wizard()
{
    kdDebug(35001) << "Wizard called" << endl;
    KChartWizard *wiz =
	new KChartWizard((KChartPart*)koDocument(), this, "KChart Wizard", true);
    kdDebug(35001) << "Executed. Now, display it" << endl;
    wiz->exec();
    repaint();
    kdDebug(35001) << "Ok, executed..." << endl;
}


void KChartView::config()
{
    // open a config dialog depending on the chart type
    KDChartParams* params = ((KChartPart*)koDocument())->params();

    KChartConfigDialog* d = new KChartConfigDialog( params, this );
    connect( d, SIGNAL( dataChanged() ),
             this, SLOT( slotRepaint() ) );
    d->exec();
    delete d;
}


void KChartView::slotRepaint()
{
    repaint();
}


void KChartView::saveConfig() {
    kdDebug(35001) << "Save config..." << endl;
    ((KChartPart*)koDocument())->saveConfig( KGlobal::config() );
}

void KChartView::loadConfig() {
    kdDebug(35001) << "Load config..." << endl;
    KGlobal::config()->reparseConfiguration();
    ((KChartPart*)koDocument())->loadConfig( KGlobal::config() );
    //refresh chart when you load config
    repaint();
}

void KChartView::defaultConfig() {
    ((KChartPart*)koDocument())->defaultConfig(  );
    repaint();
}


void KChartView::pieChart()
{
    KDChartParams* params = ((KChartPart*)koDocument())->params();
    params->setChartType( KDChartParams::Pie );
    params->setThreeDPies( true );
    repaint();
}

void KChartView::lineChart()
{
    KDChartParams* params = ((KChartPart*)koDocument())->params();
    params->setChartType( KDChartParams::Line );
    params->setLineChartSubType( KDChartParams::LineNormal );
    repaint();
}

void KChartView::barsChart()
{
    KDChartParams* params = ((KChartPart*)koDocument())->params();
    params->setChartType( KDChartParams::Bar );
    params->setBarChartSubType( KDChartParams::BarNormal );
    params->setThreeDBars( true );
    repaint();
}

void KChartView::areasChart()
{
    KDChartParams* params = ((KChartPart*)koDocument())->params();
    params->setChartType( KDChartParams::Area );
    params->setAreaChartSubType( KDChartParams::AreaNormal );
    repaint();
}


void KChartView::hiLoChart()
{
    KDChartParams* params = ((KChartPart*)koDocument())->params();
    params->setChartType( KDChartParams::HiLo );
    params->setHiLoChartSubType( KDChartParams::HiLoNormal );
    repaint();
}


void KChartView::ringChart()
{
    KDChartParams* params = ((KChartPart*)koDocument())->params();
    params->setChartType( KDChartParams::Ring );
    repaint();
}

#include "kchart_view.moc"
