/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <qlabel.h>
#include <qframe.h>

#include <kstandarddirs.h> 
#include <kcolorbutton.h>
#include <kcombobox.h>
#include <klocale.h>
#include <qpainter.h>
#include <qlayout.h>
#include <qvbox.h>
#include <qgroupbox.h>
#include <qtabwidget.h>
#include <klistbox.h>
#include <qpushbutton.h>
#include <qdom.h>    
#include <qfile.h>
#include <qpixmap.h>
#include <kiconloader.h>

#include "vgradientwidget.h"
#include "vgradienttabwidget.h"
#include "karbon_resourceserver.h"
#include "../render/vkopainter.h"
#include "vfill.h"

VGradientPreview::VGradientPreview( VGradient*& gradient, QWidget* parent, const char* name )
		: QWidget( parent, name ), m_lpgradient( &gradient )
{  
	setBackgroundMode( Qt::NoBackground );
	setMinimumSize( 70, 70 );
} // VGradientPreview::VGradientPreview

VGradientPreview::~VGradientPreview()
{
} // VGradientPreview::~VGradientPreview

void VGradientPreview::paintEvent( QPaintEvent* )
{
	QPixmap pixmap( width(), height() );
	VKoPainter gp( &pixmap, width(), height() );
	gp.setRasterOp( Qt::XorROP );
	gp.newPath();
	VGradient gradient( **m_lpgradient );
	if ( gradient.type() == VGradient::radial )
	{
		gradient.setOrigin( KoPoint( width() / 2, height() / 2 ) );
		gradient.setVector( KoPoint( width() / 4, height() / 4 ) );
	}
	else
	{
		gradient.setOrigin( KoPoint( width() / 3, 2 * ( height() / 3 ) ) );
		gradient.setVector( KoPoint( 2 * ( width() / 3 ), height() / 3 ) );
	}
	VFill fill;
	KIconLoader il;
	fill.pattern() = VPattern( il.iconPath( "karbon.png", KIcon::Small ) );
	fill.setType( VFill::patt );
	gp.setBrush( fill );
	gp.fillPath();
	fill.gradient() = gradient;
	fill.setType( VFill::grad );
	gp.setBrush( fill );
	gp.moveTo( KoPoint( 2, 2 ) );
	gp.lineTo( KoPoint( 2, height() - 2 ) );
	gp.lineTo( KoPoint( width() - 2, height() - 2 ) );
	gp.lineTo( KoPoint( width() - 2, 2 ) );
	gp.lineTo( KoPoint( 2, 2 ) );
	gp.fillPath();
	gp.end();
	
	QPainter p( &pixmap );
	
	p.setPen( colorGroup().light() );
	p.moveTo( 1, height() - 1 );
	p.lineTo( 1, 1 );
	p.lineTo( width() - 1, 1 );
	p.lineTo( width() - 1, height() - 1 );
	p.lineTo( 1, height() - 1 );
	p.setPen( colorGroup().dark() );
	p.moveTo( 0, height() - 1 );
	p.lineTo( 0, 0 );
	p.lineTo( width() - 1, 0 );
	p.moveTo( width() - 2, 2 );
	p.lineTo( width() - 2, height() - 2 );
	p.lineTo( 2, height() - 2 );    
	bitBlt( this, 0, 0, &pixmap, 0, 0, width(), height() );
} // VGradientPreview::paintEvent

VGradientTabWidget::VGradientTabWidget( VGradient& gradient, KarbonResourceServer* server, QWidget* parent, const char* name )
		: QTabWidget( parent, name ), m_gradient( &gradient ), m_resourceServer( server )
{
	setupUI();
	setupConnections();
	initUI();
} // VGradientTabWidget::VGradientTabWidget

VGradientTabWidget::~VGradientTabWidget()
{
} // VGradientTabWidget::~VGradientTabWidget

void VGradientTabWidget::setupUI()
{
	m_editGroup    = new QGroupBox( i18n( "Edit gradient:" ) );
	QGridLayout* editLayout = new QGridLayout( m_editGroup, 6, 3 );
	editLayout->setSpacing( 3 );
	editLayout->setMargin( 6 );
	editLayout->addRowSpacing( 0, 12 );
	editLayout->addMultiCellWidget( m_gradientPreview = new VGradientPreview( m_gradient, m_editGroup ), 1, 3, 0, 0 );
	editLayout->addWidget( new QLabel( i18n( "Type:" ), m_editGroup ), 1, 1 );
	editLayout->addWidget( new QLabel( i18n( "Repeat:" ), m_editGroup ), 2, 1 );
	editLayout->addWidget( new QLabel( i18n( "Target:" ), m_editGroup ), 3, 1 );
	editLayout->addWidget( m_gradientType = new KComboBox( false, m_editGroup ), 1, 2 );
	m_gradientType->insertItem( i18n( "Linear" ), 0 );
	m_gradientType->insertItem( i18n( "Radial" ), 1 );
	m_gradientType->insertItem( i18n( "Conical" ), 2 );
	editLayout->addWidget( m_gradientRepeat = new KComboBox( false, m_editGroup ), 2, 2 );
	m_gradientRepeat->insertItem( i18n( "None" ), 0 );
	m_gradientRepeat->insertItem( i18n( "Reflect" ), 1 );
	m_gradientRepeat->insertItem( i18n( "Repeat" ), 2 );
	editLayout->addWidget( m_gradientTarget = new KComboBox( false, m_editGroup ), 3, 2 );
	m_gradientTarget->insertItem( i18n( "Stroke" ), 0 );
	m_gradientTarget->insertItem( i18n( "Fill" ), 1 );
	editLayout->addMultiCellWidget( m_addToPredefs = new QPushButton( i18n( "&Add to predefined gradients" ), m_editGroup ), 5, 5, 0, 2 );
	editLayout->addMultiCellWidget( m_gradientWidget = new VGradientWidget( m_gradient, m_editGroup ), 4, 4, 0, 2 );
	addTab( m_editGroup, i18n( "Edit" ) );

	QGroupBox* predefGroup  = new QGroupBox( 1, Qt::Horizontal, i18n( "Predefined gradients:" ) );
	QGridLayout* predefLayout = new QGridLayout( predefGroup );
	predefLayout->addMultiCellWidget( m_predefGradientsView = new KListBox( predefGroup ), 0, 0, 0, 1 );
	predefLayout->addWidget( m_predefDelete = new QPushButton( i18n( "&Delete" ), predefGroup ), 1, 0 );
	predefLayout->addWidget( m_predefImport = new QPushButton( i18n( "&Import" ), predefGroup ), 1, 1 );
	m_predefImport->setEnabled( false );
	addTab( predefGroup, i18n( "Predefined" ) );
} // VGradientTabWidget::setupUI

