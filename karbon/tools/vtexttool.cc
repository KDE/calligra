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

#include <math.h>

#include <qlayout.h>
#include <qcheckbox.h>
#include <qlineedit.h>
#include <qcombobox.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qtabwidget.h>
#include <qcursor.h>
#include <qpixmap.h>
#include <qpainter.h>

#include <kfontcombo.h>
#include <knuminput.h>
#include <kdebug.h>
#include <kiconloader.h>

#include "karbon_view.h"
#include "vpainterfactory.h"
#include "vtexttool.h"
#include "vselection.h"
#include "vkopainter.h"

static void traceShape( VKoPainter* p, int x, int y, int w, int h )
{
	p->newPath();
	p->moveTo( KoPoint( x + w    , y + h     ) );
	p->lineTo( KoPoint( x + w / 3, y + h     ) );
	p->lineTo( KoPoint( x + w / 3, y + h / 3 ) );
	p->lineTo( KoPoint( x + w    , y + h / 3 ) );
	p->lineTo( KoPoint( x + w    , y + h     ) );
	
	p->moveTo( KoPoint( x                , y                 ) );
	p->lineTo( KoPoint( x + ( w / 3 ) * 2, y                 ) );
	p->lineTo( KoPoint( x + ( w / 3 ) * 2, y + ( h / 3 ) * 2 ) );
	p->lineTo( KoPoint( x                , y + ( h / 3 ) * 2 ) );
	p->lineTo( KoPoint( x                , y                 ) );
	
} // traceShape [static]

ShadowPreview::ShadowPreview( ShadowWidget* parent )
		: QWidget( parent ), m_parent( parent )
{
	setMinimumSize( 60, 60 );

	connect( this, SIGNAL( changed( int, int, bool ) ), m_parent, SLOT( setShadowValues( int, int, bool ) ) );
} // ShadowPreview::ShadowPreview

ShadowPreview::~ShadowPreview()
{
} // ShadowPreview::~ShadowPreview

void ShadowPreview::mouseReleaseEvent( QMouseEvent* e )
{
	int dx = e->x() - width() / 2;
	int dy = e->y() - height() / 2;

	float fd = sqrt( dx * dx + dy * dy );
	int a;
	if ( fd == 0 )
		a = 0;
	else if ( dy == 0 && dx < 0 )
		a = 180;
	else
	{
		float r = acos( dx / fd );
		a = ( dy <= 0 ? r : 6.2832 - r ) / 6.2832 * 360.;
	}

	emit changed( a, (int)fd, m_parent->isTranslucent() );
} // ShadowPreview::mouseReleaseEvent

void ShadowPreview::paintEvent( QPaintEvent* )
{
	int w = width() - 4;
	int h = height() - 4;
	int d = m_parent->shadowDistance();
	int a = 6.2832 - m_parent->shadowAngle();

	QPixmap pm( w, h );
	VKoPainter p( &pm, w, h );
	VColor color( VColor::rgb );
	
	VFill fill;
	KIconLoader il;
	fill.pattern() = VPattern( il.iconPath( "karbon.png", KIcon::Small ) );
	fill.setType( VFill::patt );
	p.newPath();
	p.moveTo( KoPoint( 0, 0 ) );
	p.lineTo( KoPoint( 0, h ) );
	p.lineTo( KoPoint( w, h ) );
	p.lineTo( KoPoint( w, 0 ) );
	p.lineTo( KoPoint( 0, 0 ) );
	p.setBrush( fill );
	p.fillPath();
	color.set( 1., 1., 1. );
	color.setOpacity( .5 );
	p.setBrush( VFill( color ) );
	p.fillPath();
	
	if ( m_parent->isTranslucent() )
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
	traceShape( &p, w / 4 + d * cos( a / 360. * 6.2832 ), h / 4 + d * sin( a / 360. * 6.2832 ), w / 2, h / 2 );
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

	if ( !m_parent->useShadow() )
	{
		p.newPath();
		p.moveTo( KoPoint( 0, 0 ) );
		p.lineTo( KoPoint( 0, h ) );
		p.lineTo( KoPoint( w, h ) );
		p.lineTo( KoPoint( w, 0 ) );
		p.lineTo( KoPoint( 0, 0 ) );
		VColor c( colorGroup().background() );
		c.setOpacity( .8 );
		p.setBrush( VFill( c ) );
		p.fillPath();
	}
	p.end();
	
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
	painter.drawLine( width() / 2, height() / 2 - 2, width() / 2, height() / 2 + 2 );

} // ShadowPreview::paintEvent

