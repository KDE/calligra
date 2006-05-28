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

#include <math.h>
#include <config-karbon.h>
#include <QCheckBox>
#include <QComboBox>
#include <qcursor.h>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <qpainter.h>
#include <qpixmap.h>
#include <QPushButton>
#include <qtabwidget.h>
#include <QPointF>
#include <Q3HBoxLayout>
#include <Q3GridLayout>
#include <QMouseEvent>
#include <Q3VBoxLayout>
#include <QPaintEvent>

#include <kdebug.h>
#include <kfontcombo.h>
#include <kfontdialog.h>
#include <kiconloader.h>
#include <knuminput.h>
#include <kglobalsettings.h>

#include <karbon_view.h>
#include <karbon_part.h>
#include <core/vdocument.h>
#include <core/vglobal.h>
#include <core/vselection.h>
#include <core/vfill.h>
#include <core/vgroup.h>
#include <core/vstroke.h>
#include <core/vcursor.h>
#include <render/vpainterfactory.h>
#include <render/vkopainter.h>
#include "vtexttool.h"

#include <kactioncollection.h>


static void
traceShape( VKoPainter* p, int x, int y, int w, int h )
{
/* TODO lineto and moveto need porting
	p->newPath();
	p->moveTo( QPointF( x + w , y + h ) );
	p->lineTo( QPointF( x + w / 3, y + h ) );
	p->lineTo( QPointF( x + w / 3, y + h / 3 ) );
	p->lineTo( QPointF( x + w , y + h / 3 ) );
	p->lineTo( QPointF( x + w , y + h ) );

	p->moveTo( QPointF( x , y ) );
	p->lineTo( QPointF( x + ( w / 3 ) * 2, y ) );
	p->lineTo( QPointF( x + ( w / 3 ) * 2, y + ( h / 3 ) * 2 ) );
	p->lineTo( QPointF( x , y + ( h / 3 ) * 2 ) );
	p->lineTo( QPointF( x , y ) );*/
}

ShadowPreview::ShadowPreview( ShadowWidget* parent )
		: QWidget( parent ), m_parent( parent )
{
	setBackgroundMode( Qt::NoBackground );
	setMinimumSize( 60, 60 );

	connect( this, SIGNAL( changed( int, int, bool ) ), m_parent, SLOT( setShadowValues( int, int, bool ) ) );
}

ShadowPreview::~ShadowPreview()
{
}

void
ShadowPreview::mouseReleaseEvent( QMouseEvent* e )
{
	int dx = e->x() - width() / 2;
	int dy = e->y() - height() / 2;

	float fd = sqrt( double( dx * dx + dy * dy ) );
	int a;

	if( fd == 0 )
		a = 0;
	else if( dy == 0 && dx < 0 )
		a = 180;
	else
	{
		float r = acos( dx / fd );
		a = int( ( dy <= 0 ? r : VGlobal::twopi - r ) / VGlobal::twopi * 360. );
	}

	emit changed( a, ( int ) fd, m_parent->isTranslucent() );
}

