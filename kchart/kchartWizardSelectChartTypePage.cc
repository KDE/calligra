/* $Id$ */

#include "kchartWizardSelectChartTypePage.h"
#include "kchart_view.h"
#include "kchart_factory.h"

#include <qbuttongroup.h>
#include <qframe.h>
#include <qpushbutton.h>
#include <qlabel.h>

#include <klocale.h>
#include <kglobal.h>
#include <kiconloader.h>

void KChartWizardSelectChartTypePage::addButton(QString name,
                                                QString icon_name,
                                                int type)
{
    qDebug( "Sorry, not implemented: KChartWizardSelectChartTypePage::addButton()" );
#warning Put back in
#ifdef K
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
    pushbutton->setPixmap( BarIcon( icon_name,KIcon::SizeMedium, KIcon::DefaultState,KChartFactory::global() ) );
    QLabel* label = new QLabel(name, this );
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
#endif
}


KChartWizardSelectChartTypePage::KChartWizardSelectChartTypePage( QWidget* parent,
                                                                  KChartPart* chart ) :
    QWidget( parent ),
_chart( chart )
{
    qDebug( "Sorry, not implemented: KChartWizardSelectChartTypePage::KChartWizardSelectChartTypePage()" );
#warning Put back in
#ifdef K

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

    addButton(i18n("Lines"), "chart_lines", KCHARTTYPE_LINE);
    addButton(i18n("Area"),  "chart_area", KCHARTTYPE_AREA);
    addButton(i18n("Bar"), "chart_bar", KCHARTTYPE_BAR);

    addButton(i18n("Hi-Lo-Close"), "chart_hiloclose", KCHARTTYPE_HILOCLOSE);
    addButton(i18n("Combo line bar"), "chart_combo_line_bar",
              KCHARTTYPE_COMBO_LINE_BAR);
    addButton(i18n("Combo HLC bar"), "chart_combo_hlc_bar",
              KCHARTTYPE_COMBO_HLC_BAR);

    addButton(i18n("Combo line area"),"chart_combo_line_area",
              KCHARTTYPE_COMBO_LINE_AREA);
    addButton(i18n("Combo HLC area"),"chart_combo_hlc_area",
              KCHARTTYPE_COMBO_HLC_AREA);
    addButton(i18n("3D Combo HiLo Close"), "chart_3dhiloclose",
              KCHARTTYPE_3DHILOCLOSE);

    addButton(i18n("3DCOMBO_LINE_BAR"), "3DCOMBO_LINE_BAR",
              KCHARTTYPE_3DCOMBO_LINE_BAR);
    addButton(i18n("3DCOMBO_LINE_AREA"), "3DCOMBO_LINE_AREA",
              KCHARTTYPE_3DCOMBO_LINE_AREA);
    addButton(i18n("3DCOMBO_HLC_BAR"),"3DCOMBO_HLC_BAR",
              KCHARTTYPE_3DCOMBO_HLC_BAR);

    addButton(i18n("3DCOMBO_HLC_AREA"),"3DCOMBO_HLC_AREA",
              KCHARTTYPE_3DCOMBO_HLC_AREA);
    addButton(i18n("3DBAR"),"3DBAR", KCHARTTYPE_3DBAR);
    addButton(i18n("3DAREA"),"3DAREA", KCHARTTYPE_3DAREA);

    addButton(i18n("3DLINE"),"3DLINE", KCHARTTYPE_3DLINE);
    addButton(i18n("3DPIE"),"chart_pie3D", KCHARTTYPE_3DPIE);
    addButton(i18n("2DPIE"),"chart_pie2D", KCHARTTYPE_2DPIE);

    QPushButton *current = ((QPushButton*)_typeBG->find( _chart->params()->type ));
    if (current != NULL) {
        current->setOn( true );
    }
    _type=_chart->params()->type ;
    connect( _typeBG, SIGNAL( clicked( int ) ),
             this, SLOT( chartTypeSelected( int ) ) );

    //  parent->resize( 425, 256 );
    parent->resize(xstep*5+50, ystep*4 + 100);
#endif
}


void KChartWizardSelectChartTypePage::chartTypeSelected( int type )
{
    qDebug( "Sorry, not implemented: KChartWizardSelectChartTypePage::chartTypeSelected()" );
#warning Put back in
#ifdef K
    _type=(KChartType)type;
    emit chartChange(type);
#endif
}

void KChartWizardSelectChartTypePage::apply()
{
    qDebug( "Sorry, not implemented: KChartWizardSelectChartTypePage::apply()" );
#warning Put back in
#ifdef K
    _chart->params()->type = _type;
#endif
}

#include "kchartWizardSelectChartTypePage.moc"
