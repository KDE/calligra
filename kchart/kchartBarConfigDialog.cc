/*
 *
 * Copyright 1999 by Kalle Dalheimer, released under Artistic License.
 */

#include "kchartBarConfigDialog.h"
#include "kchartBarConfigPage.h"

#include "kchartBarConfigDialog.moc"

#include <kapplication.h>
#include <klocale.h>

#include "kchart_params.h"

KChartBarConfigDialog::KChartBarConfigDialog( KChartParams* params,
					      QWidget* parent, int flag, KoChart::Data *dat ) :
    KChartConfigDialog( params, parent, flag,dat  )
{
    // Bar page
    _barpage = new KChartBarConfigPage( this );
    addTab( _barpage, i18n( "&Bars" ) );

    // Fill pages with values
    //defaults();

    setCaption( i18n( "Configuration of Bar Charts" ) );
}


void KChartBarConfigDialog::apply()
{
    KChartConfigDialog::apply();
     //_params->_overwrite = _barpage->overwriteMode();
     //_params->_xbardist = _barpage->xAxisDistance();
}


void KChartBarConfigDialog::defaults()
{
    KChartConfigDialog::defaults();
     //_barpage->setOverwriteMode( _params->_overwrite );
     //_barpage->setXAxisDistance( _params->_xbardist );
}



