/* This file is part of the KDE project
   Copyright (C) 2001, 2002, 2003 The Karbon Developers

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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include <QLabel>
#include <qpainter.h>
#include <QLayout>
#include <q3groupbox.h>
#include <QPushButton>
#include <qfileinfo.h>
#include <qpixmap.h>
#include <QPaintEvent>
#include <QGridLayout>
#include <Q3PtrList>
#include <QPointF>

#include <knuminput.h>
#include <kcombobox.h>
#include <klocale.h>
#include <klistbox.h>
#include <kiconloader.h>

#include "vgradientwidget.h"
#include "vgradienttabwidget.h"
#include "karbon_resourceserver.h"
#include "vkopainter.h"
#include "vfill.h"

VGradientListItem::VGradientListItem( const VGradient& gradient, QString filename )
		: Q3ListBoxItem( 0L ), m_filename( filename )
{
	m_gradient = new VGradient( gradient );

	m_pixmap.scaled( 200, 16 );
	VKoPainter gp( &m_pixmap, m_pixmap.width(), m_pixmap.height() );
	// Port to Qt4:
	// gp.setRasterOp( Qt::XorROP );
	gp.newPath();
	VGradient grad( *m_gradient );
	grad.setOrigin( QPointF( 0, 0 ) );
	grad.setVector( QPointF( m_pixmap.width() - 1, 0 ) );
	grad.setType( VGradient::linear );
	VFill fill;
	fill.gradient() = grad;
	fill.setType( VFill::grad );
	gp.setBrush( fill );
/* TODO: Port to Qt4:
	gp.moveTo( QPointF( 0, 0 ) );
	gp.lineTo( QPointF( 0, m_pixmap.height() - 1 ) );
	gp.lineTo( QPointF( m_pixmap.width() - 1, m_pixmap.height() - 1 ) );
	gp.lineTo( QPointF( m_pixmap.width() - 1, 0 ) );
	gp.lineTo( QPointF( 0, 0 ) );
*/
	gp.fillPath();
	gp.end();

	m_delete = QFileInfo( filename ).isWritable();
} // VGradientListItem::VGradientListItem

VGradientListItem::VGradientListItem( const VGradientListItem& gradient )
		: Q3ListBoxItem( 0L )
{
	m_pixmap = gradient.m_pixmap;
	m_delete = gradient.m_delete;
	m_gradient = new VGradient( *gradient.gradient() );
	m_filename = gradient.m_filename;
} // VGradientListItem::VGradientListItem

VGradientListItem::~VGradientListItem()
{
	delete m_gradient;
} // VGradientListItem::~VGradientListItem

int VGradientListItem::width( const Q3ListBox* lb ) const
{
	return lb->width() - 25;
} // VGradientListItem::width

void VGradientListItem::paint( QPainter* painter )
{
	painter->save();
	// TODO: Port to Qt4:
	// painter->setRasterOp( Qt::CopyROP );
	QRect r ( 0, 0, width( listBox() ), height( listBox() ) );
	painter->scale( ( (float)( width( listBox() ) ) ) / 200., 1. );
	painter->drawPixmap( 0, 0, m_pixmap );
	painter->restore();
	if ( isSelected() )
		painter->setPen( listBox()->palette().highlightedText().color() );
	else
		painter->setPen( listBox()->palette().base().color() );
	painter->drawRect( r );
	// TODO: Port to Qt4:
	// painter->flush();
} // VGradientListItem::paint

VGradientPreview::VGradientPreview( VGradient& gradient, double& opacity, QWidget* parent, const char* name )
		: QWidget( parent ), m_gradient( &gradient ), m_opacity( &opacity )
{
	setObjectName(name);

	QPalette p = palette();
	p.setBrush(QPalette::Window, QBrush(Qt::NoBrush));
	// TODO: check if this is equivalent with the line below
	// setBackgroundMode( Qt::NoBackground );
	setMinimumSize( 70, 70 );
} // VGradientPreview::VGradientPreview

VGradientPreview::~VGradientPreview()
{
} // VGradientPreview::~VGradientPreview

