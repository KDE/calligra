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

#include <qlayout.h>
#include <qcheckbox.h>
#include <qlineedit.h>
#include <qcombobox.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qcursor.h>

#include <kfontcombo.h>
#include <knuminput.h>
#include <kdebug.h>
#include <kiconloader.h>

#include "karbon_view.h"
#include "vpainterfactory.h"
#include "vtexttool.h"
#include "vselection.h"

VTextOptionsWidget::VTextOptionsWidget( VTextTool* tool, QWidget* parent )
		: QFrame( parent, "TextOptionsWidget" ), m_tool( tool )
{
	setFrameStyle( Box | Sunken );
	QGridLayout* mainLayout = new QGridLayout( this );
	mainLayout->setMargin( 3 );
	mainLayout->setSpacing( 2 );
	mainLayout->addMultiCellWidget( m_fontCombo = new KFontCombo( this ), 0, 0, 0, 3 );
	mainLayout->addWidget( m_fontSize = new KIntNumInput( this ), 1, 0 );
	mainLayout->addWidget( m_boldCheck = new QCheckBox( i18n( "Bold" ), this ), 1, 1 );
	mainLayout->addWidget( m_italicCheck = new QCheckBox( i18n( "Italic" ), this ), 1, 2 );
	mainLayout->addWidget( m_textPosition = new QComboBox( this ), 1, 3 );
	mainLayout->addMultiCellWidget( m_textEditor = new QLineEdit( this ), 2, 2, 0, 3 );
	mainLayout->addMultiCellWidget( m_editBasePath = new QPushButton( i18n( "Edit base path" ), this ), 3, 3, 0, 1 );
	mainLayout->addMultiCellWidget( m_convertToShapes = new QPushButton( i18n( "Convert to shapes" ), this ), 3, 3, 2, 3 );
	
	m_fontCombo->setCurrentText( "Helvetica" );
	m_fontSize->setValue( 12 );
	m_fontSize->setSuffix( " pt" );
	m_textEditor->setMinimumHeight( 100 );
	m_convertToShapes->setEnabled( false );
	 // TODO: Find a way to display correctly the following icons...
	m_textPosition->insertItem( SmallIcon( "14_text_above" ) );
	m_textPosition->insertItem( SmallIcon( "14_text_on" ) );
	m_textPosition->insertItem( SmallIcon( "14_text_under" ) );
	
	connect( m_fontCombo, SIGNAL( activated( int ) ), this, SLOT( valueChanged( int ) ) );
	connect( m_boldCheck, SIGNAL( stateChanged( int ) ), this, SLOT( valueChanged( int ) ) );
	connect( m_italicCheck, SIGNAL( stateChanged( int ) ), this, SLOT( valueChanged( int ) ) );
	connect( m_fontSize, SIGNAL( valueChanged( int ) ), this, SLOT( valueChanged( int ) ) );
	connect( m_textPosition, SIGNAL( activated( int ) ), this, SLOT( valueChanged( int ) ) );
	
	connect( m_textEditor, SIGNAL( returnPressed() ), this, SLOT( accept() ) );
	connect( m_textEditor, SIGNAL( textChanged( const QString& ) ), this, SLOT( textChanged( const QString& ) ) );
	
	connect( m_editBasePath, SIGNAL( clicked() ), this, SLOT( editBasePath() ) );
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
	if ( selection->objects().count() == 1 )
		visit( *selection->objects().getFirst() );
} // VTextTool::activate

void VTextTool::deactivate()
{
	if ( m_creating )
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
	if ( !m_editedText )
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
	
	if ( m_creating && m_editedText )
	{
		drawEditedText();
		delete m_editedText;
		m_editedText = 0L;
	}
	
	VPath path( 0L );
	path.moveTo( first() );
	path.lineTo( last() );
	m_text = 0L;
	m_editedText = new VText( m_optionsWidget->font(), path, m_optionsWidget->position(), m_optionsWidget->text() );
	m_editedText->setState( VObject::edit );
	m_editedText->traceText( view() );
	m_creating = true;
	
	drawEditedText();
} // VTextTool::mouseDragRelease

void VTextTool::textChanged()
{
	if ( !m_editedText )
		return;

	drawEditedText();
	
	m_editedText->setText( m_optionsWidget->text() );
	m_editedText->setFont( m_optionsWidget->font() );
	m_editedText->setPosition( m_optionsWidget->position() );
	m_editedText->traceText( view() );
	
	drawEditedText();
} // VTextTool::textChanged