void
ShadowPreview::paintEvent( QPaintEvent* )
{
	int w = width() - 4;
	int h = height() - 4;
	int d = m_parent->shadowDistance();
	int a = 360 - m_parent->shadowAngle();

	QPixmap pm( w, h );
	VKoPainter p( &pm, w, h );
	VColor color( VColor::rgb );

	VFill fill;
	KIconLoader il;
	fill.pattern() = VPattern( il.iconPath( "karbon.png", K3Icon::Small ) );
	fill.setType( VFill::patt );

	p.newPath();
	p.moveTo( QPointF( 0, 0 ) );
	p.lineTo( QPointF( 0, h ) );
	p.lineTo( QPointF( w, h ) );
	p.lineTo( QPointF( w, 0 ) );
	p.lineTo( QPointF( 0, 0 ) );
	p.setBrush( fill );
	p.fillPath();

	color.set( 1., 1., 1. );
	color.setOpacity( .5 );
	p.setBrush( VFill( color ) );
	p.fillPath();

	if( m_parent->isTranslucent() )
	{
		color.set( 0., 0., 0. );
		color.setOpacity( .3 );
	}
	else
	{
		color.set( .3, .3, .3 );
		color.setOpacity( 1. );
	}

	p.setPen( VStroke( color ) );
	p.setBrush( VFill( color ) );

	traceShape(
		&p,
		int( w / 4 + d * cos( a / 360. * VGlobal::twopi ) ),
		int( h / 4 + d * sin( a / 360. * VGlobal::twopi ) ), int( w / 2 ), int( h / 2 ) );

	p.strokePath();
	p.fillPath();

	color.set( 0., 0., 1. );
	color.setOpacity( 1. );
	p.setBrush( VFill( color ) );
	color.set( 0., 0., .5 );
	p.setPen( VStroke( color ) );
	traceShape( &p, w / 4, h / 4, w / 2, h / 2 );
	p.strokePath();
	p.fillPath();

	if( !m_parent->useShadow() )
	{
/* TODO moveto and lineto need porting
		p.newPath();
		p.moveTo( QPointF( 0, 0 ) );
		p.lineTo( QPointF( 0, h ) );
		p.lineTo( QPointF( w, h ) );
		p.lineTo( QPointF( w, 0 ) );
		p.lineTo( QPointF( 0, 0 ) );
		VColor c( colorGroup().background() );
		c.setOpacity( .8 );
		p.setBrush( VFill( c ) );
		p.fillPath();*/
	}

	p.end();

/* TODO moveto and lineto need porting
	QPainter painter( this );
	painter.drawPixmap( 2, 2, pm );
	painter.setPen( colorGroup().light() );
	painter.moveTo( 1, height() - 1 );
	painter.lineTo( 1, 1 );
	painter.lineTo( width() - 1, 1 );
	painter.lineTo( width() - 1, height() - 1 );
	painter.lineTo( 1, height() - 1 );
	painter.setPen( colorGroup().dark() );
	painter.moveTo( 0, height() - 1 );
	painter.lineTo( 0, 0 );
	painter.lineTo( width() - 1, 0 );
	painter.moveTo( width() - 2, 2 );
	painter.lineTo( width() - 2, height() - 2 );
	painter.lineTo( 2, height() - 2 );
	painter.setPen( Qt::black );
	painter.drawLine( width() / 2 - 2, height() / 2, width() / 2 + 2, height() / 2 );
	painter.drawLine( width() / 2, height() / 2 - 2, width() / 2, height() / 2 + 2 );*/
}

ShadowWidget::ShadowWidget( QWidget* parent, const char* name, int angle, int distance, bool translucent )
		: Q3GroupBox( parent, name )
{
	setTitle( i18n( "Shadow" ) );
	setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Minimum ) );

	Q3GridLayout* layout = new Q3GridLayout( this );
	layout->addRowSpacing( 0, 12 );
	layout->setMargin( 3 );
	layout->setSpacing( 2 );
	layout->setColStretch( 0, 1 );
	layout->setColStretch( 1, 0 );
	layout->setColStretch( 2, 2 );
	layout->addMultiCellWidget( m_preview = new ShadowPreview( this ), 1, 3, 0, 0 );
	layout->addWidget( new QLabel( i18n( "Angle:" ), this ), 1, 1 );
	layout->addWidget( m_angle = new KIntNumInput( this ), 1, 2 );
	layout->addWidget( new QLabel( i18n( "Distance:" ), this ), 2, 1 );
	layout->addWidget( m_distance = new KIntNumInput( this ), 2, 2 );
	layout->addWidget( m_useShadow = new QCheckBox( i18n( "Shadow" ), this ), 3, 1 );
	layout->addWidget( m_translucent = new QCheckBox( i18n( "Draw translucent shadow" ), this ), 3, 2 );
	m_distance->setRange( 1, 37, 1, true );
	m_angle->setRange( 0, 360, 10, true );
	m_angle->setValue( angle );
	m_distance->setValue( distance );
	m_translucent->setChecked( translucent );

	connect( m_angle, SIGNAL( valueChanged( int ) ), this, SLOT( updatePreview( int ) ) );
	connect( m_distance, SIGNAL( valueChanged( int ) ), this, SLOT( updatePreview( int ) ) );
	connect( m_useShadow, SIGNAL( clicked() ), this, SLOT( updatePreview() ) );
	connect( m_translucent, SIGNAL( clicked() ), this, SLOT( updatePreview() ) );

	updatePreview();
}

ShadowWidget::~ShadowWidget()
{
}

void
ShadowWidget::setUseShadow( bool use )
{
	m_useShadow->setChecked( use );
	updatePreview();
}

bool ShadowWidget::useShadow()
{
	return m_useShadow->isChecked();
}

void
ShadowWidget::setShadowAngle( int angle )
{
	m_angle->setValue( angle );
	m_preview->repaint();
}

