/*
 * $Id$
 *
 * Copyright 1999 by Kalle Dalheimer, released under Artistic License.
 */

#include "kchartBarConfigDialog.h"
#include "kchartBarConfigPage.h"

#include "kchartBarConfigDialog.moc"

#include <kapp.h>
#include <klocale.h>

#include "kdchart/KDChartParams.h"

KChartBarConfigDialog::KChartBarConfigDialog( KDChartParams* params,
					      QWidget* parent ) :
    KChartConfigDialog( params, parent  )
{
    // Bar page
    _barpage = new KChartBarConfigPage( this );
    addTab( _barpage, i18n( "&Bars" ) );

    // Fill pages with values
    //defaults();

    setCaption( i18n( "Configuration of bar charts" ) );
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