ShadowWidget::ShadowWidget( QWidget* parent, const char* name, int angle, int distance, bool translucent )
		: QGroupBox( parent, name )
{
	setTitle( i18n( "Shadow" ) );

	QGridLayout* layout = new QGridLayout( this );
	layout->addRowSpacing( 0, 12 );
	layout->setMargin( 3 );
	layout->setSpacing( 2 );
	layout->addMultiCellWidget( m_preview = new ShadowPreview( this ), 1, 3, 0, 1 );
	layout->addWidget( new QLabel( i18n( "Angle:" ), this ), 1, 2 );
	layout->addWidget( m_angle = new KIntNumInput( this ), 1, 3 );
	layout->addWidget( new QLabel( i18n( "Distance:" ), this ), 2, 2 );
	layout->addWidget( m_distance = new KIntNumInput( this ), 2, 3 );
	QHBoxLayout* cbLayout = new QHBoxLayout( layout );
	layout->addMultiCell( cbLayout, 3, 3, 2, 3 );
	cbLayout->add( m_translucent = new QCheckBox( i18n( "Draw translucent shadow" ), this ) );
	cbLayout->add( m_useShadow = new QCheckBox( i18n( "Shadow" ), this ) );
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
} // ShadowWidget::ShadowWidget 

ShadowWidget::~ShadowWidget()
{
} // ShadowWidget::ShadowWidget

void ShadowWidget::setUseShadow( bool use )
{
	m_useShadow->setChecked( use );
	m_preview->repaint();
} // ShadowWidget::setUseShadow

bool ShadowWidget::useShadow()
{
	return m_useShadow->isChecked();
} // ShadowWidget::useShadow

void ShadowWidget::setShadowAngle( int angle )
{
	m_angle->setValue( angle );
	m_preview->repaint();
} // ShadowWidget::setShadowAngle

int ShadowWidget::shadowAngle()
{
	return m_angle->value();
} // ShadowWidget::shadowAngle

void ShadowWidget::setShadowDistance( int distance )
{
	m_distance->setValue( distance );
	m_preview->repaint();
} // ShadowWidget::setShadowDistance

int ShadowWidget::shadowDistance()
{
	return m_distance->value();
} // ShadowWidget::shadowDistance

void ShadowWidget::setTranslucent( bool translucent )
{
	m_translucent->setChecked( translucent );
	m_preview->repaint();
} // ShdowWidget::setTranslucent

bool ShadowWidget::isTranslucent()
{
	return m_translucent->isChecked();
} // ShadowWidget::isTranslucent

void ShadowWidget::setShadowValues( int angle, int distance, bool translucent ) 
{
	m_angle->setValue( angle );
	m_distance->setValue( distance );
	m_translucent->setChecked( translucent );
	m_preview->repaint();
} // ShadowWidget::setShadowValues

void ShadowWidget::updatePreview( int )
{
	m_preview->repaint();
} // ShadowWidget::updatePreview

void ShadowWidget::updatePreview()
{
	m_preview->repaint();
	bool ok = m_useShadow->isChecked();
	m_angle->setEnabled( ok );
	m_distance->setEnabled( ok ); 
	m_translucent->setEnabled( ok );
} // ShadowWidget::updatePreview