int
ShadowWidget::shadowAngle()
{
	return m_angle->value();
}

void
ShadowWidget::setShadowDistance( int distance )
{
	m_distance->setValue( distance );
	m_preview->repaint();
}

int
ShadowWidget::shadowDistance()
{
	return m_distance->value();
}

void
ShadowWidget::setTranslucent( bool translucent )
{
	m_translucent->setChecked( translucent );
	m_preview->repaint();
}

bool ShadowWidget::isTranslucent()
{
	return m_translucent->isChecked();
}

void
ShadowWidget::setShadowValues( int angle, int distance, bool translucent )
{
	m_angle->setValue( angle );
	m_distance->setValue( distance );
	m_translucent->setChecked( translucent );
	updatePreview();
}

void
ShadowWidget::updatePreview( int )
{
	m_preview->repaint();
}

void
ShadowWidget::updatePreview()
{
	m_preview->repaint();
	bool ok = m_useShadow->isChecked();
	m_angle->setEnabled( ok );
	m_distance->setEnabled( ok );
	m_translucent->setEnabled( ok );
}

VTextOptionsWidget::VTextOptionsWidget( VTextTool* tool, QWidget *parent )
	: KDialogBase( parent, "", true, i18n( "Text" ), Ok | Cancel ), m_tool( tool )
{
	//setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Minimum ) );
	//setFrameStyle( Box | Sunken );
	QWidget *base = new QWidget( this );
	Q3VBoxLayout* mainLayout = new Q3VBoxLayout( base );
	mainLayout->setMargin( 3 );

	mainLayout->add( m_tabWidget = new QTabWidget( base ) );

	m_tabWidget->setFont( QFont( KGlobalSettings::generalFont().family() , 8 ) );

	QWidget* textWidget = new QWidget( m_tabWidget );

	Q3GridLayout* textLayout = new Q3GridLayout( textWidget );

	QStringList list;
	KFontChooser::getFontList( list, KFontChooser::SmoothScalableFonts );

	textLayout->setMargin( 3 );
	textLayout->setSpacing( 2 );
	textLayout->addMultiCellWidget( m_fontCombo = new KFontCombo( list, textWidget ), 0, 0, 0, 2 );
	textLayout->addWidget( m_fontSize = new KIntNumInput( textWidget ), 1, 0 );
	textLayout->addWidget( m_boldCheck = new QCheckBox( i18n( "Bold" ), textWidget ), 1, 1 );
	textLayout->addWidget( m_italicCheck = new QCheckBox( i18n( "Italic" ), textWidget ), 1, 2 );
	textLayout->addMultiCellWidget( m_textEditor = new QLineEdit( textWidget ), 2, 2, 0, 2 );

	m_tabWidget->addTab( textWidget, i18n( "Text" ) );

	QWidget* posWidget = new QWidget( m_tabWidget );
	
	Q3GridLayout* posLayout = new Q3GridLayout( posWidget );
	textLayout->setMargin( 3 );
	posLayout->setSpacing( 2 );
	posLayout->addWidget( new QLabel( i18n( "Alignment:" ), posWidget ), 0, 0 );
	posLayout->addWidget( m_textAlignment = new QComboBox( posWidget ), 0, 1 );
	posLayout->addWidget( new QLabel( i18n( "Position:" ), posWidget ), 1, 0 );
	posLayout->addWidget( m_textPosition = new QComboBox( posWidget ), 1, 1 );
	posLayout->addWidget( new QLabel( i18n( "Offset:" ), posWidget ), 2, 0 );
	posLayout->addWidget( m_textOffset = new KDoubleNumInput( posWidget ), 2, 1 );
	posLayout->setColStretch( 0, 0 );
	posLayout->setColStretch( 1, 1 );
	
	m_tabWidget->addTab( posWidget, i18n( "Position" ) );

	QWidget* fxWidget = new QWidget( m_tabWidget );

	Q3VBoxLayout* fxLayout = new Q3VBoxLayout( fxWidget );

	fxLayout->setMargin( 3 );
	fxLayout->setSpacing( 2 );
	fxLayout->add( m_shadow = new ShadowWidget( fxWidget, 0L, 315, 4, true ) );

	Q3HBoxLayout* fxLayout2 = new Q3HBoxLayout( fxLayout );

	fxLayout2->setSpacing( 2 );
	fxLayout2->addWidget( m_editBasePath = new QPushButton( i18n( "Edit Base Path" ), fxWidget ) );
	fxLayout2->addWidget( m_convertToShapes = new QPushButton( i18n( "Convert to Shapes" ), fxWidget ) );

	m_tabWidget->addTab( fxWidget, i18n( "Effects" ) );

	m_fontCombo->setCurrentText( KGlobalSettings::generalFont().family() );

	m_fontSize->setValue( 12 );
	m_fontSize->setSuffix( " pt" );

	m_textEditor->setMinimumHeight( 100 );
	m_textEditor->setText( i18n( "New text") );
	m_textEditor->selectAll();

	m_convertToShapes->setEnabled( true );

	/* TODO i18 needs porting
	m_textAlignment->insertItem( i18n( "Horizontal alignment", "Left") );
	m_textAlignment->insertItem( i18n( "Horizontal alignment", "Center") );
	m_textAlignment->insertItem( i18n( "Horizontal alignment", "Right") );

	m_textPosition->insertItem( i18n( "Vertical alignment", "Above") );
	m_textPosition->insertItem( i18n( "Vertical alignment", "On") );
	m_textPosition->insertItem( i18n( "Vertical alignment", "Under") );*/

	m_textOffset->setRange( 0.0, 100.0, 1.0, true );

	connect( m_fontCombo, SIGNAL( activated( int ) ), this, SLOT( valueChanged( int ) ) );
	connect( m_boldCheck, SIGNAL( stateChanged( int ) ), this, SLOT( valueChanged( int ) ) );
	connect( m_italicCheck, SIGNAL( stateChanged( int ) ), this, SLOT( valueChanged( int ) ) );
	connect( m_fontSize, SIGNAL( valueChanged( int ) ), this, SLOT( valueChanged( int ) ) );
	connect( m_textPosition, SIGNAL( activated( int ) ), this, SLOT( valueChanged( int ) ) );
	connect( m_textAlignment, SIGNAL( activated( int ) ), this, SLOT( valueChanged( int ) ) );
	connect( m_textOffset, SIGNAL( valueChanged( double ) ), this, SLOT( valueChanged( double ) ) );
	connect( m_textEditor, SIGNAL( returnPressed() ), this, SLOT( accept() ) );
	connect( m_textEditor, SIGNAL( textChanged( const QString& ) ), this, SLOT( textChanged( const QString& ) ) );
	connect( m_editBasePath, SIGNAL( clicked() ), this, SLOT( editBasePath() ) );
	connect( m_convertToShapes, SIGNAL( clicked() ), this, SLOT( convertToShapes() ) );
	connect( this, SIGNAL( cancelClicked() ), this, SLOT( cancel() ) );

	setMainWidget( base );
	setFixedSize( baseSize() );
}

