/* $Id$ */

#include "kchartWizardSelectChartTypePage.h"
#include "kchart_view.h"
//#include "kchartparams.h"

#include <qbuttongroup.h>
#include <qframe.h>
#include <qpushbutton.h>
#include <qlabel.h>

#include <klocale.h>
#include <kglobal.h>
#include <kiconloader.h>

void kchartWizardSelectChartTypePage::addButton(QString name,
					   QString icon_name,
					   int type) {  

  QFrame* buttonframe = new QFrame( this );
  buttonframe->setGeometry( 10+pos1*xstep, 
			    10+pos2*ystep, 
			    xsize, 
			    ysize );
  buttonframe->setLineWidth( 2 );
  QPushButton *pushbutton = new QPushButton( buttonframe );
  pushbutton->setToggleButton( true );
  pushbutton->resize( buttonframe->contentsRect().width(),
		      buttonframe->contentsRect().height() );
  _typeBG->insert( pushbutton, type );
  pushbutton->setPixmap( BarIcon( icon_name ) );

  QLabel* label = new QLabel( i18n( name ), this );
  label->setGeometry( 10+pos1*xstep, 
		      10+ysize+pos2*ystep, 
		      xsize, 
		      ylabelsize );
  label->setAlignment( AlignCenter );
  // next position
  if (pos1 == 4) {
      pos1=0;
      pos2++;
  } else pos1++;
}

kchartWizardSelectChartTypePage::kchartWizardSelectChartTypePage( QWidget* parent, KChartPart* chart ) :
  QWidget( parent ),
  _chart( chart )
{
  _typeBG = new QButtonGroup( this );
  _typeBG->setExclusive( true );
  _typeBG->hide();
  //  _typeBG->resize

  pos1=0;
  pos2=0;
  xsize = 94;
  ysize = 94;
  ylabelsize = 20;
  xstep = xsize + 10;
  ystep = ysize + ylabelsize + 10;

  addButton("Lines", "chart_lines", KCHARTTYPE_LINE);
  addButton("Area",  "chart_area", KCHARTTYPE_AREA);
  addButton("Bar", "chart_bar", KCHARTTYPE_BAR);

  addButton("Hi-lo-close", "chart_hiloclose", KCHARTTYPE_LINE);
  addButton("Combo line bar", "chart_combo_line_bar", 
	    KCHARTTYPE_COMBO_LINE_BAR);
  addButton("Combo HLC bar", "chart_combo_hlc_bar",
	    KCHARTTYPE_COMBO_HLC_BAR);

  addButton("Combo line area","chart_combo_line_area", 
	    KCHARTTYPE_COMBO_LINE_AREA);
  addButton("Combo hlc area","chart_combo_hlc_area", 
	    KCHARTTYPE_COMBO_HLC_AREA);
  addButton("3D Combo HiLo Close", "chart_3dhiloclose",
	    KCHARTTYPE_3DHILOCLOSE);

  addButton("3DCOMBO_LINE_BAR", "3DCOMBO_LINE_BAR",
	    KCHARTTYPE_3DCOMBO_LINE_BAR);
  addButton("3DCOMBO_LINE_AREA", "3DCOMBO_LINE_AREA",
	    KCHARTTYPE_3DCOMBO_LINE_AREA);
  addButton("3DCOMBO_HLC_BAR","3DCOMBO_HLC_BAR",
	    KCHARTTYPE_3DCOMBO_HLC_BAR);

  addButton("3DCOMBO_HLC_AREA","3DCOMBO_HLC_AREA",
	    KCHARTTYPE_3DCOMBO_HLC_AREA);
  addButton("3DBAR","3DBAR", KCHARTTYPE_3DBAR);
  addButton("3DAREA","3DAREA", KCHARTTYPE_3DAREA);

  addButton("3DLINE","3DLINE", KCHARTTYPE_3DLINE);
  addButton("3DPIE","3DPIE", KCHARTTYPE_3DPIE);
  addButton("2DPIE","2DPIE", KCHARTTYPE_2DPIE);

  QPushButton *current = ((QPushButton*)_typeBG->find( _chart->params()->type ));
  if (current != NULL) {
    current->setOn( true );
  }
  _type=_chart->params()->type ;
  connect( _typeBG, SIGNAL( clicked( int ) ),
		   this, SLOT( chartTypeSelected( int ) ) );

  //  parent->resize( 425, 256 );
  parent->resize(xstep*5+50, ystep*4 + 100);
}


void kchartWizardSelectChartTypePage::chartTypeSelected( int type )
{
  cerr << "Type selected: " << type << "\n";
   _type=(KChartType)type;
   // _chart->params()->type = (KChartType)type;

}

void kchartWizardSelectChartTypePage::apply()
{

 cerr << "Type selected: " <<(int)_type <<endl;
 _chart->params()->type = _type;
}

#include "kchartWizardSelectChartTypePage.moc"
