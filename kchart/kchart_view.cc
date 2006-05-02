/**
 *
 * Kalle Dalheimer <kalle@kde.org>
 */


#include <qfile.h>
#include <qpainter.h>
#include <qcursor.h>
#include <QMenu>
//Added by qt3to4:
#include <QMouseEvent>
#include <QPixmap>
#include <QPaintEvent>

#include <kaction.h>
#include <kglobal.h>
#include <klocale.h>
#include <kiconloader.h>
#include <kdebug.h>
#include <kprinter.h>
#include <kstandarddirs.h>
#include <ktempfile.h>
#include <dcopobject.h>
#include <kinstance.h>
#include <kxmlguifactory.h>
#include <kfiledialog.h>
#include <kmessagebox.h>

#include <KoTemplateCreateDia.h>

#include "kdchart/KDChart.h"
#include "kchart_view.h"
#include "kchart_factory.h"
#include "kchart_part.h"
#include "kchart_params.h"
#include "kchartWizard.h"
#include "kchartDataEditor.h"
#include "kchartConfigDialog.h"
#include "KChartViewIface.h"
#include "kchartPageLayout.h"
#include "kchartPrinterDlg.h"
#include "csvimportdialog.h"


using namespace std;


//#include "sheetdlg.h"

namespace KChart
{


KChartView::KChartView( KChartPart* part, QWidget* parent, const char* name )
    : KoView( part, parent, name )
{

    // No flicker
    setBackgroundMode( Qt::NoBackground );

    setInstance( KChartFactory::global() );
    if ( koDocument()->isReadWrite() )
        setXMLFile( "kchart.rc" );
    else
        setXMLFile( "kchart_readonly.rc" );
    m_dcop = 0;
    dcopObject(); // build it

    m_importData = new KAction( i18n( "Import Data..." ), 0,
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
                          this, SLOT( editData() ),
                          actionCollection(), "editdata");
    m_config = new KAction( i18n( "&Chart..." ), "configure", 0,

                            this, SLOT( slotConfig() ),
                            actionCollection(), "config" );

    // One KToggleAction per chart type
    m_chartbars = new KToggleAction( i18n("&Bar"), "chart_bar_3d", 0, this,
                                     SLOT( barsChart() ), actionCollection(),
                                     "barschart");
    QActionGroup *charttypes = new QActionGroup( this );
	charttypes->addAction(m_chartbars);

    m_chartline = new KToggleAction( i18n("&Line"), "chart_line", 0, this,
                                     SLOT( lineChart() ), actionCollection(),
                                     "linechart");
	charttypes->addAction(m_chartline);
	
    m_chartareas = new KToggleAction( i18n("&Area"), "chart_area", 0, this,
                                      SLOT( areasChart() ), actionCollection(),
                                      "areaschart");
	charttypes->addAction(m_chartareas);
	
    m_charthilo = new KToggleAction( i18n("&HiLo"), "chart_hilo", 0, this,
                                     SLOT( hiLoChart() ), actionCollection(),
                                     "hilochart");
	charttypes->addAction(m_charthilo);
	
    m_chartbw = new KToggleAction( i18n("Bo&x && Whiskers"), "chart_boxwhisker", 0, this,
                                     SLOT( bwChart() ), actionCollection(),
                                     "bwchart");
	charttypes->addAction(m_chartbw);
	
    m_chartpie = new KToggleAction( i18n("&Pie"), "chart_pie", 0, this,
                                    SLOT( pieChart() ), actionCollection(),
                                    "piechart");
    charttypes->addAction(m_chartpie);

    m_chartring = new KToggleAction( i18n("&Ring"), "chart_ring", 0, this,
                                     SLOT( ringChart() ), actionCollection(),
                                     "ringchart");
	charttypes->addAction(m_chartring);
    
	m_chartpolar = new KToggleAction( i18n("&Polar"), "chart_polar", 0, this,
                                     SLOT( polarChart() ), actionCollection(),
                                     "polarchart");
	charttypes->addAction(m_chartpolar);
    // Configuration KActions
    m_colorConfig = new KAction( i18n( "&Colors..." ), 0,
                            this, SLOT( slotConfigColor() ),
                            actionCollection(), "color_config" );

    m_fontConfig = new KAction( i18n( "&Font..." ), 0,
                            this, SLOT( slotConfigFont() ),
                            actionCollection(), "font_config" );

    m_backConfig = new KAction( i18n( "&Background..." ), 0,
                            this, SLOT( slotConfigBack() ),
                            actionCollection(), "back_config" );

    m_legendConfig = new KAction( i18n( "&Legend..." ), 0,
                            this, SLOT( slotConfigLegend() ),
                            actionCollection(), "legend_config" );

    m_subTypeChartConfig = new KAction( i18n( "Chart &Sub-type..." ), 0,
                            this, SLOT( slotConfigSubTypeChart() ),
                            actionCollection(), "legend_subtype" );

    m_dataFormatConfig = new KAction( i18n( "&Data Format..." ), 0,
                            this, SLOT( slotConfigDataFormat() ),
                            actionCollection(), "data_format" );

    m_headerFooterConfig = new KAction( i18n( "&Header && Footer..." ), 0,
                            this, SLOT( slotConfigHeaderFooterChart() ),
                            actionCollection(), "headerfooter_subtype" );

    m_pageLayoutConfig = new KAction( i18n( "Page Layout..." ), 0,
                            this, SLOT( slotConfigPageLayout() ),
                            actionCollection(), "page_layout" );

    // initialize the configuration
    //    loadConfig();

    // Disable some things if we can't change the data, e.g. because
    // we are inside another application that provides the data for us.
    if (!((KChartPart*)koDocument())->canChangeValue()) {
	m_edit->setEnabled(false);
	m_importData->setEnabled(false);
    }

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
    QPainter painter( this );

    // ### TODO: Scaling

    // Let the document do the drawing
    // This calls KChartPart::paintContent, basically.
    koDocument()->paintEverything( painter, rect(), false, this );
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

void KChartView::editData()
{
    kchartDataEditor   ed(this);
    KChartParams      *params = ((KChartPart*)koDocument())->params();
    KDChartTableData  *dat    = ((KChartPart*)koDocument())->data();

    kDebug(35001) << "***Before calling editor: cols =" << dat->cols()
		   << " , rows = "     << dat->rows()
		   << " , usedCols = " << dat->usedCols()
		   << "  usedRows = "  << dat->usedRows() << endl;

    ed.setData( params, dat );
    ed.setRowLabels(((KChartPart*)koDocument())->rowLabelTexts());
    ed.setColLabels(((KChartPart*)koDocument())->colLabelTexts());

    // Activate the Apply button in the editor.
    connect(&ed,  SIGNAL(applyClicked(kchartDataEditor *)),
	    this, SLOT(applyEdit(kchartDataEditor *)));

    // Execute the data editor.
    if ( ed.exec() != QDialog::Accepted ) {
        return;
    }
    if (!ed.modified())
    return;

    // Get the data and legend back.
    ed.getData(params, dat);
    ed.getRowLabels(((KChartPart*)koDocument())->rowLabelTexts());
    ed.getColLabels(((KChartPart*)koDocument())->colLabelTexts());
    ((KChartPart*)koDocument())->setModified(true);

    kDebug(35001) << "***After calling editor: cols =" << dat->cols()
		   << " , rows = "     << dat->rows()
		   << " , usedCols = " << dat->usedCols()
		   << "  usedRows = "  << dat->usedRows() << endl;
    update();
}


void KChartView::applyEdit(kchartDataEditor *ed)
{
    if (!ed->modified())
	return;

    ed->getData( ((KChartPart*)koDocument())->params(), 
		 ((KChartPart*)koDocument())->data() );
    ed->getRowLabels(((KChartPart*)koDocument())->rowLabelTexts());
    ed->getColLabels(((KChartPart*)koDocument())->colLabelTexts());

    ((KChartPart*)koDocument())->setModified(true);

    update();
}


void KChartView::wizard()
{
    kDebug(35001) << "Wizard called" << endl;
    KChartWizard *wiz = new KChartWizard((KChartPart*)koDocument(), this,
					 "KChart Wizard", true);
    kDebug(35001) << "Executed. Now, display it" << endl;
    if (wiz->exec()) {
	((KChartPart*)koDocument())->setModified(true);
        update();
        updateGuiTypeOfChart();
        kDebug(35001) << "Ok, executed..." << endl;
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
    KDChartTableData    *dat    = ((KChartPart*)koDocument())->data();

    KChartConfigDialog  *d      = new KChartConfigDialog( params, this, flags,
							  dat );

    connect( d, SIGNAL( dataChanged() ),
             this, SLOT( slotRepaint() ) );
    d->exec();
    delete d;
}


void KChartView::slotRepaint()
{
    ((KChartPart*)koDocument())->setModified(true);
    update();
}


void KChartView::saveConfig()
{
    kDebug(35001) << "Save config..." << endl;
    ((KChartPart*)koDocument())->saveConfig( KGlobal::config() );
}


void KChartView::loadConfig()
{
    kDebug(35001) << "Load config..." << endl;

    KGlobal::config()->reparseConfiguration();
    ((KChartPart*)koDocument())->loadConfig( KGlobal::config() );

    updateGuiTypeOfChart();
    //refresh chart when you load config
    update();
}


void KChartView::defaultConfig()
{
    ((KChartPart*)koDocument())->defaultConfig(  );
    updateGuiTypeOfChart();
    update();
}


void KChartView::pieChart()
{
    if ( m_chartpie->isChecked() ) {
	forceAxisParams(false);
	KChartParams  *params = ((KChartPart*)koDocument())->params();

	params->setChartType( KChartParams::Pie );
	params->setThreeDPies(params->threeDBars());
	params->setExplodeFactor( 0 );
	params->setExplode( true );

	updateButton();
	update();
	((KChartPart*)koDocument())->setModified(true);
    }
    else
        m_chartpie->setChecked( true ); // always one has to be checked !
}

void KChartView::forceAxisParams(bool lineMode) {
    KChartParams  *params = ((KChartPart*)koDocument())->params();
    KDChartAxisParams  axisParams;
    axisParams = params->axisParams( KDChartAxisParams::AxisPosLeft );
    if(params->chartType() == KChartParams::Line)
        m_logarithmicScale = axisParams.axisCalcMode();
    if(lineMode) {
        if(m_logarithmicScale)
            axisParams.setAxisCalcMode(KDChartAxisParams::AxisCalcLogarithmic);
    } else
        axisParams.setAxisCalcMode(KDChartAxisParams::AxisCalcLinear);
    params->setAxisParams( KDChartAxisParams::AxisPosLeft, axisParams );
}

void KChartView::lineChart()
{
    if ( m_chartline->isChecked() ) {
	forceAxisParams(true);
	KChartParams* params = ((KChartPart*)koDocument())->params();

	params->setChartType( KChartParams::Line );
	params->setLineChartSubType( KDChartParams::LineNormal );

	updateButton();
	update();
	((KChartPart*)koDocument())->setModified(true);
    }
    else
	m_chartline->setChecked( true ); // always one has to be checked !

}


void KChartView::barsChart()
{
    if ( m_chartbars->isChecked() ) {
	forceAxisParams(false);
	KChartParams* params = ((KChartPart*)koDocument())->params();

	params->setChartType( KChartParams::Bar );
	params->setBarChartSubType( KDChartParams::BarNormal );

	updateButton();
    params->setThreeDBars( params->threeDPies() );
	update();
	((KChartPart*)koDocument())->setModified(true);
    }
    else
	m_chartbars->setChecked( true ); // always one has to be checked !
}


void KChartView::areasChart()
{
    if ( m_chartareas->isChecked() ) {
	forceAxisParams(false);
	KChartParams* params = ((KChartPart*)koDocument())->params();

	params->setChartType( KChartParams::Area );
	params->setAreaChartSubType( KDChartParams::AreaNormal );

	updateButton();
	update();
	((KChartPart*)koDocument())->setModified(true);
    }
    else
	m_chartareas->setChecked( true ); // always one has to be checked !

}


void KChartView::hiLoChart()
{
    if ( m_charthilo->isChecked() ) {
	forceAxisParams(false);
	KChartParams* params = ((KChartPart*)koDocument())->params();

	params->setChartType( KChartParams::HiLo );
	params->setHiLoChartSubType( KDChartParams::HiLoNormal );

	updateButton();
	update();
	((KChartPart*)koDocument())->setModified(true);
    }
    else
	m_charthilo->setChecked( true ); // always one has to be checked !
}


void KChartView::ringChart()
{
    if ( m_chartring->isChecked() ) {
	forceAxisParams(false);
	KChartParams* params = ((KChartPart*)koDocument())->params();

	params->setChartType( KChartParams::Ring );

	updateButton();
	update();
	((KChartPart*)koDocument())->setModified(true);
    }
    else
	m_chartring->setChecked( true ); // always one has to be checked !

}


void KChartView::polarChart()
{
    if ( m_chartpolar->isChecked() ) {
	forceAxisParams(false);
        KDChartParams* params = ((KChartPart*)koDocument())->params();

        params->setChartType( KDChartParams::Polar );
        params->setPolarChartSubType( KDChartParams::PolarNormal );

        update();
	((KChartPart*)koDocument())->setModified(true);
    }
    else
        m_chartpolar->setChecked( true ); // always one has to be checked !
}


void KChartView::bwChart()
{
    if ( m_chartbw->isChecked() ) {
	forceAxisParams(false);
        KDChartParams* params = ((KChartPart*)koDocument())->params();

        params->setChartType( KDChartParams::BoxWhisker );
        params->setBWChartSubType( KDChartParams::BWNormal );

        update();
	((KChartPart*)koDocument())->setModified(true);
    }
    else
        m_chartbw->setChecked( true ); // always one has to be checked !
}


void KChartView::mousePressEvent ( QMouseEvent *e )
{
    if (!koDocument()->isReadWrite() || !factory())
        return;
    if ( e->button() == Qt::RightButton )
        ((QMenu*)factory()->container("action_popup",this))->popup(QCursor::pos());
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

void KChartView::slotConfigDataFormat()
{
    config(KChartConfigDialog::KC_DATAFORMAT);
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

    bool state=(params->chartType()==KChartParams::Bar ||
                params->chartType()==KChartParams::Area ||
                params->chartType()==KChartParams::Line ||
                params->chartType()==KChartParams::HiLo ||
                params->chartType()==KChartParams::Polar);
    m_subTypeChartConfig->setEnabled(state);
}


void KChartView::slotConfigPageLayout()
{
    KChartParams      *params = ((KChartPart*)koDocument())->params();
    KChartPageLayout  *dialog = new KChartPageLayout(params, this,
						     "Page Layout");

    connect( dialog, SIGNAL( dataChanged() ),
             this,   SLOT( slotRepaint() ) );

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

    int  height;
    int  width;
    if ( !printer.previewOnly() ) {
	int const scalex = printer.option("kde-kchart-printsizex").toInt();
	int const scaley = printer.option("kde-kchart-printsizey").toInt();

	width  = printer.width()  * scalex / 100;
	height = printer.height() * scaley / 100;
    }
    else {
	// Fill the whole page.
	width  = printer.width();
	height = printer.height();
    }

    QRect  rect(0, 0, width, height);
    KDChart::print(&painter,
		   ((KChartPart*)koDocument())->params(),
		   ((KChartPart*)koDocument())->data(),
		   0, 		// regions
		   &rect);

    painter.end();
}


// Import data from a Comma Separated Values file.
//

void KChartView::importData()
{
    // Get the name of the file to open.
    QString filename = KFileDialog::getOpenFileName(QString::null,// startDir
						    QString::null,// filter
						    0,
						    i18n("Import Data"));
    kDebug(35001) << "Filename = <" << filename << ">" << endl;
    if (filename.isEmpty())
      return;

    // Check to see if we can read the file.
    QFile  inFile(filename);
    if (!inFile.open(QIODevice::ReadOnly)) {
	KMessageBox::sorry( 0, i18n("The file %1 could not be read.")
			    .arg(filename) );
	inFile.close();
	return;
    }

    // Let the CSV dialog structure the data in the file.
    QByteArray  inData( inFile.readAll() );
    inFile.close();
    CSVImportDialog  *dialog = new CSVImportDialog(0L, inData);

    if ( !dialog->exec() ) {
	kDebug(35001) << "Cancel was pressed" << endl;
	return;
    }

    kDebug(35001) << "OK was pressed" << endl;

    uint  rows = dialog->rows();
    uint  cols = dialog->cols();

    kDebug(35001) << "Rows: " << rows << "  Cols: " << cols << endl;

    bool  hasRowHeaders = ( rows > 1 && dialog->firstRowContainHeaders() );
    bool  hasColHeaders = ( cols > 1 && dialog->firstColContainHeaders() );

    KDChartTableData  data( rows, cols );
    data.setUsedRows( rows );
    data.setUsedCols( cols );
    for (uint row = 0; row < rows; row++) {
	for (uint col = 0; col < cols; col++) {
	    bool     ok;
	    QString  tmp;
	    double   val;

	    // Get the text and convert to double unless in the headers.
	    tmp = dialog->text( row, col );
	    if ( ( row == 0 && hasRowHeaders )
		 || ( col == 0 && hasColHeaders ) ) {
		kDebug(35001) << "Setting header (" << row << "," << col
			       << ") to value " << tmp << endl;
		data.setCell( row, col, tmp );
	    }
	    else {
		val = tmp.toDouble(&ok);
		if (!ok)
		    val = 0.0;

		kDebug(35001) << "Setting (" << row << "," << col
			       << ") to value " << val << endl;

		// and do the actual setting.
		data.setCell( row, col, val );
	    }
	}
    }

    ((KChartPart*)koDocument())->doSetData( data,
					    hasRowHeaders, hasColHeaders );
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