void VTextTool::accept()
{
	if ( m_text )
	{
		m_text->setFont( m_editedText->font() );
		m_text->setBasePath( m_editedText->basePath() );
		m_text->setPosition( m_editedText->position() );
		m_text->setText( m_editedText->text() );
		m_text->traceText( view() );
		m_text->setParent( view()->part()->document().activeLayer() );
		m_text->setState( VObject::normal );
	}
	else
	{
		m_text = m_editedText->clone();
	} 

	VTextCmd* cmd = new VTextCmd(
		&view()->part()->document(),
		view(),
		( m_creating ? i18n( "Insert text" ) : i18n( "Change text" ) ),
		m_text );
	view()->part()->addCommand( cmd, true );
	view()->selectionChanged();

	delete m_editedText;
	m_creating = false;
	m_editedText = 0L;
	m_text = 0L;
} // VTextTool::accept

void VTextTool::cancel()
{
	drawPathCreation();
} // VTextTool::cancel

void VTextTool::editBasePath()
{
	if ( !m_editedText )
	view()->part()->document().selection()->clear();
	view()->part()->document().selection()->append( &m_editedText->basePath() );
} // VTextTool::editBasePath

void VTextTool::visitVComposite( VComposite& composite )
{
	if ( composite.paths().count() == 0 )
		return;

	m_text = 0L; 
	m_editedText = new VText( m_optionsWidget->font(), *composite.paths().getFirst(), m_optionsWidget->position(), m_optionsWidget->text() );
	m_editedText->setState( VObject::edit );
	m_editedText->traceText( view() );
	m_creating = true;

	drawEditedText();
} // VTextTool::visitVComposite

void VTextTool::visitVPath( VPath& path )
{
	m_text = 0L; 
	m_editedText = new VText( m_optionsWidget->font(), path, m_optionsWidget->position(), m_optionsWidget->text() );
	m_editedText->setState( VObject::edit );
	m_editedText->traceText( view() );
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
	m_creating = false;
	
	text.setState( VObject::hidden );
	view()->canvasWidget()->repaintAll( true );
	
	drawEditedText();
} // VTextTool::visitVText

VTextTool::VTextCmd::VTextCmd( VDocument* doc, KarbonView* view, const QString& name, VText* text )
		: VCommand( doc, name, "14_text" ), m_view( view ), m_text( text )
{
	m_textModifications = 0L;
	
	m_executed = false;
} // VTextTool::VTextCmd::VTextCmd

VTextTool::VTextCmd::VTextCmd( VDocument* doc, KarbonView* view, const QString& name, VText* text,
		const QFont &newFont, const VPath& newBasePath, VText::Position newPosition, const QString& newText )
		: VCommand( doc, name, "14_text" ), m_view( view ), m_text( text )
{
	m_textModifications = (VTextModifPrivate*)malloc( sizeof( VTextModifPrivate ) );
	m_textModifications->newFont     = newFont;
	m_textModifications->oldFont     = text->font();
	m_textModifications->newBasePath = newBasePath;
	m_textModifications->oldBasePath = text->basePath();
	m_textModifications->newPosition = newPosition;
	m_textModifications->oldPosition = text->position();
	m_textModifications->newText     = newText;
	m_textModifications->oldText     = text->text();
	
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
	
	if ( !m_textModifications )
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
		m_text->setText( m_textModifications->newText );
		m_text->traceText( m_view );
	}

	m_executed = true;
} // VTextTool::VTextCmd::execute

void VTextTool::VTextCmd::unexecute()
{
	if( !m_text )
		return;
	
	if ( !m_textModifications )
	{
		document()->selection()->take( *m_text );
		m_text->setState( VObject::deleted );
	}
	else
	{
		m_text->setFont( m_textModifications->oldFont );
		m_text->setBasePath( m_textModifications->oldBasePath );
		m_text->setPosition( m_textModifications->oldPosition );
		m_text->setText( m_textModifications->oldText );
		m_text->traceText( m_view );
	}

	m_executed = false;
} // VTextTool::VTextCmd::unexecute

VTextTool::VTextToCompositeCmd::VTextToCompositeCmd( VDocument* doc, const QString& name, VText* text )
		: VCommand( doc, name, "14_text" ), m_text( text )
{
	
} // VTextTool::VTextToCompositeCmd::VTextToCompositeCmd

VTextTool::VTextToCompositeCmd::~VTextToCompositeCmd()
{
} // VTextTool::VTextToCompositeCmd::~VTextToCompositeCmd

#include "vtexttool.moc"