VTextOptionsWidget::VTextOptionsWidget( VTextTool* tool, QWidget* parent )
		: QFrame( parent, "TextOptionsWidget" ), m_tool( tool )
{
	setFrameStyle( Box | Sunken );
	QVBoxLayout* mainLayout = new QVBoxLayout( this );
	mainLayout->setMargin( 3 );
	mainLayout->add( m_tabWidget = new QTabWidget( this ) );
	m_tabWidget->setFont( QFont( "helvetica" , 8 ) );

	QWidget* textWidget = new QWidget( m_tabWidget );
	QGridLayout* textLayout = new QGridLayout( textWidget );
	textLayout->setMargin( 3 );
	textLayout->setSpacing( 2 );
	textLayout->addMultiCellWidget( m_fontCombo = new KFontCombo( textWidget ), 0, 0, 0, 2 );
	textLayout->addWidget( m_fontSize = new KIntNumInput( textWidget ), 1, 0 );
	textLayout->addWidget( m_boldCheck = new QCheckBox( i18n( "Bold" ), textWidget ), 1, 1 );
	textLayout->addWidget( m_italicCheck = new QCheckBox( i18n( "Italic" ), textWidget ), 1, 2 );
	textLayout->addMultiCellWidget( m_textEditor = new QLineEdit( textWidget ), 2, 2, 0, 2 );
	m_tabWidget->addTab( textWidget, i18n( "Text" ) );
	
	QWidget* fxWidget = new QWidget( m_tabWidget );
	QGridLayout* fxLayout = new QGridLayout( fxWidget );
	fxLayout->setMargin( 3 );
	fxLayout->setSpacing( 2 );
	fxLayout->addMultiCellWidget( m_shadow = new ShadowWidget( fxWidget, 0L, 315, 4, true ), 0, 0, 0, 3 );
	fxLayout->addWidget( new QLabel( i18n( "Alignment:" ), fxWidget ), 1, 0 );
	fxLayout->addWidget( m_textAlignment = new QComboBox( fxWidget ), 1, 1 );
	fxLayout->addWidget( new QLabel( i18n( "Position:" ), fxWidget ), 1, 2 );
	fxLayout->addWidget( m_textPosition = new QComboBox( fxWidget ), 1, 3 );
	fxLayout->addMultiCellWidget( m_editBasePath = new QPushButton( i18n( "Edit base path" ), fxWidget ), 2, 2, 0, 1 );
	fxLayout->addMultiCellWidget( m_convertToShapes = new QPushButton( i18n( "Convert to shapes" ), fxWidget ), 2, 2, 2, 3 );
	m_tabWidget->addTab( fxWidget, i18n( "Effects" ) );
	
	m_fontCombo->setCurrentText( "Helvetica" );
	m_fontSize->setValue( 12 );
	m_fontSize->setSuffix( " pt" );
	m_textEditor->setMinimumHeight( 100 );
	m_convertToShapes->setEnabled( true );
	 // TODO: Find a way to display correctly the following icons...
	m_textAlignment->insertItem( "Left" );
	m_textAlignment->insertItem( "Center" );
	m_textAlignment->insertItem( "Right" );
	m_textPosition->insertItem( SmallIcon( "14_text_above" ) );
	m_textPosition->insertItem( SmallIcon( "14_text_on" ) );
	m_textPosition->insertItem( SmallIcon( "14_text_under" ) );
	
	connect( m_fontCombo, SIGNAL( activated( int ) ), this, SLOT( valueChanged( int ) ) );
	connect( m_boldCheck, SIGNAL( stateChanged( int ) ), this, SLOT( valueChanged( int ) ) );
	connect( m_italicCheck, SIGNAL( stateChanged( int ) ), this, SLOT( valueChanged( int ) ) );
	connect( m_fontSize, SIGNAL( valueChanged( int ) ), this, SLOT( valueChanged( int ) ) );
	connect( m_textPosition, SIGNAL( activated( int ) ), this, SLOT( valueChanged( int ) ) );
	connect( m_textAlignment, SIGNAL( activated( int ) ), this, SLOT( valueChanged( int ) ) );
	
	connect( m_textEditor, SIGNAL( returnPressed() ), this, SLOT( accept() ) );
	connect( m_textEditor, SIGNAL( textChanged( const QString& ) ), this, SLOT( textChanged( const QString& ) ) );
	
	connect( m_editBasePath, SIGNAL( clicked() ), this, SLOT( editBasePath() ) );
	connect( m_convertToShapes, SIGNAL( clicked() ), this, SLOT( convertToShapes() ) );
} // VTextOptionsWidget::VTextOptionsWidget

