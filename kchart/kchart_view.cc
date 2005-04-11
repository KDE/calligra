/**
 *
 * Kalle Dalheimer <kalle@kde.org>
 */


#include "kdchart/KDChart.h"
#include "kchart_view.h"
#include "kchart_factory.h"
#include "kchart_part.h"
#include "kchartWizard.h"
#include "kchartDataEditor.h"
#include "kchartConfigDialog.h"
#include "KChartViewIface.h"
#include "kchartPageLayout.h"
#include "kchart_params.h"
#include "kchartPrinterDlg.h"
#include "csvimportdialog.h"

#include <qfile.h>
#include <qpainter.h>
#include <qcursor.h>
#include <qpopupmenu.h>
#include <kaction.h>
#include <kglobal.h>
#include <klocale.h>
#include <kiconloader.h>
#include <kdebug.h>
#include <kprinter.h>
#include <kstandarddirs.h>
#include <ktempfile.h>
#include <dcopobject.h>
#include <kxmlguifactory.h>
#include <qpaintdevicemetrics.h>
#include <qcstring.h> // For QByteArray
#include <kfiledialog.h>
#include <kmessagebox.h>

#include <koTemplateCreateDia.h>

using namespace std;


//#include "sheetdlg.h"

namespace KChart
{


KChartView::KChartView( KChartPart* part, QWidget* parent, const char* name )
    : KoView( part, parent, name )
{
    setInstance( KChartFactory::global() );
    if ( koDocument()->isReadWrite() )
        setXMLFile( "kchart.rc" );
    else
        setXMLFile( "kchart_readonly.rc" );
    m_dcop = 0;
    dcopObject(); // build it

    new KAction( i18n( "Import Data..." ), 0,
		 this, SLOT( importData() ),
		 actionCollection(), "import_data" );
#if 0
    KAction * actionExtraCreateTemplate	=
#endif
	new KAction( i18n( "&Create Template From Document..." ), 0,
		       this, SLOT( extraCreateTemplate() ),
		       actionCollection(), "extra_template" );

    m_wizard = new KAction( i18n("Customize with &Wizard..."),
                            "wizard", 0,
                            this, SLOT( wizard() ),
                            actionCollection(), "wizard");
    m_edit = new KAction( i18n("Edit &Data..."), "edit", 0,
                          this, SLOT( edit() ),
                          actionCollection(), "edit");
    m_config = new KAction( i18n( "&Configure Chart..." ), "configure", 0,
                            this, SLOT( slotConfig() ),
                            actionCollection(), "config" );

    // One KToggleAction per chart type
    m_chartbars = new KToggleAction( i18n("&Bar"), "chart_bar_3d", 0, this,
                                     SLOT( barsChart() ), actionCollection(),
                                     "barschart");
    m_chartbars->setExclusiveGroup( "charttypes" );
    m_chartbars->setChecked( true );

    m_chartline = new KToggleAction( i18n("&Line"), "chart_line", 0, this,
                                     SLOT( lineChart() ), actionCollection(),
                                     "linechart");
    m_chartline->setExclusiveGroup( "charttypes" );

    m_chartareas = new KToggleAction( i18n("&Area"), "chart_area", 0, this,
                                      SLOT( areasChart() ), actionCollection(),
                                      "areaschart");
    m_chartareas->setExclusiveGroup( "charttypes" );

    m_charthilo = new KToggleAction( i18n("&HiLo"), "chart_hilo", 0, this,
                                     SLOT( hiLoChart() ), actionCollection(),
                                     "hilochart");
    m_charthilo->setExclusiveGroup( "charttypes" );

    m_chartbw = new KToggleAction( i18n("Bo&x && Whiskers"), "chart_boxwhisker", 0, this,
                                     SLOT( bwChart() ), actionCollection(),
                                     "bwchart");
    m_chartbw->setExclusiveGroup( "charttypes" );

    m_chartpie = new KToggleAction( i18n("&Pie"), "chart_pie", 0, this,
                                    SLOT( pieChart() ), actionCollection(),
                                    "piechart");
    m_chartpie->setExclusiveGroup( "charttypes" );

    m_chartring = new KToggleAction( i18n("&Ring"), "chart_ring", 0, this,
                                     SLOT( ringChart() ), actionCollection(),
                                     "ringchart");
    m_chartring->setExclusiveGroup( "charttypes" );

    m_chartpolar = new KToggleAction( i18n("&Polar"), "chart_polar", 0, this,
                                     SLOT( polarChart() ), actionCollection(),
                                     "polarchart");
    m_chartpolar->setExclusiveGroup( "charttypes" );

    // Configuration KActions
    m_colorConfig = new KAction( i18n( "&Configure Colors..." ), 0,
                            this, SLOT( slotConfigColor() ),
                            actionCollection(), "color_config" );

    m_fontConfig = new KAction( i18n( "&Configure Font..." ), 0,
                            this, SLOT( slotConfigFont() ),
                            actionCollection(), "font_config" );

    m_backConfig = new KAction( i18n( "&Configure Background..." ), 0,
                            this, SLOT( slotConfigBack() ),
                            actionCollection(), "back_config" );

    m_legendConfig = new KAction( i18n( "&Configure Legend..." ), 0,
                            this, SLOT( slotConfigLegend() ),
                            actionCollection(), "legend_config" );
    m_subTypeChartConfig= new KAction( i18n( "&Configure Sub Type Chart..." ), 0,
                            this, SLOT( slotConfigSubTypeChart() ),
                            actionCollection(), "legend_subtype" );
    m_headerFooterConfig= new KAction( i18n( "&Configure Header Footer..." ), 0,
                            this, SLOT( slotConfigHeaderFooterChart() ),
                            actionCollection(), "headerfooter_subtype" );

    m_pageLayoutConfig=new KAction( i18n( "Page Layout..." ), 0,
                            this, SLOT( slotConfigPageLayout() ),
                            actionCollection(), "page_layout" );

    // initialize the configuration
    //    loadConfig();

    // Make sure there is always some test data.
    m_edit->setEnabled(((KChartPart*)koDocument())->canChangeValue());
#if 0
    createTempData();
#else
    //((KChartPart*)koDocument())->initTestChart();
#endif

    updateGuiTypeOfChart();
}


KChartView::~KChartView()
{
    delete m_dcop;
}


DCOPObject* KChartView::dcopObject()
{
    if ( !m_dcop )
	m_dcop = new KChartViewIface( this );

    return m_dcop;
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


// Edit the data to the chart.
// This opens a spreadsheet like editor with the data in it.
//

void KChartView::edit()
{
    kchartDataEditor  ed(this);
    //KChartParams     *params=((KChartPart*)koDocument())->params();

    KoChart::Data    *dat = (( (KChartPart*)koDocument())->data());

    kdDebug(35001) << "***Before calling editor: cols =" << dat->cols()
		   << " , rows = "     << dat->rows()
		   << " , usedCols = " << dat->usedCols()
		   << "  usedRows = "  << dat->usedRows() << endl;

    ed.setData(dat);
    ed.setRowLabels(((KChartPart*)koDocument())->rowLabelTexts());
    ed.setColLabels(((KChartPart*)koDocument())->colLabelTexts());

    connect(&ed,  SIGNAL(applyClicked(kchartDataEditor *)),
	    this, SLOT(applyEdit(kchartDataEditor *)));

    // Execute the data editor.
    if ( ed.exec() != QDialog::Accepted ) {
        return;
    }
    if (!ed.modified())
    return;

    // Get the data and legend back.
    ed.getData(dat);
    ed.getRowLabels(((KChartPart*)koDocument())->rowLabelTexts());
    ed.getColLabels(((KChartPart*)koDocument())->colLabelTexts());
    ((KChartPart*)koDocument())->setModified(true);

    kdDebug(35001) << "***After calling editor: cols =" << dat->cols()
		   << " , rows = "     << dat->rows()
		   << " , usedCols = " << dat->usedCols()
		   << "  usedRows = "  << dat->usedRows() << endl;
    repaint();
}


void KChartView::applyEdit(kchartDataEditor *ed)
{
    if (!ed->modified())
	return;

    ed->getData(( (KChartPart*)koDocument())->data());
    ed->getRowLabels(((KChartPart*)koDocument())->rowLabelTexts());
    ed->getColLabels(((KChartPart*)koDocument())->colLabelTexts());
    ((KChartPart*)koDocument())->setModified(true);

    repaint();

}


void KChartView::wizard()
{
    kdDebug(35001) << "Wizard called" << endl;
    KChartWizard *wiz =
	new KChartWizard((KChartPart*)koDocument(), this, "KChart Wizard", true);
    kdDebug(35001) << "Executed. Now, display it" << endl;
    if (wiz->exec()) {
	((KChartPart*)koDocument())->setModified(true);
        repaint();
        updateGuiTypeOfChart();
        kdDebug(35001) << "Ok, executed..." << endl;
    }
}


void KChartView::updateGuiTypeOfChart()
{
    KDChartParams* params = ((KChartPart*)koDocument())->params();

    switch(params->chartType()) {
    case KDChartParams::Bar:
	m_chartbars->setChecked(true);
	break;
    case KDChartParams::Line:
	m_chartline->setChecked(true);
	break;
    case KDChartParams::Area:
	m_chartareas->setChecked(true);
	break;
    case KDChartParams::Pie:
	m_chartpie->setChecked(true);
	break;
    case KDChartParams::HiLo:
	m_charthilo->setChecked(true);
	break;
    case KDChartParams::Ring:
	m_chartring->setChecked(true);
	break;
    case KDChartParams::Polar:
        m_chartpolar->setChecked(true);
        break;
    case KDChartParams::BoxWhisker:
        m_chartbw->setChecked( true );
        break;
    default:
	//todo
	break;
    }

    // Disable subtype configuration button if appropriate.
    updateButton();
}


void KChartView::slotConfig()
{
    config(KChartConfigDialog::KC_ALL);
}


void KChartView::config(int flags)
{
    // open a config dialog depending on the chart type
    KChartParams        *params = ((KChartPart*)koDocument())->params();
    KoChart::Data       *dat    = (( (KChartPart*)koDocument())->data());
    KChartAuxiliary     *aux    = ((KChartPart*)koDocument())->auxdata();
    KChartConfigDialog  *d      = new KChartConfigDialog( params, this, flags,
							  dat, aux );

    connect( d, SIGNAL( dataChanged() ),
             this, SLOT( slotRepaint() ) );
    d->exec();
    delete d;
}


void KChartView::slotRepaint()
{
    ((KChartPart*)koDocument())->setModified(true);
    repaint();
}


void KChartView::saveConfig()
{
    kdDebug(35001) << "Save config..." << endl;
    ((KChartPart*)koDocument())->saveConfig( KGlobal::config() );
}


void KChartView::loadConfig()
{
    kdDebug(35001) << "Load config..." << endl;

    KGlobal::config()->reparseConfiguration();
    ((KChartPart*)koDocument())->loadConfig( KGlobal::config() );

    updateGuiTypeOfChart();
    //refresh chart when you load config
    repaint();
}


void KChartView::defaultConfig()
{
    ((KChartPart*)koDocument())->defaultConfig(  );
    updateGuiTypeOfChart();
    repaint();
}


void KChartView::pieChart()
{
    if ( m_chartpie->isChecked() ) {
	KChartParams  *params = ((KChartPart*)koDocument())->params();

	params->setChartType( KDChartParams::Pie );
    params->setThreeDPies(params->threeDBars());
	params->setExplodeFactor( 0 );
	params->setExplode( true );

	updateButton();
	repaint();
	((KChartPart*)koDocument())->setModified(true);
    }
    else
        m_chartpie->setChecked( true ); // always one has to be checked !
}

void KChartView::lineChart()
{
    if ( m_chartline->isChecked() ) {
	KChartParams* params = ((KChartPart*)koDocument())->params();

	params->setChartType( KDChartParams::Line );
	params->setLineChartSubType( KDChartParams::LineNormal );

	updateButton();
	repaint();
	((KChartPart*)koDocument())->setModified(true);
    }
    else
	m_chartline->setChecked( true ); // always one has to be checked !

}


void KChartView::barsChart()
{
    if ( m_chartbars->isChecked() ) {
	KChartParams* params = ((KChartPart*)koDocument())->params();

	params->setChartType( KDChartParams::Bar );
	params->setBarChartSubType( KDChartParams::BarNormal );

	updateButton();
    params->setThreeDBars( params->threeDPies() );
	repaint();
	((KChartPart*)koDocument())->setModified(true);
    }
    else
	m_chartbars->setChecked( true ); // always one has to be checked !
}


void KChartView::areasChart()
{
    if ( m_chartareas->isChecked() ) {
	KChartParams* params = ((KChartPart*)koDocument())->params();

	params->setChartType( KDChartParams::Area );
	params->setAreaChartSubType( KDChartParams::AreaNormal );

	updateButton();
	repaint();
	((KChartPart*)koDocument())->setModified(true);
    }
    else
	m_chartareas->setChecked( true ); // always one has to be checked !

}


void KChartView::hiLoChart()
{
    if ( m_charthilo->isChecked() ) {
	KChartParams* params = ((KChartPart*)koDocument())->params();

	params->setChartType( KDChartParams::HiLo );
	params->setHiLoChartSubType( KDChartParams::HiLoNormal );

	updateButton();
	repaint();
	((KChartPart*)koDocument())->setModified(true);
    }
    else
	m_charthilo->setChecked( true ); // always one has to be checked !
}


void KChartView::ringChart()
{
    if ( m_chartring->isChecked() ) {
	KChartParams* params = ((KChartPart*)koDocument())->params();

	params->setChartType( KDChartParams::Ring );

	updateButton();
	repaint();
	((KChartPart*)koDocument())->setModified(true);
    }
    else
	m_chartring->setChecked( true ); // always one has to be checked !

}


void KChartView::polarChart()
{
    if ( m_chartpolar->isChecked() ) {
        KDChartParams* params = ((KChartPart*)koDocument())->params();

        params->setChartType( KDChartParams::Polar );
        params->setPolarChartSubType( KDChartParams::PolarNormal );

        repaint();
	((KChartPart*)koDocument())->setModified(true);
    }
    else
        m_chartpolar->setChecked( true ); // always one has to be checked !
}


void KChartView::bwChart()
{
    if ( m_chartbw->isChecked() ) {
        KDChartParams* params = ((KChartPart*)koDocument())->params();

        params->setChartType( KDChartParams::BoxWhisker );
        params->setBWChartSubType( KDChartParams::BWNormal );

        repaint();
	((KChartPart*)koDocument())->setModified(true);
    }
    else
        m_chartbw->setChecked( true ); // always one has to be checked !
}


void KChartView::mousePressEvent ( QMouseEvent *e )
{
    if (!koDocument()->isReadWrite() || !factory())
        return;
    if ( e->button() == RightButton )
        ((QPopupMenu*)factory()->container("action_popup",this))->popup(QCursor::pos());
}


void KChartView::slotConfigColor()
{
    config(KChartConfigDialog::KC_COLORS);
}


void KChartView::slotConfigFont()
{
    config(KChartConfigDialog::KC_FONT);
}


void KChartView::slotConfigBack()
{
    config(KChartConfigDialog::KC_BACK);
}


void KChartView::slotConfigLegend()
{
   config(KChartConfigDialog::KC_LEGEND);
}


void KChartView::slotConfigSubTypeChart()
{
    config(KChartConfigDialog::KC_SUBTYPE);
}


void KChartView::slotConfigHeaderFooterChart()
{
    config(KChartConfigDialog::KC_HEADERFOOTER);
}


// FIXME: Rename into something suitable.
void KChartView::updateButton()
{
    // Disable sub chart config item.
    KChartParams* params = ((KChartPart*)koDocument())->params();

    bool state=(params->chartType()==KDChartParams::Bar ||
                params->chartType()==KDChartParams::Area ||
                params->chartType()==KDChartParams::Line ||
                params->chartType()==KDChartParams::HiLo ||
                params->chartType()==KDChartParams::Polar);
    m_subTypeChartConfig->setEnabled(state);
}


void KChartView::slotConfigPageLayout()
{
    KChartParams* params = ((KChartPart*)koDocument())->params();
    KChartPageLayout *dialog=new KChartPageLayout(params,this,"Page Layout");

    connect( dialog, SIGNAL( dataChanged() ),
             this, SLOT( slotRepaint() ) );

    dialog->exec();
    delete dialog;
}

void KChartView::setupPrinter( KPrinter &printer )
{
  if ( !printer.previewOnly() )
    printer.addDialogPage( new KChartPrinterDlg( 0, "KChart page" ) );
}

void KChartView::print(KPrinter &printer)
{
  printer.setFullPage( false );
  QPainter painter;
  painter.begin(&printer);
  QPaintDeviceMetrics pdm( &printer );
  int height, width;
  if ( !printer.previewOnly() )
  {
    	int const scalex = printer.option("kde-kchart-printsizex").toInt();
	int const scaley = printer.option("kde-kchart-printsizey").toInt();
	// FIXME: Losing precision here.  First multiply, then divide.
	width = (double)pdm.width()/100*scalex;
	height = (double)pdm.height()/100*scaley;
  }
  else
  { //fill the whole page
	width = pdm.width();
	height = pdm.height();
  }

  KDChart::print(&painter,((KChartPart*)koDocument())->params(),((KChartPart*)koDocument())->data(),0, new QRect(0,0, width, height));
  painter.end();
}

void KChartView::importData()
{
    // Get the name of the file to open.
    QString filename = KFileDialog::getOpenFileName(QString::null,// startDir
						    QString::null,// filter
						    0,
						    i18n("Import Data"));
    kdDebug(35001) << "Filename = <" << filename << ">" << endl;
    if (filename == "")
      return;

    // Check to see if we can read the file.
    QFile  inFile(filename);
    if (!inFile.open(IO_ReadOnly)) {
	KMessageBox::sorry( 0, i18n("The file %1 could not be read.")
			    .arg(filename) );
	inFile.close();
	return;
    }

    QByteArray  inData( inFile.readAll() );
    inFile.close();
    CSVImportDialog  *dialog = new CSVImportDialog(0L, inData);

    if ( dialog->exec() ) {
	kdDebug(35001) << "OK was pressed" << endl;
    }
    else {
	kdDebug(35001) << "Cancel was pressed" << endl;
    }
}


void KChartView::extraCreateTemplate()
{
    int width = 60;
    int height = 60;
    QPixmap pix = koDocument()->generatePreview(QSize(width, height));

    KTempFile tempFile( QString::null, ".chrt" );
    tempFile.setAutoDelete(true);

    koDocument()->saveNativeFormat( tempFile.name() );

    KoTemplateCreateDia::createTemplate( "kchart_template", KChartFactory::global(),
                                         tempFile.name(), pix, this );

    KChartFactory::global()->dirs()->addResourceType("kchart_template",
                                                    KStandardDirs::kde_default( "data" ) +
                                                    "kchart/templates/");
}


}  //KChart namespace

#include "kchart_view.moc"