void VGradientPreview::paintEvent( QPaintEvent* )
{
	QPixmap pixmap( width(), height() );
	VKoPainter gp( &pixmap, width(), height() );
	// Port to Qt4:
	// gp.setRasterOp( Qt::XorROP );
	gp.newPath();
	VGradient gradient( *m_gradient );
	if( gradient.type() == VGradient::radial || gradient.type() == VGradient::conic )
	{
		gradient.setOrigin( QPointF( width() / 2, height() / 2 ) );
		gradient.setFocalPoint( QPointF( width() / 2, height() / 2 ) );
		gradient.setVector( QPointF( width() / 4, height() / 4 ) );
	}
	else
	{
		gradient.setOrigin( QPointF( width() / 3, 2 * ( height() / 3 ) ) );
		gradient.setVector( QPointF( 2 * ( width() / 3 ), height() / 3 ) );
	}
	VFill fill;
	KIconLoader il;
	fill.pattern() = VPattern( il.iconPath( "karbon.png", K3Icon::Small ) );
	fill.setType( VFill::patt );
	gp.setBrush( fill );
	gp.fillPath();
	fill.gradient() = gradient;
	fill.setType( VFill::grad );
	VColor c = fill.color();
	c.setOpacity( *m_opacity );
	fill.setColor( c, false );
	gp.setBrush( fill );
	/* TODO: Port to Qt4:
	gp.moveTo( QPointF( 2, 2 ) );
	gp.lineTo( QPointF( 2, height() - 2 ) );
	gp.lineTo( QPointF( width() - 2, height() - 2 ) );
	gp.lineTo( QPointF( width() - 2, 2 ) );
	gp.lineTo( QPointF( 2, 2 ) );
	*/
	gp.fillPath();
	gp.end();

	QPainter p( &pixmap );

	p.setPen( palette().light().color() );
	/* TODO: Port to Qt4:
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
	p.lineTo( 2, height() - 2 );*/

	QPainter p2(this);
	p2.drawPixmap(QPoint(0, 0), pixmap, QRect(0, 0, width(), height()));
} // VGradientPreview::paintEvent

VGradientTabWidget::VGradientTabWidget( VGradient& gradient, KarbonResourceServer* server, QWidget* parent, const char* name )
		: QTabWidget( parent ), m_gradient( gradient ), m_resourceServer( server )
{
	setObjectName(name);
	setupUI();
	setupConnections();
	initUI();
} // VGradientTabWidget::VGradientTabWidget

VGradientTabWidget::~VGradientTabWidget()
{
} // VGradientTabWidget::~VGradientTabWidget

void VGradientTabWidget::setupUI()
{
	m_editGroup = new Q3GroupBox( i18n( "Edit Gradient" ) );
	QGridLayout* editLayout = new QGridLayout;
	editLayout->setSpacing( 3 );
	editLayout->setMargin( 6 );
	editLayout->setRowMinimumHeight( 0, 12 );
	editLayout->addWidget( m_gradientPreview = new VGradientPreview( m_gradient, m_gradOpacity, m_editGroup ), 1, 3, 0, 0 );
	editLayout->addWidget( new QLabel( i18n( "Type:" ), m_editGroup ), 1, 1 );
	editLayout->addWidget( new QLabel( i18n( "Repeat:" ), m_editGroup ), 2, 1 );
	editLayout->addWidget( new QLabel( i18n( "Target:" ), m_editGroup ), 3, 1 );
	editLayout->addWidget( m_gradientType = new KComboBox( false, m_editGroup ), 1, 2 );
	m_gradientType->insertItem( 0, i18n( "Linear" ) );
	m_gradientType->insertItem( 1, i18n( "Radial" ) );
	m_gradientType->insertItem( 2, i18n( "Conical" ) );
	editLayout->addWidget( m_gradientRepeat = new KComboBox( false, m_editGroup ), 2, 2 );
	m_gradientRepeat->insertItem( 0, i18n( "None" ) );
	m_gradientRepeat->insertItem( 1, i18n( "Reflect" ) );
	m_gradientRepeat->insertItem( 2, i18n( "Repeat" ) );
	editLayout->addWidget( m_gradientTarget = new KComboBox( false, m_editGroup ), 3, 2 );
	m_gradientTarget->insertItem( 0, i18n( "Stroke" ) );
	m_gradientTarget->insertItem( 1, i18n( "Fill" ) );
	editLayout->addWidget( m_addToPredefs = new QPushButton( i18n( "&Add to Predefined Gradients" ), m_editGroup ), 6, 6, 0, 2 );
	editLayout->addWidget( m_gradientWidget = new VGradientWidget( m_gradient, m_editGroup ), 4, 4, 0, 2 );
	editLayout->addWidget( new QLabel( i18n( "Overall opacity:" ), m_editGroup ), 5, 0 );
	m_opacity = new KIntNumInput( 100, m_editGroup );
	m_opacity->setRange( 0, 100, 1, true );
	m_opacity->setValue( 100 );
	editLayout->addWidget( m_opacity, 5, 5, 1, 2 );
	m_editGroup->setLayout(editLayout);
	addTab( m_editGroup, i18n( "Edit" ) );

	Q3GroupBox* predefGroup  = new Q3GroupBox( i18n( "Predefined Gradients" ) );
	QGridLayout* predefLayout = new QGridLayout;
	predefLayout->setSpacing( 3 );
	predefLayout->setMargin( 6 );
	predefLayout->setRowMinimumHeight( 0, 12 );
	predefLayout->addWidget( m_predefGradientsView = new KListBox( predefGroup ), 1, 1, 0, 2 );
	predefLayout->addWidget( m_predefDelete = new QPushButton( i18n( "&Delete" ), predefGroup ), 2, 0 );
	predefLayout->addWidget( m_predefImport = new QPushButton( i18n( "&Import" ), predefGroup ), 2, 1 );
	m_predefImport->setEnabled( false );
	predefGroup->setLayout(predefLayout);
	addTab( predefGroup, i18n( "Predefined" ) );
} // VGradientTabWidget::setupUI