VTextOptionsWidget::~VTextOptionsWidget()
{
} // VTextOptionsWidget::~VTextOptionsWidget

void VTextOptionsWidget::valueChanged( int )
{
	m_fontCombo->setBold( m_boldCheck->isChecked() );
	m_fontCombo->setItalic( m_italicCheck->isChecked() );
	m_textEditor->setFont( QFont( m_fontCombo->currentText(), m_fontSize->value(), ( m_boldCheck->isChecked() ? 75 : 50 ), m_italicCheck->isChecked() ) );
	m_tool->textChanged();
} // VTextOptionsWidget::valueChanged

void VTextOptionsWidget::accept()
{
	m_tool->accept();
} // VTextOptionsWidget::accept

void VTextOptionsWidget::textChanged( const QString& )
{
	m_tool->textChanged();
} // VTextOptionsWidget::textChanged

void VTextOptionsWidget::editBasePath()
{
	m_tool->editBasePath();
} // VTextOptionsWidget::editBasePath

void VTextOptionsWidget::convertToShapes()
{
	m_tool->convertToShapes();
} // VTextOptionsWidget::convertToShapes

void VTextOptionsWidget::setFont( const QFont& font )
{
	m_fontCombo->setCurrentText( font.family() );
	m_boldCheck->setChecked( font.bold() );
	m_italicCheck->setChecked( font.italic() );
	m_fontSize->setValue( font.pointSize() );
	m_fontCombo->setBold( m_boldCheck->isChecked() );
	m_fontCombo->setItalic( m_italicCheck->isChecked() );
	m_textEditor->setFont( QFont( m_fontCombo->currentText(), m_fontSize->value(), ( m_boldCheck->isChecked() ? 75 : 50 ), m_italicCheck->isChecked() ) );
} // VTextOptionsWidget::setFont

QFont VTextOptionsWidget::font()
{
	return QFont( m_fontCombo->currentText(), m_fontSize->value(), ( m_boldCheck->isChecked() ? 75 : 50 ), m_italicCheck->isChecked() );
} // VTextOptionsWidget::font

void VTextOptionsWidget::setText( const QString& text )
{
	m_textEditor->setText( text );
} // VTextOptionsWidget::setText

QString VTextOptionsWidget::text()
{
	return m_textEditor->text();
} // VTextOptionsWidget::text

void VTextOptionsWidget::setPosition( VText::Position position )
{
	m_textPosition->setCurrentItem( position );
} // VTextOptionsWidget::setPosition

VText::Position VTextOptionsWidget::position()
{
	return (VText::Position)m_textPosition->currentItem();
} // VTextOptionsWidget::position

void VTextOptionsWidget::setAlignment( VText::Alignment alignment )
{
	m_textAlignment->setCurrentItem( alignment );
} // VTextOptionsWidget::setAlignment

VText::Alignment VTextOptionsWidget::alignment()
{
	return (VText::Alignment)m_textAlignment->currentItem();
} // VTextOptionsWidget::alignment

void VTextOptionsWidget::setUseShadow( bool state )
{
	m_shadow->setUseShadow( state );
} // VTextOptionsWidget::setUseShadow

bool VTextOptionsWidget::useShadow()
{
	return m_shadow->useShadow();
} // VTextOptionsWidget::useShadow

void VTextOptionsWidget::setShadow( int angle, int distance, bool translucent )
{
	m_shadow->setShadowValues( angle, distance, translucent );
} // VTextOptionsWidget::setShadow