VTextOptionsWidget::~VTextOptionsWidget()
{
}

void
VTextOptionsWidget::valueChanged( int )
{
	m_fontCombo->setBold( m_boldCheck->isChecked() );
	m_fontCombo->setItalic( m_italicCheck->isChecked() );

	m_textEditor->setFont( QFont( m_fontCombo->currentText(), m_fontSize->value(), ( m_boldCheck->isChecked() ? 75 : 50 ), m_italicCheck->isChecked() ) );

	if( m_tool && isVisible() ) 
		m_tool->textChanged();
}

void
VTextOptionsWidget::valueChanged( double )
{
	if( m_tool && isVisible() ) 
		m_tool->textChanged();
}

void
VTextOptionsWidget::accept()
{
	if( m_tool )
		m_tool->accept();
	hide();
}

void
VTextOptionsWidget::cancel()
{
	if( m_tool )
		m_tool->cancel();
}

void
VTextOptionsWidget::textChanged( const QString& )
{
	if( m_tool && isVisible() )
		m_tool->textChanged();
}

void
VTextOptionsWidget::editBasePath()
{
	if( m_tool )
		m_tool->editBasePath();
}

void
VTextOptionsWidget::convertToShapes()
{
	if( m_tool )
		m_tool->convertToShapes();
}

void
VTextOptionsWidget::setFont( const QFont& font )
{
	m_fontCombo->setCurrentText( font.family() );

	m_boldCheck->setChecked( font.bold() );

	m_italicCheck->setChecked( font.italic() );

	m_fontSize->setValue( font.pointSize() );

	m_fontCombo->setBold( m_boldCheck->isChecked() );
	m_fontCombo->setItalic( m_italicCheck->isChecked() );

	m_textEditor->setFont( QFont( m_fontCombo->currentText(), m_fontSize->value(), ( m_boldCheck->isChecked() ? 75 : 50 ), m_italicCheck->isChecked() ) );
}

