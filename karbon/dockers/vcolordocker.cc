/* This file is part of the KDE project
   Made by Tomislav Lukman (tomislav.lukman@ck.tel.hr)
   Copyright (C) 2002 - 2005, The Karbon Developers

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
#include <QLayout>
#include <qtabwidget.h>
#include <QWidget>
#include <QColor>
#include <QToolTip>
#include <qevent.h>
#include <q3ptrlist.h>
#include <QVBoxLayout>
#include <QMouseEvent>

#include <klocale.h>
#include <KoMainWindow.h>
#include <KoUniColorChooser.h>
#include <KoShapeManager.h>
#include <KoSelection.h>

#include "karbon_part.h"
#include "karbon_view.h"
#include "karbon_factory.h"
#include "karbon_resourceserver.h"
#include "vcolor.h"
#include "vcolorslider.h"
#include "vselection.h"
#include "vfillcmd.h"
#include "vstrokecmd.h"
#include "vcommand.h"
#include "vobject.h"
#include "vcanvas.h"
#include "vcolordocker.h"

#include <kdebug.h>

VColorDocker::VColorDocker( KarbonPart* part, KarbonView* parent, const char* /*name*/ )
	: QWidget(), m_part ( part ), m_view( parent )
{
	m_isStrokeDocker = false;
	setWindowTitle( i18n( "Color Chooser" ) );

	m_opacity = 1;

	m_fillCmd = 0;
	m_strokeCmd = 0;

	/* ##### HSV WIDGET ##### */
	// TODO port to KoUniColorChooser
	m_colorChooser = new KoUniColorChooser( this );
	connect( m_colorChooser, SIGNAL( sigColorChanged( const KoColor &) ), this, SLOT( updateColor( const KoColor &) ) );
	//connect( m_colorChooser, SIGNAL( sigColorChanged( const QColor &) ), this, SLOT( updateBgColor( const QColor &) ) );
	connect(this, SIGNAL(colorChanged(const KoColor &)), m_colorChooser, SLOT(setColor(const KoColor &)));
	//connect(this, SIGNAL(bgColorChanged(const QColor &)), mHSVWidget, SLOT(setBgColor(const QColor &)));

	//Opacity
	mOpacity = new VColorSlider( i18n( "Opacity:" ), QColor( "white" ), QColor( "black" ), 0, 100, 100, this );
	//TODO: Make "white" a transparent color
	connect( mOpacity, SIGNAL( valueChanged ( int ) ), this, SLOT( updateOpacity() ) );
	mOpacity->setToolTip( i18n( "Alpha (opacity)" ) );

	QVBoxLayout *mainWidgetLayout = new QVBoxLayout;
	mainWidgetLayout->addWidget( m_colorChooser );
	mainWidgetLayout->addWidget( mOpacity );
	mainWidgetLayout->activate();

	setLayout(mainWidgetLayout);

	//setMaximumHeight( 174 );
	setMinimumWidth( 194 );
}

VColorDocker::~VColorDocker()
{
}

void VColorDocker::updateColor( const KoColor &c )
{
	KoSelection *selection = m_view->canvasWidget()->shapeManager()->selection();
	if( ! selection )
		return;

	foreach( KoShape* shape, selection->selectedShapes() )
	{
		shape->setBackground( QBrush( c.toQColor() ) );
		shape->repaint();
	}
}

void VColorDocker::updateFgColor(const KoColor &c)
{
	m_colorChooser->blockSignals(true);

	m_oldColor = m_color;

	m_color = c;
	/*
	VColor v = VColor(c);
	v.setOpacity( m_opacity );

	VCommandHistory* history = m_part->commandHistory();
	const Q3PtrList<VCommand>* commandList = history->commands();
	VStrokeCmd* command = dynamic_cast<VStrokeCmd*>(commandList->getLast());

	if(command == 0 || m_strokeCmd == 0)
	{
		m_strokeCmd = new VStrokeCmd( &m_part->document(), v );
		m_part->addCommand( m_strokeCmd, true );
	}
	else
	{

		Q3PtrList<VObject> VOldObjectList = command->getSelection()->objects();
		Q3PtrList<VObject> VNewObjectList = m_part->document().selection()->objects();

		if( VOldObjectList == VNewObjectList )
		{
			m_strokeCmd->changeStroke(v);
			m_part->repaintAllViews();
		}
		else
		{
			m_strokeCmd = new VStrokeCmd( &m_part->document(), v );
			m_part->addCommand( m_strokeCmd, true );
		}
	}
	*/
	emit colorChanged( c );

	m_colorChooser->blockSignals(false);
}

void VColorDocker::updateBgColor(const KoColor &c)
{
	m_colorChooser->blockSignals(true);

	m_oldColor = m_color;

	m_color = c;

	/*
	VColor v = VColor(c);
	v.setOpacity( m_opacity );

	VCommandHistory* history = m_part->commandHistory();
	const Q3PtrList<VCommand>* commandList = history->commands();
	VFillCmd* command = dynamic_cast<VFillCmd*>(commandList->getLast());

	if(command == 0 || m_fillCmd == 0)
	{
		m_fillCmd = new VFillCmd( &m_part->document(), VFill(v) );
		m_part->addCommand( m_fillCmd, true );
	}
	else
	{

		Q3PtrList<VObject> VOldObjectList = command->getSelection()->objects();
		Q3PtrList<VObject> VNewObjectList = m_part->document().selection()->objects();

		if( VOldObjectList == VNewObjectList )
		{
			m_fillCmd->changeFill(VFill(v));
			m_part->repaintAllViews();
		}
		else
		{
			m_fillCmd = new VFillCmd( &m_part->document(), VFill(v) );
			m_part->addCommand( m_fillCmd, true );
		}
	}
	*/
	emit colorChanged( c );

	m_colorChooser->blockSignals(false);
}

void VColorDocker::updateOpacity()
{
	m_opacity = mOpacity->value() / 100.0;

	m_oldColor = m_color;

	/*
	VColor c = VColor(m_color);
	c.setOpacity( m_opacity );

	if ( isStrokeDocker() )
		m_part->addCommand( new VStrokeCmd( &m_part->document(), c ), true );
	else
		m_part->addCommand( new VFillCmd( &m_part->document(), VFill( c ) ), true );
	*/
}

void
VColorDocker::mouseReleaseEvent( QMouseEvent * )
{
	//changeColor();
}

void VColorDocker::setFillDocker()
{
	m_isStrokeDocker = false;
}

void VColorDocker::setStrokeDocker()
{
	m_isStrokeDocker = true;
}

void VColorDocker::update()
{
	/*
	mHSVWidget->blockSignals(true);

	int objCnt = m_part->document().selection()->objects().count();

	if( objCnt > 0 )
	{
		VObject *obj = m_part->document().selection()->objects().getFirst();

		QColor fgColor = QColor(obj->stroke()->color());
		QColor bgColor = QColor(obj->fill()->color());

		//mHSVWidget->setFgColor(fgColor);
		//mHSVWidget->setBgColor(bgColor);
	}

	mHSVWidget->blockSignals(false);
	*/
}

#include "vcolordocker.moc"