bool VTextOptionsWidget::translucentShadow()
{
	return m_shadow->isTranslucent();
} // VTextOptionsWidget::translucentShadow

int VTextOptionsWidget::shadowAngle()
{
	return m_shadow->shadowAngle();
} // VTextOptionsWidget::shadowAngle

int VTextOptionsWidget::shadowDistance()
{
	return m_shadow->shadowDistance();
} // VTextOptionsWidget::shadowDistance

VTextTool::VTextTool( KarbonView* view )
		: VTool( view )
{
	m_optionsWidget = new VTextOptionsWidget( this );
	m_text = 0L;
	m_editedText = 0L;
} // VTextTool::VTextTool

VTextTool::~VTextTool()
{
	delete m_optionsWidget;
} // VTextTool::~VTextTool

QString VTextTool::contextHelp()
{
	QString s = "<qt><b>Text tool</b><br>";
	s += "<i>Type</i> your text and <i>click and drag</i> to place it.<br>";
	s += "If the selection made before the tool activation was a text object, it is edited.<br>";
	s += "If the selection was a path, the text is drawn along it.<br>";
	s += "Press <i>Return</i> to validate your text.</qt>";
	return s;
} // VTextTool::contextHelp

void VTextTool::activate()
{
	view()->statusMessage()->setText( i18n( "Text Tool" ) );
	view()->canvasWidget()->viewport()->setCursor( QCursor( Qt::crossCursor ) );

	m_creating = true;
	delete m_text;
	delete m_editedText;

	VSelection* selection = view()->part()->document().selection();
	kdDebug() << "Nb objects selected: " << selection->objects().count() << endl;
	if( selection->objects().count() == 1 )
		visit( *selection->objects().getFirst() );
} // VTextTool::activate

void VTextTool::deactivate()
{
	if( m_creating )
		delete m_text;
} // VTextTool::deactivate()

void VTextTool::drawPathCreation()
{
	VPainter* painter = view()->painterFactory()->editpainter();

	view()->canvasWidget()->setYMirroring( true );
	painter->setZoomFactor( view()->zoom() );

	painter->setRasterOp( Qt::NotROP );
	painter->newPath();
	painter->setPen( Qt::DotLine );
	painter->setBrush( Qt::NoBrush );

	painter->moveTo( first() ); 
	painter->lineTo( m_last );
	painter->strokePath();
} // VTextTool::drawPathCreation

void VTextTool::drawEditedText()
{
	if( !m_editedText )
		return;

	kdDebug() << "Drawing: " << m_editedText->text() << endl;
	VPainter* painter = view()->painterFactory()->editpainter();

	view()->canvasWidget()->setYMirroring( true );
	painter->setZoomFactor( view()->zoom() );

	m_editedText->draw( painter );
} // VTextTool::drawEditedText

void VTextTool::mouseButtonPress()
{
	m_last = first();
	drawPathCreation();
} // VTextTool::mouseButtonPress

void VTextTool::mouseButtonRelease()
{
	cancel();
} // VTextTool::mouseButtonRelease

void VTextTool::mouseDrag()
{
	drawPathCreation();
	
	m_last = last();
	
	drawPathCreation();
} // VTextTool::mouseDrag

void VTextTool::mouseDragRelease()
{
	drawPathCreation();
	
	if( m_creating && m_editedText )
	{
		drawEditedText();
		delete m_editedText;
		m_editedText = 0L;
	}
	
	VPath path( 0L );
	path.moveTo( first() );
	path.lineTo( last() );
	m_text = 0L;
	m_editedText = new VText( m_optionsWidget->font(), path, m_optionsWidget->position(), m_optionsWidget->alignment(), m_optionsWidget->text() );
	m_editedText->setState( VObject::edit );
	m_editedText->traceText();
	m_creating = true;
	
	drawEditedText();
} // VTextTool::mouseDragRelease