QFont VTextOptionsWidget::font()
{
	return QFont( m_fontCombo->currentText(), m_fontSize->value(), ( m_boldCheck->isChecked() ? 75 : 50 ), m_italicCheck->isChecked() );
}

void
VTextOptionsWidget::setText( const QString& text )
{
	m_textEditor->setText( text );
}

QString VTextOptionsWidget::text()
{
	return m_textEditor->text();
}

void
VTextOptionsWidget::setPosition( VText::Position position )
{
	m_textPosition->setCurrentItem( position );
}

VText::Position VTextOptionsWidget::position()
{
	return ( VText::Position ) m_textPosition->currentItem();
}

void
VTextOptionsWidget::setAlignment( VText::Alignment alignment )
{
	m_textAlignment->setCurrentItem( alignment );
}

VText::Alignment VTextOptionsWidget::alignment()
{
	return ( VText::Alignment ) m_textAlignment->currentItem();
}

void 
VTextOptionsWidget::setOffset( double offset )
{
	if( offset < 0.0 ) offset = 0.0;
	if( offset > 100.0 ) offset = 100.0;

	m_textOffset->setValue( offset );
}
double 
VTextOptionsWidget::offset()
{
	return m_textOffset->value();
}

void
VTextOptionsWidget::setUseShadow( bool state )
{
	m_shadow->setUseShadow( state );
}

bool VTextOptionsWidget::useShadow()
{
	return m_shadow->useShadow();
}

void
VTextOptionsWidget::setShadow( int angle, int distance, bool translucent )
{
	m_shadow->setShadowValues( angle, distance, translucent );
}

bool VTextOptionsWidget::translucentShadow()
{
	return m_shadow->isTranslucent();
}

int
VTextOptionsWidget::shadowAngle()
{
	return m_shadow->shadowAngle();
}

int
VTextOptionsWidget::shadowDistance()
{
	return m_shadow->shadowDistance();
}

void 
VTextOptionsWidget::initialize( VObject &text )
{
	if( m_tool )
		m_tool->visit( text );
}

VTextTool::VTextTool( KarbonView *view )
		: VTool( view, "tool_text" )
{
	m_optionsWidget = new VTextOptionsWidget( this, 0L );
	m_text = 0L;
	m_editedText = 0L;
	registerTool( this );
	m_cursor = new QCursor( VCursor::createCursor( VCursor::CrossHair ) );
}

VTextTool::~VTextTool()
{
	delete m_optionsWidget;
	delete m_editedText;
	delete m_cursor;
}

QString VTextTool::contextHelp()
{
	QString s = i18n( "<qt><b>Text Tool</b><br>" );
	s += i18n("<i>Click</i> on document to place horizontal text.<br>" );
	s += i18n("<i>Click and drag</i> in document to place directional text.<br>" );
	s += i18n("<i>Click</i> on a selected path object to place text along its outline.<br>" );
	s += i18n("<i>Click</i> on a selected text object to change it.<br></qt>" );

	return s;
}

void
VTextTool::activate()
{
	VTool::activate();
	view()->statusMessage()->setText( i18n( "Text Tool" ) );
	view()->setCursor( *m_cursor );

	m_creating = true;
	m_text = 0L;
	delete m_editedText;
	m_editedText = 0L;
}

void
VTextTool::deactivate()
{
}

void
VTextTool::draw( VPainter* painter )
{
	if( m_editedText )
		m_editedText->draw( painter, &m_editedText->boundingBox() );
}

void
VTextTool::drawPathCreation()
{
	VPainter * painter = view()->painterFactory()->editpainter();

	painter->setZoomFactor( view()->zoom() );

	// TODO rasterops need porting
	// painter->setRasterOp( Qt::NotROP );
	painter->newPath();
	painter->setPen( Qt::DotLine );
	painter->setBrush( Qt::NoBrush );

/* TODO moveto and lineto need porting
	painter->moveTo( first() );
	painter->lineTo( m_last );*/

	painter->strokePath();
}

void
VTextTool::drawEditedText()
{
	if( m_editedText ) 
		view()->repaintAll( m_editedText->boundingBox() );
}

void
VTextTool::mouseButtonPress()
{
	m_last = first();
	drawPathCreation();
	m_stepwise = false;
}

