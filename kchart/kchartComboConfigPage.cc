/*
 * $Id$
 *
 * Copyright 2000 by Laurent Montel, released under Artistic License.
 */

#include "kchartComboConfigPage.h"
#include "kchartComboConfigPage.moc"

#include <kapp.h>
#include <klocale.h>
#include <kdebug.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qradiobutton.h>
#include <qbuttongroup.h>

#include "kdchart/KDChartParams.h"

KChartComboPage::KChartComboPage( KDChartParams* params,
                                  QWidget* parent ) :
    QWidget( parent ),_params( params )
{
    //QVBoxLayout* toplevel = new QVBoxLayout( this, 10 );

    QGridLayout* layout = new QGridLayout(this, 2, 2,15,7 );
    //toplevel->addLayout( layout );
    QButtonGroup* gb = new QButtonGroup( i18n("HLC Style"), this );
    QGridLayout *grid1 = new QGridLayout(gb,7,1,15,7);
    layout->addWidget(gb,0,0);

    diamond=new QRadioButton( i18n("Diamond"), gb ); ;
    grid1->addWidget(diamond,0,0);
    closeconnected=new QRadioButton( i18n("Close connected"), gb );
    grid1->addWidget(closeconnected,1,0);
    connecting=new QRadioButton( i18n("Connecting"), gb );
    grid1->addWidget(connecting,2,0);
    icap=new QRadioButton( i18n("Icap"), gb );
    grid1->addWidget(icap,3,0);
    gb->setAlignment(Qt::AlignLeft);
    grid1->addColSpacing(0,diamond->width());
    grid1->addColSpacing(0,closeconnected->width());
    grid1->addColSpacing(0,connecting->width());
    grid1->addColSpacing(0,icap->width());
    grid1->setColStretch(0,1);
    grid1->activate();
    //it's not good but I don't know how
    //to reduce space
    layout->addColSpacing(1,300);
}

void KChartComboPage::init()
{
    qDebug( "Sorry, not implemented: KChartComboPage::init()" );
#ifdef __GNUC__
#warning Put back in
#endif
#ifdef K
    switch((int)_params->hlc_style)
        {
        case (int)KCHARTHLCSTYLE_DIAMOND:
            {
                diamond->setChecked(true);
                break;
            }
        case (int)KCHARTHLCSTYLE_CLOSECONNECTED:
            {
                closeconnected->setChecked(true);
                break;
            }
        case (int)KCHARTHLCSTYLE_CONNECTING:
            {
                connecting->setChecked(true);
                break;
            }
        case (int)KCHARTHLCSTYLE_ICAP:
            {
                icap->setChecked(true);
                break;
            }
        default:
            {
                kdDebug(35001)<<"Error in hlc_style\n";
                break;
            }
        }
#endif
}

void KChartComboPage::apply()
{
    qDebug( "Sorry, not implemented: KChartComboPage::apply()" );
#ifdef __GNUC__
#warning Put back in
#endif
#ifdef K
    if(diamond->isChecked())
        {
            _params->hlc_style = KCHARTHLCSTYLE_DIAMOND;
        }
    else if(closeconnected->isChecked())
        {
            _params->hlc_style = KCHARTHLCSTYLE_CLOSECONNECTED;
        }
    else if(connecting->isChecked())
        {
            _params->hlc_style = KCHARTHLCSTYLE_CONNECTING;
        }
    else if(icap->isChecked())
        {
            _params->hlc_style = KCHARTHLCSTYLE_ICAP;
        }
    else
        {
            kdDebug(35001)<<"Error in groupbutton\n";
        }
#endif
}