void VTextTool::textChanged()
{
	
	if( !m_editedText )
		return;

	if( !m_creating && m_text && m_text->state() != VObject::hidden )
	{
		m_text->setState( VObject::hidden );
		view()->canvasWidget()->repaintAll( true );
	}
	else
		drawEditedText();
	
	m_editedText->setText( m_optionsWidget->text() );
	m_editedText->setFont( m_optionsWidget->font() );
	m_editedText->setPosition( m_optionsWidget->position() );
	m_editedText->setAlignment( m_optionsWidget->alignment() );
	m_editedText->traceText();
	
	drawEditedText();
} // VTextTool::textChanged

void VTextTool::accept()
{
	VTextCmd* cmd;
	
	if( !m_creating )
	{
		cmd = new VTextCmd(
			&view()->part()->document(),
			( m_creating ? i18n( "Insert text" ) : i18n( "Change text" ) ),
			m_text,
			m_editedText->font(),
			m_editedText->basePath(),
			m_editedText->position(),
			m_editedText->alignment(),
			m_editedText->text(),
			m_optionsWidget->useShadow(),
			m_optionsWidget->shadowAngle(), 
			m_optionsWidget->shadowDistance(), 
			m_optionsWidget->translucentShadow() );
	}
	else
	{
		m_text = m_editedText->clone();
		m_text->setUseShadow( m_optionsWidget->useShadow() );
		m_text->setShadow( m_optionsWidget->shadowAngle(), m_optionsWidget->shadowDistance(), m_optionsWidget->translucentShadow() );
		cmd = new VTextCmd(
			&view()->part()->document(),
			( m_creating ? i18n( "Insert text" ) : i18n( "Change text" ) ),
			m_text );
	} 
	
	view()->part()->addCommand( cmd, true );
	view()->selectionChanged();

	m_creating = false;
} // VTextTool::accept

void VTextTool::cancel()
{
	drawPathCreation();
} // VTextTool::cancel

void VTextTool::editBasePath()
{
	if( !m_editedText )
		return;
	view()->part()->document().selection()->clear();
	view()->part()->document().selection()->append( &m_editedText->basePath() );
} // VTextTool::editBasePath

void VTextTool::convertToShapes()
{
	if( !m_text )
		return;

	VTextToCompositeCmd* cmd = new VTextToCompositeCmd(
		&view()->part()->document(),
		i18n( "Text conversion" ),
		m_text );
	view()->part()->addCommand( cmd, true );
	view()->selectionChanged();

	m_creating = false;
	delete m_editedText;
	m_text = 0L;
	m_editedText = 0L;
} // VTextTool::convertToShapes

void VTextTool::visitVComposite( VComposite& composite )
{
	if( composite.paths().count() == 0 )
		return;

	m_text = 0L; 
	m_editedText = new VText( m_optionsWidget->font(), *composite.paths().getFirst(), m_optionsWidget->position(), m_optionsWidget->alignment(), m_optionsWidget->text() );
	m_editedText->setState( VObject::edit );
	m_editedText->traceText();
	m_creating = true;

	drawEditedText();
} // VTextTool::visitVComposite

void VTextTool::visitVPath( VPath& path )
{
	m_text = 0L; 
	m_editedText = new VText( m_optionsWidget->font(), path, m_optionsWidget->position(), m_optionsWidget->alignment(), m_optionsWidget->text() );
	m_editedText->setState( VObject::edit );
	m_editedText->traceText();
	m_creating = true;

	drawEditedText();
} // VTextTool::visitVPath

void VTextTool::visitVText( VText& text )
{
	m_text = &text;
	m_editedText = text.clone();
	m_optionsWidget->setFont( text.font() );
	m_optionsWidget->setText( text.text() );
	m_optionsWidget->setPosition( text.position() );
	m_optionsWidget->setAlignment( text.alignment() );
	m_creating = false;
} // VTextTool::visitVText

VTextTool::VTextCmd::VTextCmd( VDocument* doc, const QString& name, VText* text )
		: VCommand( doc, name, "14_text" ), m_text( text )
{
	m_textModifications = 0L;
	
	m_executed = false;
} // VTextTool::VTextCmd::VTextCmd

