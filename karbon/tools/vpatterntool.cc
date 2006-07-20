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

#include "vpatterntool.h"

#include <qtoolbutton.h>
#include <q3frame.h>
#include <Q3ButtonGroup>
#include <QLayout>
#include <qfileinfo.h>
#include <QLabel>
#include <qcursor.h>
#include <QRectF>
#include <QPointF>
#include <Q3PtrList>
#include <QVBoxLayout>

#include <kiconloader.h>
#include <KoResourceChooser.h>
#include <kfiledialog.h>
#include <kmessagebox.h>

#include <karbon_factory.h>
#include <karbon_resourceserver.h>
#include <karbon_view.h>
#include <karbon_part.h>
#include <render/vpainter.h>
#include <render/vpainterfactory.h>
#include <core/vselection.h>
#include <core/vfill.h>
#include <core/vstroke.h>
#include <commands/vfillcmd.h>
#include <commands/vstrokecmd.h>
#include <widgets/vstrokefillpreview.h>

#include <kactioncollection.h>

VPatternWidget::VPatternWidget( Q3PtrList<QTableWidgetItem>* patterns, VTool*, QWidget* parent )
	: KDialog( parent )
	, m_pattern( 0 )
{
	setModal( true );
	setCaption( i18n( "Choose Pattern" ) );
	setButtons( Ok | Cancel );

	QWidget *base = new QWidget( this );
	QVBoxLayout* layout = new QVBoxLayout( base );
	layout->addWidget( m_patternChooser = new KoResourceChooser( QSize( 32, 32 ), base ) );
	layout->addWidget( m_buttonGroup = new Q3HButtonGroup( base ) );
	m_buttonGroup->insert( m_importPatternButton = new QToolButton( m_buttonGroup ) );
	m_buttonGroup->insert( m_deletePatternButton = new QToolButton( m_buttonGroup ) );
	m_patternChooser->setFixedSize( 180, 120 );
	m_importPatternButton->setIcon( SmallIconSet( "14_layer_newlayer" ) );
	m_importPatternButton->setText( i18n( "Import" ) );
	m_deletePatternButton->setIcon( SmallIconSet("14_layer_deletelayer" ) );
	m_deletePatternButton->setText( i18n( "Delete" ) );

	m_buttonGroup->setInsideMargin( 3 );
	m_importPatternButton->setEnabled( true );
	m_deletePatternButton->setEnabled( false );

	//setFrameStyle( Box | Sunken );
	layout->setMargin( 3 );

	connect( m_buttonGroup, SIGNAL( clicked( int ) ), this, SLOT( slotButtonClicked( int ) ) );
	connect( m_patternChooser, SIGNAL( selected( QTableWidgetItem* ) ), this, SLOT( patternSelected( QTableWidgetItem* ) ) );

	QTableWidgetItem* item;
	for( item = patterns->first(); item; item = patterns->next() )
		m_patternChooser->addItem( item );
	m_pattern = (VPattern*)patterns->first();

	setMainWidget( base );
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
	VPattern* pattern = KarbonFactory::rServer()->addPattern( KFileDialog::getOpenFileName( KUrl(),
		"*.jpg *.gif *.png *.tif *.xpm *.bmp", this, i18n( "Choose Pattern to Add" ) ) );
	if( pattern )
		m_patternChooser->addItem( pattern );
} // VPatternWidget::importPattern

