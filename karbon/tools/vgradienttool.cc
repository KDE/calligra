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

#include <qcursor.h>
#include <QLabel>
#include <QRectF>
#include <QPointF>

#include <klocale.h>

#include <karbon_part.h>
#include <karbon_view.h>
#include <karbon_factory.h>
#include <render/vpainter.h>
#include <render/vpainterfactory.h>
#include "vgradienttool.h"
#include <widgets/vgradienttabwidget.h>
#include <commands/vfillcmd.h>
#include <commands/vstrokecmd.h>
#include <core/vstroke.h>
#include <core/vselection.h>
#include <widgets/vstrokefillpreview.h>

#include <kdebug.h>

#include <kactioncollection.h>

VGradientTool::VGradientOptionsWidget::VGradientOptionsWidget( VGradient& gradient )
	: KDialogBase( KDialogBase::Plain, Qt::Dialog, 0L, "", true, i18n( "Edit Gradient" ), Ok | Cancel )
{
	m_gradientWidget = new VGradientTabWidget( gradient, KarbonFactory::rServer(), this );
	setMainWidget( m_gradientWidget );
	setFixedSize( baseSize() );
}

VGradientTool::VGradientTool( KarbonView *view )
	: VTool( view, "gradienttool" ), m_state( normal ), m_handleSize( 3 ), m_active( false )
{
	setObjectName( "tool_gradient" );
	m_optionsWidget = new VGradientOptionsWidget( m_gradient );
	registerTool( this );
}

VGradientTool::~VGradientTool()
{
	delete m_optionsWidget;
}

