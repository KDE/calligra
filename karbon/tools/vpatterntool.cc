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

#include "vpatterntool.h"

#include <qtoolbutton.h>
#include <qframe.h>
#include <qhbuttongroup.h>
#include <qlayout.h>
#include <qfileinfo.h>
#include <qlabel.h>
#include <qcursor.h>

#include <kiconloader.h>
#include <koIconChooser.h>
#include <kfiledialog.h>

#include "karbon_factory.h"
#include "karbon_resourceserver.h"
#include "karbon_view.h"
#include "vpainter.h"
#include "vpainterfactory.h"
#include "vselection.h"
#include "vfillcmd.h"

VPatternWidget::VPatternWidget( QPtrList<KoIconItem>* patterns, VTool* tool, QWidget* parent )
		: QFrame( parent ), m_tool( tool )
{
	KIconLoader il;

	QVBoxLayout* layout = new QVBoxLayout( this );
	layout->addWidget( m_patternChooser = new KoIconChooser( QSize( 32, 32 ), this ) );
	layout->addWidget( m_buttonGroup = new QHButtonGroup( this ) );
	m_buttonGroup->insert( m_importPatternButton = new QToolButton( m_buttonGroup ) );
	m_buttonGroup->insert( m_deletePatternButton = new QToolButton( m_buttonGroup ) );
	m_patternChooser->setFixedSize( 180, 120 );
	m_importPatternButton->setIconSet( QPixmap( il.iconPath( "14_pattern_import.png", KIcon::Small ) ) );
	m_importPatternButton->setTextLabel( i18n( "Import" ) );
	m_deletePatternButton->setIconSet( QPixmap( il.iconPath( "14_pattern_delete.png", KIcon::Small ) ) );
	m_deletePatternButton->setTextLabel( i18n( "Delete" ) );

	m_buttonGroup->setInsideMargin( 3 );
	m_importPatternButton->setEnabled( true );
	m_deletePatternButton->setEnabled( false );

	setFrameStyle( Box | Sunken );
	layout->setMargin( 3 );

	connect( m_buttonGroup, SIGNAL( clicked( int ) ), this, SLOT( slotButtonClicked( int ) ) );
	connect( m_patternChooser, SIGNAL( selected( KoIconItem* ) ), this, SLOT( patternSelected( KoIconItem* ) ) );
	
	KoIconItem* item;
	for( item = patterns->first(); item; item = patterns->next() )
		m_patternChooser->addItem( item );
	m_pattern = (VPattern*)patterns->first();
} // VPatternWidget::VPatternWidget

VPatternWidget::~VPatternWidget()
{
} // VPatternWidget::~VPatternWidget

VPattern* VPatternWidget::selectedPattern()
{
	return m_pattern;
} // VPatternWidget::selectedPattern

void VPatternWidget::importPattern()
{
	VPattern* pattern = KarbonFactory::rServer()->addPattern( KFileDialog::getOpenFileName( QString::null, "*.jpg *.gif *.png", this, i18n( "Choose the pattern to add" ) ) );
	if( pattern )
		m_patternChooser->addItem( pattern );
} // VPatternWidget::importPattern

void VPatternWidget::deletePattern()
{
	m_patternChooser->removeItem( m_pattern );
	KarbonFactory::rServer()->removePattern( m_pattern );
	m_patternChooser->updateContents();
} // VPatternWidget::deletePattern

void VPatternWidget::slotButtonClicked( int id )
{
	switch( id )
	{
		case 0: importPattern();
				break;
		case 1: deletePattern();
				break;
	} 
} // VPatternWidget::slotButtonClicked

void VPatternWidget::patternSelected( KoIconItem* item )
{
	m_pattern = (VPattern*)item;
	m_deletePatternButton->setEnabled( QFileInfo( m_pattern->tilename() ).isWritable() );
} // VPatternWidget::patternSelected

VPatternTool::VPatternTool( KarbonView* view )
	: VTool( view )
{
	m_optionsWidget = new VPatternWidget( &KarbonFactory::rServer()->patterns(), this );
} // VPatternTool::VPatternTool

VPatternTool::~VPatternTool()
{
	delete m_optionsWidget;
} // VPatternTool::~VPatternTool

void
VPatternTool::activate()
{
	view()->statusMessage()->setText( i18n( "Pattern" ) );
	view()->canvasWidget()->viewport()->setCursor( QCursor( Qt::crossCursor ) );
}

QString VPatternTool::contextHelp()
{
	QString s = i18n( "<qt><b>Pattern tool:</b><br>" );
	s += i18n( "<i>Click</i> on the pattern you want in the chooser.<br>" );
	s += i18n( "<i>Click and drag</i> to choose the pattern vector.</qt>" );
	return s;
} // VPatternTool::contextHelp

void VPatternTool::draw()
{
	VPainter *painter = view()->painterFactory()->editpainter();
	view()->canvasWidget()->setYMirroring( true );
	painter->setZoomFactor( view()->zoom() );
	painter->setRasterOp( Qt::NotROP );

	painter->setPen( Qt::DotLine );
	painter->newPath();
	painter->moveTo( first() );
	painter->lineTo( m_current );
	painter->strokePath();
} // VPatternTool::draw

void
VPatternTool::mouseDrag()
{
	draw();

	m_current = last();

	draw();
} // VPatternTool::mouseDrag

void
VPatternTool::mouseButtonPress()
{
	m_current = first();
} // VPatternTool::mouseButtonPress

void
VPatternTool::mouseButtonRelease()
{
	if( view()->part()->document().selection()->objects().count() == 0 ) 
		return;
	
	VPattern pattern = *m_optionsWidget->selectedPattern();
	pattern.setOrigin( first() );
	pattern.setVector( KoPoint( first().x() + 10, first().y() ) );

	/*if( target == fill )
	{*/
		VFill fill;
		fill.pattern() = pattern;
		fill.setType( VFill::patt );
		view()->part()->addCommand(
			new VFillCmd( &view()->part()->document(), fill, "14_pattern" ), true );
/*	}
	else
	{
		VStroke stroke;
		stroke.pattern() = pattern;
		stroke.setType( VStroke::patt );
		view()->part()->addCommand(
			new VStrokeCmd( &view()->part()->document(), &stroke, "14_pattern" ), true );
	}*/

	view()->selectionChanged();
} // VPatternTool::mouseButtonRelease

void
VPatternTool::mouseDragRelease()
{
	if( view()->part()->document().selection()->objects().count() == 0 )
	{
		draw();
		return;
	}
	
	VPattern pattern = *m_optionsWidget->selectedPattern();
	pattern.setOrigin( first() );
	pattern.setVector( KoPoint( last().x(), first().y() + first().y() - last().y() ) );

/*	if( target == fill )
	{*/
		VFill fill;
		fill.pattern() = pattern;
		fill.setType( VFill::patt );
		view()->part()->addCommand(
			new VFillCmd( &view()->part()->document(), fill, "14_pattern" ), true );
/*	}
	else
	{
		VStroke stroke;
		stroke.pattern() = pattern;
		stroke.setType( VStroke::patt );
		view()->part()->addCommand(
			new VStrokeCmd( &view()->part()->document(), &stroke, "14_pattern" ), true );
	}*/

	view()->selectionChanged();
} // VPatternTool::mouseDragRelease


#include "vpatterntool.moc"