void VPatternWidget::deletePattern()
{
//	m_patternChooser->removeItem( m_pattern );
	KarbonFactory::rServer()->removePattern( m_pattern );
	m_pattern = static_cast<VPattern*>( m_patternChooser->currentItem() );
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

void VPatternWidget::patternSelected( QTableWidgetItem* item )
{
	m_pattern = (VPattern*)item;
	m_deletePatternButton->setEnabled( QFileInfo( m_pattern->tilename() ).isWritable() );
} // VPatternWidget::patternSelected

VPatternTool::VPatternTool( KarbonView *view )
	: VTool( view, "tool_pattern" ), m_state( normal ), m_handleSize( 3 ), m_active( false )
{
	Q3PtrList<QTableWidgetItem> patterns = KarbonFactory::rServer()->patterns();
	m_optionsWidget = new VPatternWidget( &patterns, this );
	registerTool( this );
} // VPatternTool::VPatternTool

VPatternTool::~VPatternTool()
{
	delete m_optionsWidget;
} // VPatternTool::~VPatternTool

void
VPatternTool::activate()
{
	m_active = true;
	m_state = normal;
	VTool::activate();
	view()->statusMessage()->setText( i18n( "Pattern" ) );
	view()->setCursor( QCursor( Qt::CrossCursor ) );

	if( view() )
	{
		// disable selection handles
		view()->part()->document().selection()->showHandle( false );
		// connect to the stroke-fill-preview to get notice when the stroke or fill gets selected
		VStrokeFillPreview* preview = view()->strokeFillPreview();
		if( preview )
		{
			connect( preview, SIGNAL( fillSelected() ), this, SLOT( targetChanged() ) );
			connect( preview, SIGNAL( strokeSelected() ), this, SLOT( targetChanged() ) );
		}
		view()->repaintAll( view()->part()->document().selection()->boundingBox() );
	}
}

void
VPatternTool::deactivate()
{
	m_active = false;

	if( view() )
	{
		// enable selection handles
		view()->part()->document().selection()->showHandle( true );
		VStrokeFillPreview* preview = view()->strokeFillPreview();
		if( preview )
		{
			disconnect( preview, SIGNAL( fillSelected() ), this, SLOT( targetChanged() ) );
			disconnect( preview, SIGNAL( strokeSelected() ), this, SLOT( targetChanged() ) );
		}
		view()->repaintAll( view()->part()->document().selection()->boundingBox() );
	}
}


QString 
VPatternTool::contextHelp()
{
	QString s = i18n( "<qt><b>Pattern tool:</b><br>" );
	s += i18n( "<i>Click</i> on the pattern you want in the chooser.<br>" );
	s += i18n( "<i>Click and drag</i> to choose the pattern vector.</qt>" );
	s += i18n( "<i>Press i or Shift+i</i> to decrease or increase the handle size.<br>" );
	return s;
} // VPatternTool::contextHelp

void VPatternTool::draw()
{
	if( ! view() || view()->part()->document().selection()->objects().count() == 0 )
		return;

	VPainter *painter = view()->painterFactory()->editpainter();
	// TODO rasterops need porting
	// painter->setRasterOp( Qt::NotROP );

	painter->setPen( Qt::DotLine );
	
	// differentiate between moving a handle and creating a complete new vector
	if( m_state == moveOrigin || m_state == moveVector )
	{
		painter->newPath();
		painter->moveTo( m_fixed );
		painter->lineTo( m_current );
		painter->strokePath();
		// draw the handle rects
		painter->drawNode( m_fixed, m_handleSize );
		painter->drawNode( m_current, m_handleSize );
	}
	else if( m_state == createNew )
	{
		painter->newPath();
		painter->moveTo( first() );
		painter->lineTo( m_current );
		painter->strokePath();
		// draw the handle rects
		painter->drawNode( first(), m_handleSize );
		painter->drawNode( m_current, m_handleSize );
	}
} // VPatternTool::draw

bool 
VPatternTool::getPattern( VPattern &pattern )
{
	if( ! view() ) 
		return false;
	
	// determine if stroke of fill is selected for editing
	//VStrokeFillPreview *preview = view()->strokeFillPreview();
	//bool strokeSelected = ( preview && preview->strokeIsSelected() );
	bool strokeSelected = false; // FIXME: stroke patterns don't work

	VSelection* selection = view()->part()->document().selection();
	if( selection->objects().count() != 1 ) 
		return false;
	
	VObject *obj = selection->objects().getFirst();
	// get the pattern of the first selected object, if any
	if( strokeSelected && obj->stroke()->type() == VStroke::patt )
		pattern = obj->stroke()->pattern();
	else if( ! strokeSelected && obj->fill()->type() == VFill::patt )
		pattern = obj->fill()->pattern();
	else
		return false;
	
	return true;
}

void 
VPatternTool::draw( VPainter* painter )
{
	if( ! m_active )
		return;

	if( m_state != normal )
		return;

	if( ! getPattern( m_pattern ) )
		return;

	QPointF s = m_pattern.origin();
	QPointF e = m_pattern.vector();

	// save the handle rects for later inside testing
	m_origin = QRectF( s.x()-m_handleSize, s.y()-m_handleSize, 2*m_handleSize, 2*m_handleSize );
	m_vector = QRectF( e.x()-m_handleSize, e.y()-m_handleSize, 2*m_handleSize, 2*m_handleSize );

	QColor lightBlue = QColor( "blue" ).light();
	painter->setPen( lightBlue );
	painter->setBrush( lightBlue );
	/* this needs porting
	painter->setRasterOp( Qt::XorROP );
*/

	// draw the pattern vector
	painter->newPath();
	painter->moveTo( s );
	painter->lineTo( e );
	painter->strokePath();
	
	// draw the handle rects
	painter->drawNode( m_origin.center(), m_handleSize );
	painter->drawNode( m_vector.center(), m_handleSize );
}

void
VPatternTool::mouseDrag()
{
	if( m_state == normal ) 
		return;

	draw();

	m_current = last();

	draw();
} // VPatternTool::mouseDrag

void
VPatternTool::mouseButtonPress()
{
	m_current = first();

	// set the apropriate editing state
	if( m_origin.contains( m_current ) )
	{
		m_state = moveOrigin;
		m_fixed = m_vector.center();
	}
	else if( m_vector.contains( m_current ) )
	{
		m_state = moveVector;
		m_fixed = m_origin.center();
	}
	else 
		m_state = createNew;
} // VPatternTool::mouseButtonPress

void
VPatternTool::mouseButtonRelease()
{
	m_state = normal;

	if( view()->part()->document().selection()->objects().count() == 0 ) 
		return;

	// save old pattern position
	VPattern oldPattern = m_pattern;

	if( first() == last() )
	{
		if( showDialog() != QDialog::Accepted )
			return;
	}
	else if( !m_optionsWidget->selectedPattern() )
		KMessageBox::error( 0L, i18n( "Please select a pattern." ), "" );

	bool strokeSelected = false;

	// determine the target from the stroke-fill-preview-widget
	//VStrokeFillPreview* preview = view()->strokeFillPreview();
	//if( preview && preview->strokeIsSelected() ) // FIXME: stroke patterns don't work
	//	strokeSelected = true;

	// calculate a sane intial position for the new pattern
	if( view()->part()->document().selection()->objects().count() == 1 )
	{
		VObject *obj = view()->part()->document().selection()->objects().getFirst();

		if( ( ! strokeSelected && obj->fill()->type() != VFill::patt ) || (strokeSelected &&  obj->stroke()->type() != VStroke::patt ) )
		{
			QRectF bbox = obj->boundingBox();
			oldPattern.setOrigin( bbox.bottomLeft() + 0.5*(bbox.topLeft()-bbox.bottomLeft()) );
			oldPattern.setVector( bbox.bottomRight() + 0.5*(bbox.topRight()-bbox.bottomRight()) );
		}
	}

	VPattern m_pattern = *m_optionsWidget->selectedPattern();

	// use the old pattern position
	m_pattern.setOrigin( oldPattern.origin() );
	m_pattern.setVector( oldPattern.vector() );

	if( ! strokeSelected )
	{
		VFill fill;
		fill.pattern() = m_pattern;
		fill.setType( VFill::patt );
		view()->part()->addCommand(
			new VFillCmd( &view()->part()->document(), fill, "14_pattern" ), true );
	}
	else
	{
		VStroke stroke;
		stroke.pattern() = m_pattern;
		stroke.setType( VStroke::patt );
		view()->part()->addCommand(
			new VStrokeCmd( &view()->part()->document(), &stroke, "14_pattern" ), true );
	}
} // VPatternTool::mouseButtonRelease

void
VPatternTool::mouseDragRelease()
{
	if( ! view() || m_state == normal )
		return;

	if( view()->part()->document().selection()->objects().count() == 0 )
	{
		draw();
		return;
	}

	if( !m_optionsWidget->selectedPattern() )
	{
		draw();
		KMessageBox::error( 0L, i18n( "Please select a pattern." ), "" );
		return;
	}
	//m_pattern = *m_optionsWidget->selectedPattern();

	if( m_state == moveOrigin )
		m_pattern.setOrigin( last() );
	else if( m_state == moveVector )
		m_pattern.setVector( last() );
	else if( m_state == createNew )
	{
		m_pattern.setOrigin( first() );
		m_pattern.setVector( last() );
	}

	m_state = normal;

	VStrokeFillPreview* preview = view()->strokeFillPreview();
	if( ! preview )
		return;

	//if( ! preview->strokeIsSelected() ) // FIXME: stroke patterns don't work
	{
		VFill fill;
		fill.pattern() = m_pattern;
		fill.setType( VFill::patt );
		view()->part()->addCommand(
			new VFillCmd( &view()->part()->document(), fill, "14_pattern" ), true );
	}
	/*
	else
	{
		VStroke stroke;
		stroke.pattern() = m_pattern;
		stroke.setType( VStroke::patt );
		view()->part()->addCommand(
			new VStrokeCmd( &view()->part()->document(), &stroke, "14_pattern" ), true );
	}
	*/
} // VPatternTool::mouseDragRelease

void
VPatternTool::cancel()
{
	// Erase old object:
	if( isDragging() )
		draw();
	m_state = normal;
}

bool
VPatternTool::showDialog() const
{
	return m_optionsWidget->exec() == QDialog::Accepted;
}

void
VPatternTool::setup( KActionCollection *collection )
{
	m_action = static_cast<KAction *>(collection -> action( objectName() ) );

	if( m_action == 0 )
	{
		m_action = new KAction( KIcon( "14_pattern" ), i18n( "Pattern Tool" ), collection, objectName() );
		m_action->setDefaultShortcut( Qt::SHIFT+Qt::Key_H );
		m_action->setToolTip( i18n( "Pattern" ) );
		connect( m_action, SIGNAL( triggered() ), this, SLOT( activate() ) );
		// TODO needs porting: m_action->setExclusiveGroup( "misc" );
		//m_ownAction = true;
	}
}

void
VPatternTool::setCursor() const
{
	if( !view() ) return;

	// set a different cursor if mouse is inside the handle rects
	if( m_origin.contains( last() ) || m_vector.contains( last() ) )
		view()->setCursor( QCursor( Qt::SizeAllCursor ) );
	else
		view()->setCursor( QCursor( Qt::ArrowCursor ) );
}

bool 
VPatternTool::keyReleased( Qt::Key key )
{
	// increase/decrease the handle size
	switch( key )
	{
		case Qt::Key_I:
			if( shiftPressed() ) 
				m_handleSize++;
			else if( m_handleSize > 3 )
				m_handleSize--;
		break;
		default: return false;
	}

	if( view() )
		view()->repaintAll( view()->part()->document().selection()->boundingBox() );

	return true;
}

void 
VPatternTool::targetChanged()
{
	if( view() ) 
		view()->repaintAll( view()->part()->document().selection()->boundingBox() );
}

#include "vpatterntool.moc"