void
VGradientTool::activate()
{
	m_active = true;
	m_state = normal;
	view()->statusMessage()->setText( i18n( "Gradient" ) );
	view()->setCursor( QCursor( Qt::CrossCursor ) );
	VTool::activate();

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
VGradientTool::deactivate()
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
VGradientTool::statusText()
{
	return i18n( "Gradient tool" );
}

QString 
VGradientTool::contextHelp()
{
	QString s = i18n( "<qt><b>Gradient tool:</b><br>" );
	s += i18n( "<i>Click and drag</i> to choose the gradient vector.<br>" );
	s += i18n( "<i>Click and drag</i> a gradient vector handle to change the gradient vector.<br>" );
	s += i18n( "<i>Shift click and drag</i> to move the radial gradient focal point.<br>" );
	s += i18n( "<i>Press i or Shift+i</i> to decrease or increase the handle size.<br>" );
	s += i18n( "<br><b>Gradient editing:</b><br>" );
	s += i18n( "<i>Click and drag</i> to move points.<br>" );
	s += i18n( "<i>Double click</i> on a color point to edit it.<br>" );
	s += i18n( "<i>Right click</i> on a color point to remove it.</qt>" );
	return s;
}

bool 
VGradientTool::getGradient( VGradient &gradient )
{
	if( ! view() ) 
		return false;
	
	// determine if stroke of fill is selected for editing
	VStrokeFillPreview *preview = view()->strokeFillPreview();
	bool strokeSelected = ( preview && preview->strokeIsSelected() );
		
	VSelection* selection = view()->part()->document().selection();
	if( selection->objects().count() != 1 ) 
		return false;
	
	VObject *obj = selection->objects().getFirst();
	// get the gradient of the first selected object, if any
	if( strokeSelected && obj->stroke()->type() == VStroke::grad )
		gradient = obj->stroke()->gradient();
	else if( ! strokeSelected && obj->fill()->type() == VFill::grad )
		gradient = obj->fill()->gradient();
	else
		return false;
	
	return true;
}

bool
VGradientTool::getOpacity( double &opacity )
{
	if( ! view() ) 
		return false;
	
	// determine if stroke of fill is selected for editing
	VStrokeFillPreview *preview = view()->strokeFillPreview();
	bool strokeSelected = ( preview && preview->strokeIsSelected() );
		
	VSelection* selection = view()->part()->document().selection();
	if( selection->objects().count() != 1 ) 
		return false;
	
	VObject *obj = selection->objects().getFirst();
	// get the opacity of the first selected object, if any
	if( strokeSelected && obj->stroke()->type() == VStroke::grad )
		opacity = obj->stroke()->color().opacity();
	else if( ! strokeSelected && obj->fill()->type() == VFill::grad )
		opacity = obj->fill()->color().opacity();
	else return false;
	
	return true;
}

void 
VGradientTool::draw( VPainter* painter )
{
	if( ! m_active )
		return;

	if( m_state != normal )
		return;

	if( ! getGradient( m_gradient ) )
		return;

	QPointF s = m_gradient.origin();
	QPointF e = m_gradient.vector();
	QPointF f = m_gradient.focalPoint();

	// save the handle rects for later inside testing
	m_origin = QRectF( s.x()-m_handleSize, s.y()-m_handleSize, 2*m_handleSize, 2*m_handleSize );
	m_vector = QRectF( e.x()-m_handleSize, e.y()-m_handleSize, 2*m_handleSize, 2*m_handleSize );
	m_center = QRectF( f.x()-m_handleSize, f.y()-m_handleSize, 2*m_handleSize, 2*m_handleSize );

	QColor lightBlue = QColor( "blue" ).light();
	painter->setPen( lightBlue );
	painter->setBrush( lightBlue );
	/* TODO porting required
	painter->setRasterOp( Qt::XorROP );*/

	// draw the gradient vector
	painter->newPath();
	painter->moveTo( s );
	painter->lineTo( e );
	painter->strokePath();
	
	if( m_gradient.type() == VGradient::radial )
	{
		// draw the focal point cross
		double size = (double)m_handleSize / view()->zoom();
		QPointF focal = m_center.center();
		QRectF cross = QRectF( focal.x()-3*size, focal.y()-3*size, 6*size, 6*size );
		painter->newPath();
		painter->moveTo( cross.topLeft() );
		painter->lineTo( cross.bottomRight() );
		painter->strokePath();
		painter->newPath();
		painter->moveTo( cross.bottomLeft() );
		painter->lineTo( cross.topRight() );
		painter->strokePath();
	}
	// draw the handle rects
	painter->drawNode( m_origin.center(), m_handleSize );
	painter->drawNode( m_vector.center(), m_handleSize );
}

void
VGradientTool::draw()
{
	if( ! view() || view()->part()->document().selection()->objects().count() == 0 )
		return;

	VPainter *painter = view()->painterFactory()->editpainter();
	// TODO: rasterops need porting
	// painter->setRasterOp( Qt::NotROP );

	painter->setPen( Qt::DotLine );
	painter->newPath();
	
	// differentiate between moving a handle and creating a complete new vector
	if( m_state == moveOrigin || m_state == moveVector )
	{
		painter->moveTo( m_fixed );
		painter->lineTo( m_current );
		// draw the handle rects
		painter->drawNode( m_fixed, m_handleSize );
		painter->drawNode( m_current, m_handleSize );
	}
	else if( m_state == createNew )
	{
		painter->moveTo( first() );
		painter->lineTo( m_current );
		// draw the handle rects
		painter->drawNode( first(), m_handleSize );
		painter->drawNode( m_current, m_handleSize );
	}
	else if( m_state == moveCenter )
	{
		// draw the focal point cross
		double size = (double)m_handleSize / view()->zoom();
		QRectF cross = QRectF( m_current.x()-3*size, m_current.y()-3*size, 6*size, 6*size );
		painter->moveTo( cross.topLeft() );
		painter->lineTo( cross.bottomRight() );
		painter->strokePath();
		painter->newPath();
		painter->moveTo( cross.bottomLeft() );
		painter->lineTo( cross.topRight() );
	}

	painter->strokePath();
}

void
VGradientTool::mouseDrag()
{
	if( m_state == normal ) 
		return;

	// undo old line
	draw();

	m_current = last();

	draw();
}

void
VGradientTool::mouseButtonPress()
{
	m_current = first();

	// set the apropriate editing state
	if( m_center.contains( m_current ) && shiftPressed())
	{
		m_state = moveCenter;
	}
	else if( m_origin.contains( m_current ) )
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
}

void
VGradientTool::mouseButtonRelease()
{
	m_state = normal;

	if( ! view() || view()->part()->document().selection()->objects().count() == 0 ) 
		return;

	// save old gradient position
	VGradient oldGradient = m_gradient;

	bool strokeSelected = false;

	// determine the target from the stroke-fill-preview-widget
	VStrokeFillPreview* preview = view()->strokeFillPreview();
	if( preview && preview->strokeIsSelected() )
		strokeSelected = true;

	if( first() == last() )
	{
		m_optionsWidget->gradientWidget()->setGradient( m_gradient );
		if( strokeSelected )
		{
			m_optionsWidget->gradientWidget()->setTarget( VGradientTabWidget::STROKE );
			m_optionsWidget->gradientWidget()->setOpacity( 1.0 );
		}
		else
		{
			m_optionsWidget->gradientWidget()->setTarget( VGradientTabWidget::FILL );
			double opacity;
			if( getOpacity( opacity ) )
				m_optionsWidget->gradientWidget()->setOpacity( opacity );
		}
		
		if( ! showDialog() )
			return;
		
		m_gradient = m_optionsWidget->gradientWidget()->gradient();

		// if the gradient dialog was shown and accepted, determine the target from the dialog
		strokeSelected = ( m_optionsWidget->gradientWidget()->target() == VGradientTabWidget::STROKE );
	}

	// calculate a sane intial position for the new gradient
	if( view()->part()->document().selection()->objects().count() == 1 )
	{
		VObject *obj = view()->part()->document().selection()->objects().getFirst();

		if( ( ! strokeSelected && obj->fill()->type() != VFill::grad ) 
		|| ( strokeSelected && obj->stroke()->type() != VStroke::grad ) )
		{
			QRectF bbox = obj->boundingBox();
			switch( m_gradient.type() )
			{
				case VGradient::linear:
					oldGradient.setOrigin( bbox.bottomLeft() + 0.5*(bbox.bottomRight()-bbox.bottomLeft()) );
					oldGradient.setVector( bbox.topLeft() + 0.5*(bbox.topRight()-bbox.topLeft()) );
					oldGradient.setFocalPoint( bbox.center() );
				break;
				case VGradient::radial:
					oldGradient.setOrigin( bbox.center() );
					oldGradient.setVector( bbox.topLeft() + 0.5*(bbox.topRight()-bbox.topLeft()) );
					oldGradient.setFocalPoint( bbox.center() );
				break;
				case VGradient::conic:
					oldGradient.setOrigin( bbox.center() );
					oldGradient.setVector( bbox.topLeft() + 0.5*(bbox.topRight()-bbox.topLeft()) );
					oldGradient.setFocalPoint( bbox.center() );
				break;
			}
		}
	}

	// workaround for a libart 2.3.10 bug
	if( oldGradient.origin() == oldGradient.vector() )
		oldGradient.vector().rx()+=1;

	// use the old gradient position
	m_gradient.setVector( oldGradient.vector() );
	m_gradient.setOrigin( oldGradient.origin() );
	m_gradient.setFocalPoint( oldGradient.focalPoint() );

	if( ! strokeSelected )
	{
		VFill fill;
		fill.gradient() = m_gradient;
		fill.setType( VFill::grad );
		VColor c = fill.color();
		c.setOpacity( m_optionsWidget->gradientWidget()->opacity() );
		fill.setColor( c, false );
		view()->part()->addCommand(
			new VFillCmd( &view()->part()->document(), fill, "14_gradient" ), true );
	}
	else
	{
		view()->part()->addCommand(
			new VStrokeCmd( &view()->part()->document(), &m_gradient ), true );
	}
}

void
VGradientTool::mouseDragRelease()
{
	if( ! view() || m_state == normal ) 
		return;

	if( view()->part()->document().selection()->objects().count() == 0 )
	{
		draw();
		return;
	}

	if( m_state == moveOrigin )
		m_gradient.setOrigin( last() );
	else if( m_state == moveVector )
		m_gradient.setVector( last() );
	else if( m_state == moveCenter )
		m_gradient.setFocalPoint( last() );
	else if( m_state == createNew )
	{
		m_gradient.setOrigin( first() );
		m_gradient.setFocalPoint( first() );
		m_gradient.setVector( last() );
	}

	m_state = normal;

	VStrokeFillPreview* preview = view()->strokeFillPreview();
	if( ! preview )
		return;

	if( ! preview->strokeIsSelected() )
	{
		VFill fill;
		fill.gradient() = m_gradient;
		fill.setType( VFill::grad );
		VColor c = fill.color();
		c.setOpacity( m_optionsWidget->gradientWidget()->opacity() );
		fill.setColor( c, false );
		view()->part()->addCommand(
			new VFillCmd( &view()->part()->document(), fill, "14_gradient" ), true );
	}
	else
		view()->part()->addCommand(
			new VStrokeCmd( &view()->part()->document(), &m_gradient ), true );
}

void
VGradientTool::cancel()
{
	// Erase old object:
	if( isDragging() )
		draw();
	m_state = normal;
}

bool
VGradientTool::showDialog() const
{
	return m_optionsWidget->exec() == QDialog::Accepted;
}

void
VGradientTool::setup( KActionCollection *collection )
{
	m_action = static_cast<KAction *>(collection -> action( objectName() ) );

	if( m_action == 0 )
	{
		m_action = new KAction( KIcon( "14_gradient" ), i18n( "Gradient Tool" ), collection, objectName() );
		m_action->setDefaultShortcut( Qt::Key_G );
		m_action->setToolTip( i18n( "Gradient" ) );
		connect( m_action, SIGNAL( triggered() ), this, SLOT( activate() ) );
		// TODO needs porting: m_action->setExclusiveGroup( "misc" );
		//m_ownAction = true;
	}
}

void
VGradientTool::setCursor() const
{
	if( !view() ) return;

	// set a different cursor if mouse is inside the handle rects
	if( m_origin.contains( last() ) || m_vector.contains( last() ) || m_center.contains( last() ) )
		view()->setCursor( QCursor( Qt::SizeAllCursor ) );
	else
		view()->setCursor( QCursor( Qt::ArrowCursor ) );
}

bool 
VGradientTool::keyReleased( Qt::Key key )
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
VGradientTool::targetChanged()
{
	if( view() ) 
		view()->repaintAll( view()->part()->document().selection()->boundingBox() );
}

#include "vgradienttool.moc"