void
VTextTool::mouseButtonRelease()
{
	if( ! view() ) 
		return;

	VSelection* selection = view()->part()->document().selection();
	VObject* selObj = selection->objects().getFirst();

	// initialize dialog with single selected object
	if( selection->objects().count() == 1 && selObj->boundingBox().contains( last() ) )
		m_optionsWidget->initialize( *selObj );
	else 
	{
		// use a default horizontal path when just clicking
		VSubpath path( 0L );
		path.moveTo( first() );
		path.lineTo( QPointF( first().x()+10, first().y() ) );
	
		if( ! createText( path ) )
			return;
	}

	if( dynamic_cast<VText*>( selObj ) && selObj->boundingBox().contains( last() ) )
		m_optionsWidget->setCaption( i18n( "Change Text") );
	else 
		m_optionsWidget->setCaption( i18n( "Insert Text") );

	m_optionsWidget->show();
}

void
VTextTool::mouseDrag()
{
	drawPathCreation();

	if( m_stepwise && shiftPressed() )
	{
		QPointF act = last();
		QPointF dst = act - first();
 
		double angle = atan2( dst.y(), dst.x() );
		if( angle < 0 )
			angle += VGlobal::twopi;

		// calculate previuos and next modulo 45 degree step 
		double prevStep = angle - fmod( angle, VGlobal::pi_2 / 2.0f );
		double nextStep = prevStep + VGlobal::pi_2 / 2.0f;
		// calculate distance between first and last point
		double length = sqrt( dst.x()*dst.x() + dst.y()*dst.y() );

		// use nearest step
		if( angle - prevStep < nextStep - angle )
		{
			m_last.setX( first().x() + length * cos( prevStep ) );
			m_last.setY( first().y() + length * sin( prevStep ) );
		}
		else
		{
			m_last.setX( first().x() + length * cos( nextStep ) );
			m_last.setY( first().y() + length * sin( nextStep ) );
		}
	}
	else
		m_last = last();

	drawPathCreation();
}

void
VTextTool::mouseDragRelease()
{
	drawPathCreation();

	if( m_creating && m_editedText )
	{
		drawEditedText();
		delete m_editedText;
		m_editedText = 0L;
	}

	// use dragged path to create text along
	VSubpath path( 0L );
	path.moveTo( first() );
	path.lineTo( m_last );

	if( createText( path ) )
	{
		m_optionsWidget->setCaption( i18n( "Insert Text") );
		m_optionsWidget->show();
	}
}

bool
VTextTool::createText( VSubpath &path )
{
	// no original text is used
	m_text = 0L;
	delete m_editedText;

	m_editedText = new VText( m_optionsWidget->font(), path, m_optionsWidget->position(), m_optionsWidget->alignment(), m_optionsWidget->text() );
	
	if( ! m_editedText ) 
		return false;

	m_editedText->setState( VObject::edit );

#ifdef HAVE_KARBONTEXT
	m_editedText->traceText();
#endif

	// yes, we are creating a new text object
	m_creating = true;

	return true;
}

void
VTextTool::textChanged()
{
	if( !m_editedText )
		return;

	if( !m_creating && m_text && m_text->state() != VObject::hidden )
	{
		// hide the original text if we are changing it
		m_text->setState( VObject::hidden );
		view()->repaintAll( m_text->boundingBox() );
	}
	else
		view()->repaintAll( m_editedText->boundingBox() );

	m_editedText->setText( m_optionsWidget->text() );
	m_editedText->setFont( m_optionsWidget->font() );
	m_editedText->setPosition( m_optionsWidget->position() );
	m_editedText->setAlignment( m_optionsWidget->alignment() );
	m_editedText->setOffset( 0.01 * m_optionsWidget->offset()  );
#ifdef HAVE_KARBONTEXT
	m_editedText->traceText();
#endif

	drawEditedText();
}

void
VTextTool::accept()
{
	if( !m_editedText )
		return;

	VTextCmd* cmd;

	if( !m_creating )
	{
		cmd = new VTextCmd(
				  &view()->part()->document(),
				  i18n( "Change Text" ),
				  m_text,
				  m_editedText->font(),
				  m_editedText->basePath(),
				  m_editedText->position(),
				  m_editedText->alignment(),
				  m_editedText->offset(),
				  m_editedText->text(),
				  m_optionsWidget->useShadow(),
				  m_optionsWidget->shadowAngle(),
				  m_optionsWidget->shadowDistance(),
				  m_optionsWidget->translucentShadow() );
	}
	else
	{
		VText *newText = m_editedText->clone();
		newText->setUseShadow( m_optionsWidget->useShadow() );
		newText->setShadow( m_optionsWidget->shadowAngle(), m_optionsWidget->shadowDistance(), m_optionsWidget->translucentShadow() );

		cmd = new VTextCmd(
				  &view()->part()->document(),
				  i18n( "Insert Text" ),
				  newText );

		delete m_editedText;
		m_editedText = 0L;
	}

	view()->part()->addCommand( cmd, true );
	view()->part()->repaintAllViews();
	m_creating = false;
}

