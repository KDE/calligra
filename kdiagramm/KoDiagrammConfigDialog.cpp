/*
 * $Id$
 *
 * Copyright 1999 by Kalle Dalheimer, released under Artistic License.
 */

#include "KoDiagrammConfigDialog.h"
#include "KoDiagrammColorConfigPage.h"
#include "KoDiagrammGeometryConfigPage.h"
#include "KoDiagrammParameters.h"

#include "KoDiagrammConfigDialog.moc"

#include <kapp.h>
#include <klocale.h>

KoDiagrammConfigDialog::KoDiagrammConfigDialog( KoDiagrammParameters* params,
												QWidget* parent ) :
	QTabDialog( parent, "Chart config dialog", true ),
	_params( params )
{
	// Geometry page
	// PENDING(kalle) _xstep only for axes charts
	_geompage = new KoDiagrammGeometryConfigPage( this );
	addTab( _geompage, i18n( "&Geometry" ) );

	// Color page
	_colorpage = new KoDiagrammColorConfigPage( this );
	addTab( _colorpage, i18n( "&Colors" ) );

	// setup buttons
	setDefaultButton( i18n( "Defaults" ) );
	setCancelButton( i18n( "Cancel" ) );
	connect( this, SIGNAL( applyButtonPressed() ), this, SLOT( apply() ) );
	connect( this, SIGNAL( defaultButtonPressed() ), this, SLOT( defaults() ) );
}


void KoDiagrammConfigDialog::apply()
{
	// Copy application data from dialog into parameter structure that is also 
	// being used by the application.

	// color page
	_params->_accentcolor = _colorpage->accentColor();
	_params->_axislabelcolor = _colorpage->axisLabelColor();
	_params->_fgcolor = _colorpage->foregroundColor();
	_params->_bgcolor = _colorpage->backgroundColor();
	_params->_textcolor = _colorpage->textColor();
	_params->_labelcolor = _colorpage->labelColor();
	for( uint i = 0; i < NUMDATACOLORS; i++ )
		_params->_datacolors.setColor( i, _colorpage->dataColor( i ) );
}


void KoDiagrammConfigDialog::defaults()
{
	// Fill pages with values

	// color page
	_colorpage->setAccentColor( _params->_accentcolor );
	_colorpage->setAxisLabelColor( _params->_axislabelcolor );
	_colorpage->setForegroundColor( _params->_fgcolor );
	_colorpage->setBackgroundColor( _params->_bgcolor );
	_colorpage->setTextColor( _params->_textcolor );
	_colorpage->setLabelColor( _params->_labelcolor );
	for( uint i = 0; i < NUMDATACOLORS; i++ )
		_colorpage->setDataColor( i, _params->_datacolors.color( i ) );
}

