/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers
*/

#include <qbuttongroup.h>
#include <qtoolbutton.h>

#include <kdualcolorbtn.h>
#include <kdualcolorbutton.h>
#include <kiconloader.h>
#include <koMainWindow.h>
#include <koView.h>
#include <kpixmap.h>
#include <kstandarddirs.h>

#include "karbon_part.h"
#include "karbon_factory.h"
#include "vtoolcontainer.h"
#include "vstrokefillpreview.h"


QPtrDict< VToolContainer > VToolContainer::m_containers;

VToolContainer *VToolContainer::instance( KarbonPart* part, KoView* parent,
	const char* name )
{
	if( !m_containers.find( part ) )
		m_containers.insert( part, new VToolContainer( parent, name ) );

	return m_containers.find( part );
}

VToolContainer::VToolContainer( KoView* parent, const char* /*name*/ )
	: QToolBar( parent->shell() )
{
	setOrientation( Vertical );
	setLabel( "" );

	QToolButton* button;

	btngroup = new QButtonGroup( 2, Horizontal, this );
	btngroup->setExclusive( true );
	btngroup->setInsideSpacing( 2 );
	btngroup->setInsideMargin( 5 );

	button = new QToolButton( btngroup );
	QPixmap pixmap = BarIcon( "select", KarbonFactory::instance() );
	button->setPixmap( pixmap );
	button->setToggleButton( true );
	button->toggle();
	connect( button, SIGNAL( clicked() ), this, SIGNAL( selectToolActivated() ) );
	btngroup->insert( button, Select );

	button = new QToolButton( btngroup );
	pixmap = BarIcon( "14_scale", KarbonFactory::instance() );
	button->setPixmap( pixmap );
	button->setToggleButton( true );
	connect( button, SIGNAL( clicked() ), this, SIGNAL( scaleToolActivated() ) );
	btngroup->insert( button, Scale );

	button = new QToolButton(btngroup);
	pixmap = BarIcon( "14_rotate", KarbonFactory::instance() );
	button->setPixmap( pixmap );
	button->setToggleButton( true );
	connect( button, SIGNAL( clicked() ), this, SIGNAL( rotateToolActivated() ) );
	btngroup->insert( button, Rotate );

	button = new QToolButton(btngroup);
	pixmap = BarIcon( "14_shear", KarbonFactory::instance() );
	button->setPixmap( pixmap );
	button->setToggleButton( true );
	connect( button, SIGNAL( clicked() ), this, SIGNAL( shearToolActivated() ) );
	btngroup->insert( button, Shear );

	button = new QToolButton(btngroup);
	pixmap = BarIcon( "14_ellipse", KarbonFactory::instance() );
	button->setPixmap( pixmap );
	button->setToggleButton( true );
	connect( button, SIGNAL( clicked() ), this, SIGNAL( ellipseToolActivated() ) );
	btngroup->insert( button, Ellipse );

	button = new QToolButton(btngroup);
	pixmap = BarIcon( "14_rectangle", KarbonFactory::instance() );
	button->setPixmap( pixmap );
	button->setToggleButton( true );
	connect( button, SIGNAL( clicked() ), this, SIGNAL( rectangleToolActivated() ) );
	btngroup->insert( button, Rectangle );

	button = new QToolButton( btngroup );
	pixmap=BarIcon( "14_roundrect", KarbonFactory::instance() );
	button->setPixmap( pixmap );
	button->setToggleButton( true );
	connect( button, SIGNAL( clicked() ), this, SIGNAL( roundRectToolActivated() ) );
	btngroup->insert( button, Roundrect );

	button = new QToolButton( btngroup );
	pixmap=BarIcon( "14_polygon", KarbonFactory::instance() );
	button->setPixmap( pixmap );
	button->setToggleButton( true );
	connect( button, SIGNAL( clicked() ), this, SIGNAL( polygonToolActivated() ) );
	btngroup->insert( button, Polygon );

	button = new QToolButton(btngroup);
	pixmap = BarIcon( "14_star", KarbonFactory::instance() );
	button->setPixmap( pixmap );
	button->setToggleButton( true );
	connect( button, SIGNAL( clicked() ), this, SIGNAL( starToolActivated() ) );
	btngroup->insert( button, Star );

	button = new QToolButton( btngroup );
	pixmap = BarIcon( "14_sinus", KarbonFactory::instance() );
	button->setPixmap( pixmap );
	button->setToggleButton( true );
	connect( button, SIGNAL( clicked() ), this, SIGNAL( sinusToolActivated() ) );
	btngroup->insert( button, Sinus );

	button = new QToolButton( btngroup );
	pixmap = BarIcon( "14_spiral", KarbonFactory::instance() );
	button->setPixmap( pixmap );
	button->setToggleButton( true );
	connect( button, SIGNAL( clicked() ), this, SIGNAL( spiralToolActivated() ) );
	btngroup->insert( button, Spiral );

	button = new QToolButton( btngroup );
	pixmap = BarIcon( "14_text", KarbonFactory::instance() );
	button->setPixmap( pixmap );
	button->setToggleButton( true );
	connect( button, SIGNAL( clicked() ), this, SIGNAL( textToolActivated() ) );
	btngroup->insert( button, Text );

	//dialog buttons
	dlggroup = new QButtonGroup( 2, Horizontal, this );
	dlggroup->setInsideSpacing( 2 );
	dlggroup->setInsideMargin( 5 );

	// has to be a new non-toggle group (dialogs, not tools)
	button = new QToolButton( dlggroup );
	pixmap = BarIcon( "outlinedlg", KarbonFactory::instance());
	button->setPixmap( pixmap );
	connect( button, SIGNAL( clicked() ), this, SIGNAL( strokeActivated() ) );
	dlggroup->insert( button, Outline);

	button = new QToolButton( dlggroup );
	pixmap = BarIcon( "solidfilldlg", KarbonFactory::instance() );
	button->setPixmap( pixmap );
	connect( button, SIGNAL( clicked() ), this, SIGNAL( solidFillActivated() ) );
	dlggroup->insert( button, SolidFill );

	button = new QToolButton( dlggroup );
	pixmap = BarIcon( "gradientdlg", KarbonFactory::instance() );
	button->setPixmap( pixmap );
	dlggroup->insert( button, Gradient );

	//dialog buttons
	m_dualColorButton = new KDualColorButton( this );
	m_strokeFillPreview = new VStrokeFillPreview( this );

	connect(
		m_dualColorButton, SIGNAL( fgChanged( const QColor& ) ),
		this, SIGNAL(strokeColorChanged( const QColor& )));
	connect(
		m_dualColorButton, SIGNAL( bgChanged( const QColor& ) ),
		this, SIGNAL( fillColorChanged( const QColor& ) ) );

	m_dualColorButton->setMaximumWidth( 30 );
	m_dualColorButton->setMaximumHeight( 30 );
}

VToolContainer::~VToolContainer()
{
}


#include "vtoolcontainer.moc"