void
VTextTool::cancel()
{
	if( m_text )
	{
		// show original text if we canceled changing it
		m_text->setState( VObject::selected );
		view()->repaintAll( m_text->boundingBox() );
	}
	else
		drawPathCreation();

	delete m_editedText;
	m_editedText = 0L;
}

void
VTextTool::editBasePath()
{
	if( !m_editedText )
		return;

	view()->part()->document().selection()->clear();
	view()->part()->document().selection()->append( &m_editedText->basePath() );
	view()->part()->repaintAllViews();
}

void
VTextTool::convertToShapes()
{
	if( !m_text )
		return;

	VTextToCompositeCmd* cmd = new VTextToCompositeCmd(
								   &view()->part()->document(),
								   i18n( "Text Conversion" ),
								   m_text );

	view()->part()->addCommand( cmd, true );

	m_creating = false;

	delete m_editedText;

	m_text = 0L;
	m_editedText = 0L;
}

void
VTextTool::visitVPath( VPath& composite )
{
	if( composite.paths().count() == 0 )
		return;

	if( createText( *composite.paths().getFirst() ) )
		drawEditedText();
}

void
VTextTool::visitVSubpath( VSubpath& path )
{
	if( createText( path ) )
		drawEditedText();
}

void
VTextTool::visitVText( VText& text )
{
	m_text = &text;
	delete m_editedText;
	m_editedText = text.clone();

	m_optionsWidget->setFont( text.font() );
	m_optionsWidget->setText( text.text() );
	m_optionsWidget->setPosition( text.position() );
	m_optionsWidget->setAlignment( text.alignment() );
	m_optionsWidget->setOffset( text.offset() * 100.0 );
	m_optionsWidget->setUseShadow( text.useShadow() );
	m_optionsWidget->setShadow( text.shadowAngle(), text.shadowDistance(), text.translucentShadow() );
	m_creating = false;
	m_text->setState( VObject::hidden );
	m_editedText->setState( VObject::edit );
}

VTextTool::VTextCmd::VTextCmd( VDocument* doc, const QString& name, VText* text )
		: VCommand( doc, name, "14_text" ), m_text( text )
{
	m_textModifications = 0L;

	m_executed = false;
}

VTextTool::VTextCmd::VTextCmd( VDocument* doc, const QString& name, VText* text,
							   const QFont &newFont, const VSubpath& newBasePath, VText::Position newPosition, VText::Alignment newAlignment, double newOffset, const QString& newText,
							   bool newUseShadow, int newShadowAngle, int newShadowDistance, bool newTranslucentShadow )
		: VCommand( doc, name, "14_text" ), m_text( text )
{
	m_textModifications = new VTextModifPrivate();
	m_textModifications->newFont = newFont;
	m_textModifications->oldFont = text->font();
	m_textModifications->newBasePath = newBasePath;
	m_textModifications->oldBasePath = text->basePath();
	m_textModifications->newPosition = newPosition;
	m_textModifications->oldPosition = text->position();
	m_textModifications->newAlignment = newAlignment;
	m_textModifications->oldAlignment = text->alignment();
	m_textModifications->newOffset = newOffset;
	m_textModifications->oldOffset = text->offset();
	m_textModifications->newText = newText;
	m_textModifications->oldText = text->text();
	m_textModifications->newUseShadow = newUseShadow;
	m_textModifications->oldUseShadow = text->useShadow();
	m_textModifications->newShadowAngle = newShadowAngle;
	m_textModifications->oldShadowAngle = text->shadowAngle();
	m_textModifications->newShadowDistance = newShadowDistance;
	m_textModifications->oldShadowDistance = text->shadowDistance();
	m_textModifications->newTranslucentShadow = newTranslucentShadow;
	m_textModifications->oldTranslucentShadow = text->translucentShadow();

	m_executed = false;
}

VTextTool::VTextCmd::~VTextCmd()
{
	delete m_textModifications;
}