void VGradientTabWidget::setupConnections()
{
	connect( m_gradientType, SIGNAL( activated( int ) ), this, SLOT( combosChange( int ) ) );
	connect( m_gradientRepeat, SIGNAL( activated( int ) ), this, SLOT( combosChange( int ) ) );
	connect( m_gradientWidget, SIGNAL( changed() ), m_gradientPreview, SLOT( update() ) );
	connect( m_addToPredefs, SIGNAL( clicked() ), this, SLOT( addGradientToPredefs() ) );
	connect( m_predefGradientsView, SIGNAL( executed( QListBoxItem* ) ), this, SLOT( changeToPredef( QListBoxItem* ) ) );
	connect( m_predefGradientsView, SIGNAL( highlighted( QListBoxItem* ) ), this, SLOT( predefSelected( QListBoxItem* ) ) );
	connect( m_predefDelete, SIGNAL( clicked() ), this, SLOT( deletePredef() ) );
} // VGradientTabWidget::setupConnection

void VGradientTabWidget::initUI()
{
	m_gradientType->setCurrentItem( m_gradient->type() );
	m_gradientRepeat->setCurrentItem( m_gradient->repeatMethod() );
	m_gradientTarget->setCurrentItem( FILL );

	QPtrList<VGradientListItem>* gradientList = m_resourceServer->gradients();
	if ( gradientList->count() > 0 )
		for ( VGradientListItem* g = gradientList->first(); g != NULL; g = gradientList->next() )
		{
			m_predefGradientsView->insertItem( new VGradientListItem( *g ) );
		}
} // VGradientTabWidget::initUI

const VGradient* VGradientTabWidget::gradient()
{
	return m_gradient;
} // VGradientTabWidget::gradient

void VGradientTabWidget::setGradient( VGradient& gradient )
{
	m_gradient = &gradient;
	
	initUI();
} // VGradientTabWidget::setGradient

VGradientTabWidget::VGradientTarget VGradientTabWidget::target()
{
	return (VGradientTarget)m_gradientTarget->currentItem();
} // VGradientTabWidget::target

void VGradientTabWidget::setTarget( VGradientTarget target )
{
	m_gradientTarget->setCurrentItem( target );
} // VGradientTabWidget::setTarget

void VGradientTabWidget::combosChange( int )
{
	m_gradient->setType( (VGradient::VGradientType)m_gradientType->currentItem() );
	m_gradient->setRepeatMethod( (VGradient::VGradientRepeatMethod)m_gradientRepeat->currentItem() );
	
	m_gradientPreview->update();
} // VGradientTabWidget::combosChange

void VGradientTabWidget::addGradientToPredefs()
{
	VGradientListItem* item = m_resourceServer->addGradient( new VGradient( *m_gradient ) );
	m_predefGradientsView->insertItem( item );
} // VGradientTabWidget::addGradientToPredefs()

void VGradientTabWidget::predefSelected( QListBoxItem* item )
{
	if ( item )
	{
		VGradientListItem* gradientItem = (VGradientListItem*)item;
		m_predefDelete->setEnabled( gradientItem->canDelete() );
	}
} // VGradientTabWidget::predefSelected

void VGradientTabWidget::changeToPredef( QListBoxItem* item ) 
{
	if ( item )
	{
		VGradientListItem* gradientItem = (VGradientListItem*)item;
		(*m_gradient) = *( gradientItem->gradient() );
		m_gradientType->setCurrentItem( m_gradient->type() );
		m_gradientRepeat->setCurrentItem( m_gradient->repeatMethod() );
		m_gradientPreview->update();
		m_gradientWidget->update();
		showPage( m_editGroup );
	}
} // VGradientTabWidget::changeToPredef

void VGradientTabWidget::deletePredef()
{
	int i = m_predefGradientsView->currentItem();
	m_resourceServer->removeGradient( (VGradientListItem*)m_predefGradientsView->item( i ) );
	m_predefGradientsView->removeItem( i );
} // VGradientTabWidget::deletePredef

#include "vgradienttabwidget.moc"