void VGradientTabWidget::setupConnections()
{
	connect( m_gradientType,		SIGNAL( activated( int ) ),				this, SLOT( combosChange( int ) ) );
	connect( m_gradientRepeat,		SIGNAL( activated( int ) ),				this, SLOT( combosChange( int ) ) );
	connect( m_gradientWidget,		SIGNAL( changed() ),					m_gradientPreview, SLOT( update() ) );
	connect( m_addToPredefs,		SIGNAL( clicked() ),					this, SLOT( addGradientToPredefs() ) );
	connect( m_predefGradientsView, SIGNAL( doubleClicked( Q3ListBoxItem *, const QPoint & ) ),	this, SLOT( changeToPredef( Q3ListBoxItem* ) ) );
	connect( m_predefDelete,		SIGNAL( clicked() ),					this, SLOT( deletePredef() ) );
	connect( m_opacity,				SIGNAL( valueChanged( int ) ),			this, SLOT( opacityChanged( int ) ) );
} // VGradientTabWidget::setupConnection

void VGradientTabWidget::initUI()
{
	m_gradientType->setCurrentIndex( m_gradient.type() );
	m_gradientRepeat->setCurrentIndex( m_gradient.repeatMethod() );
	m_gradientTarget->setCurrentIndex( FILL );
	m_opacity->setValue( 100 );

	m_predefGradientsView->clear();
	Q3PtrList<VGradientListItem>* gradientList = m_resourceServer->gradients();
	if( gradientList->count() > 0 )
		for( VGradientListItem* g = gradientList->first(); g != NULL; g = gradientList->next() )
			m_predefGradientsView->insertItem( new VGradientListItem( *g ) );
} // VGradientTabWidget::initUI

double
VGradientTabWidget::opacity() const
{
	return m_opacity->value() / 100.0;
}

void
VGradientTabWidget::setOpacity( double opacity )
{
	if( opacity < 0.0 || opacity > 1.0 )
		return;

	m_gradOpacity = opacity;
	m_opacity->setValue( int(opacity*100.0) );
}

const VGradient&
VGradientTabWidget::gradient()
{
	return m_gradient;
} // VGradientTabWidget::gradient

void VGradientTabWidget::setGradient( VGradient& gradient )
{
	m_gradient = gradient;

	initUI();
} // VGradientTabWidget::setGradient

VGradientTabWidget::VGradientTarget VGradientTabWidget::target()
{
	return (VGradientTarget)m_gradientTarget->currentIndex();
} // VGradientTabWidget::target

void VGradientTabWidget::setTarget( VGradientTarget target )
{
	m_gradientTarget->setCurrentIndex( target );
} // VGradientTabWidget::setTarget

void VGradientTabWidget::combosChange( int )
{
	m_gradient.setType( (VGradient::VGradientType)m_gradientType->currentIndex() );
	m_gradient.setRepeatMethod( (VGradient::VGradientRepeatMethod)m_gradientRepeat->currentIndex() );

	m_gradientPreview->update();
} // VGradientTabWidget::combosChange

void VGradientTabWidget::opacityChanged( int value )
{
	m_gradOpacity = value / 100.0;
	m_gradientPreview->update();
}

void VGradientTabWidget::addGradientToPredefs()
{
	VGradientListItem* item = m_resourceServer->addGradient( new VGradient( m_gradient ) );
	m_predefGradientsView->insertItem( item );
} // VGradientTabWidget::addGradientToPredefs()

void VGradientTabWidget::predefSelected( Q3ListBoxItem* item )
{
	if( item )
	{
		VGradientListItem* gradientItem = (VGradientListItem*)item;
		m_predefDelete->setEnabled( gradientItem->canDelete() );
	}
} // VGradientTabWidget::predefSelected

void VGradientTabWidget::changeToPredef( Q3ListBoxItem* item )
{
	if( item )
	{
		VGradientListItem* gradientItem = (VGradientListItem*)item;
		m_gradient = *gradientItem->gradient();
		m_gradientType->setCurrentIndex( m_gradient.type() );
		m_gradientRepeat->setCurrentIndex( m_gradient.repeatMethod() );
		m_opacity->setValue( 100 );
		m_gradientPreview->update();
		m_gradientWidget->update();
		setCurrentWidget( m_editGroup );
	}
} // VGradientTabWidget::changeToPredef

void VGradientTabWidget::deletePredef()
{
	int i = m_predefGradientsView->currentItem();
	if( !m_predefGradientsView->item( i ) )
		return;
	m_resourceServer->removeGradient( (VGradientListItem*)m_predefGradientsView->item( i ) );
	m_predefGradientsView->removeItem( i );
} // VGradientTabWidget::deletePredef

#include "vgradienttabwidget.moc"