void
VTextTool::VTextCmd::execute()
{
	if( !m_text )
		return;

	if( !m_textModifications )
	{
		if( m_text->state() == VObject::deleted )
			m_text->setState( VObject::normal );
		else
		{
			m_text->setState( VObject::normal );
			document()->append( m_text );
			document()->selection()->clear();
			document()->selection()->append( m_text );
		}
	}
	else
	{
		m_text->setFont( m_textModifications->newFont );
		m_text->setBasePath( m_textModifications->newBasePath );
		m_text->setPosition( m_textModifications->newPosition );
		m_text->setAlignment( m_textModifications->newAlignment );
		m_text->setOffset( m_textModifications->newOffset );
		m_text->setText( m_textModifications->newText );
		m_text->setUseShadow( m_textModifications->newUseShadow );
		m_text->setShadow( m_textModifications->newShadowAngle, m_textModifications->newShadowDistance, m_textModifications->newTranslucentShadow );

#ifdef HAVE_KARBONTEXT
		m_text->traceText();
#endif

		m_text->setState( VObject::normal );
	}

	m_executed = true;

	setSuccess( true );
}

void
VTextTool::VTextCmd::unexecute()
{
	if( !m_text )
		return;

	if( !m_textModifications )
	{
		document()->selection()->take( *m_text );
		m_text->setState( VObject::deleted );
	}
	else
	{
		m_text->setFont( m_textModifications->oldFont );
		m_text->setBasePath( m_textModifications->oldBasePath );
		m_text->setPosition( m_textModifications->oldPosition );
		m_text->setAlignment( m_textModifications->oldAlignment );
		m_text->setOffset( m_textModifications->oldOffset );
		m_text->setText( m_textModifications->oldText );
		m_text->setUseShadow( m_textModifications->oldUseShadow );
		m_text->setShadow( m_textModifications->oldShadowAngle, m_textModifications->oldShadowDistance, m_textModifications->oldTranslucentShadow );

#ifdef HAVE_KARBONTEXT
		m_text->traceText();
#endif

		m_text->setState( VObject::normal );
	}

	m_executed = false;

	setSuccess( false );
}

VTextTool::VTextToCompositeCmd::VTextToCompositeCmd( VDocument* doc, const QString& name, VText* text )
		: VCommand( doc, name, "14_text" ), m_text( text ), m_group( 0L ), m_executed( false )
{
}

VTextTool::VTextToCompositeCmd::~VTextToCompositeCmd()
{
}

void
VTextTool::VTextToCompositeCmd::execute()
{
	if( !m_text )
		return;

	if( !m_group )
	{
		m_group = m_text->toVGroup();
		document()->append( m_group );
	}

	m_text->setState( VObject::deleted );
	m_group->setState( VObject::normal );
	document()->selection()->clear();
	document()->selection()->append( m_group );

	m_executed = true;

	setSuccess( true );
}

void
VTextTool::VTextToCompositeCmd::unexecute()
{
	if( !m_text )
		return;

	m_text->setState( VObject::normal );

	document()->selection()->take( *m_group );

	m_group->setState( VObject::deleted );

	m_executed = false;

	setSuccess( false );
}

bool
VTextTool::showDialog() const
{
	VSelection* selection = view()->part()->document().selection();

	// initialize dialog with single selected object
	if( selection->objects().count() == 1 )
		m_optionsWidget->initialize( *selection->objects().getFirst());
	else 
		return false;

	if( dynamic_cast<VText*>( selection->objects().getFirst() ) )
		m_optionsWidget->setCaption(i18n( "Change Text") );
	else 
		m_optionsWidget->setCaption(i18n( "Insert Text") );

	m_optionsWidget->show();
	return true;
}

void 
VTextTool::mouseDragShiftPressed()
{
	m_stepwise = true;
	mouseDrag();
}

void 
VTextTool::mouseDragShiftReleased()
{
	m_stepwise = false;
	mouseDrag();
}

void
VTextTool::setup( KActionCollection *collection )
{
	m_action = static_cast<KAction *>(collection -> action( name() ) );

	if( m_action == 0 )
	{
		m_action = new KAction( i18n( "Text Tool" ), "14_text", Qt::SHIFT+Qt::Key_T, this, SLOT( activate() ), collection, name() );
		m_action->setToolTip( i18n( "Text Tool" ) );
		// TODO needs porting: m_action->setExclusiveGroup( "misc" );
		//m_ownAction = true;
	}
}

#include "vtexttool.moc"