VTextTool::VTextCmd::VTextCmd( VDocument* doc, const QString& name, VText* text,
		const QFont &newFont, const VPath& newBasePath, VText::Position newPosition, VText::Alignment newAlignment, const QString& newText,
		bool newUseShadow, int newShadowAngle, int newShadowDistance, bool newTranslucentShadow )
		: VCommand( doc, name, "14_text" ), m_text( text )
{
	m_textModifications = new VTextModifPrivate();
	m_textModifications->newFont              = newFont;
	m_textModifications->oldFont              = text->font();
	m_textModifications->newBasePath          = newBasePath;
	m_textModifications->oldBasePath          = text->basePath();
	m_textModifications->newPosition          = newPosition;
	m_textModifications->oldPosition          = text->position();
	m_textModifications->newAlignment         = newAlignment;
	m_textModifications->oldAlignment         = text->alignment();
	m_textModifications->newText              = newText;
	m_textModifications->oldText              = text->text();
	m_textModifications->newUseShadow         = newUseShadow;
	m_textModifications->oldUseShadow         = text->useShadow();
	m_textModifications->newShadowAngle       = newShadowAngle;
	m_textModifications->oldShadowAngle       = text->shadowAngle();
	m_textModifications->newShadowDistance    = newShadowDistance;
	m_textModifications->oldShadowDistance    = text->shadowDistance();
	m_textModifications->newTranslucentShadow = newTranslucentShadow;
	m_textModifications->oldTranslucentShadow = text->translucentShadow();
	
	m_executed = false;
} // VTextTool::VTextCmd::VTextCmd

VTextTool::VTextCmd::~VTextCmd()
{
	delete m_textModifications;
} // VTextTool::VTextCmd::~VTextCmd

void VTextTool::VTextCmd::execute()
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
		m_text->setText( m_textModifications->newText );
		m_text->setUseShadow( m_textModifications->newUseShadow );
		m_text->setShadow( m_textModifications->newShadowAngle, m_textModifications->newShadowDistance, m_textModifications->newTranslucentShadow );
		m_text->traceText();
		m_text->setState( VObject::normal );
	}

	m_executed = true;
} // VTextTool::VTextCmd::execute

void VTextTool::VTextCmd::unexecute()
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
		m_text->setText( m_textModifications->oldText );
		m_text->setUseShadow( m_textModifications->oldUseShadow );
		m_text->setShadow( m_textModifications->oldShadowAngle, m_textModifications->oldShadowDistance, m_textModifications->oldTranslucentShadow );
		m_text->traceText();
		m_text->setState( VObject::normal );
	}

	m_executed = false;
} // VTextTool::VTextCmd::unexecute

VTextTool::VTextToCompositeCmd::VTextToCompositeCmd( VDocument* doc, const QString& name, VText* text )
		: VCommand( doc, name, "14_text" ), m_text( text ), m_group( 0L ), m_executed( false )
{
} // VTextTool::VTextToCompositeCmd::VTextToCompositeCmd

VTextTool::VTextToCompositeCmd::~VTextToCompositeCmd()
{
} // VTextTool::VTextToCompositeCmd::~VTextToCompositeCmd

void VTextTool::VTextToCompositeCmd::execute()
{
	if ( !m_text )
		return;

	if ( !m_group )
	{
		m_group = m_text->toVGroup();
		document()->append( m_group );
	}
		
	m_text->setState( VObject::deleted );
	m_group->setState( VObject::normal );
	document()->selection()->clear();
	document()->selection()->append( m_group );

	m_executed = true;
} // VTextTool::VTextToCompositeCmd::execute

void VTextTool::VTextToCompositeCmd::unexecute()
{
	if ( !m_text )
		return;

	m_text->setState( VObject::normal );
	document()->selection()->take( *m_group );
	m_group->setState( VObject::deleted );

	m_executed = false;
} // VTextTool::VTextToCompositeCmd::unexecute

#include "vtexttool.moc"
