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

#include <qlabel.h>
#include <qlayout.h>
#include <qtabwidget.h>
#include <qwidget.h>
#include <qcolor.h>
#include <qtooltip.h>
#include <qevent.h>
#include <qptrlist.h>

#include <klocale.h>
#include <KoMainWindow.h>

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

#include "vcolordocker.h"

#include <ko_hsv_widget.h>
#include <ko_cmyk_widget.h>
#include <ko_rgb_widget.h>
#include <koColor.h>

#include <kdebug.h>

VColorDocker::VColorDocker( KarbonPart* part, KarbonView* parent, const char* /*name*/ )
	: QWidget(), m_part ( part ), m_view( parent )
{
	m_isStrokeDocker = false;
	setCaption( i18n( "Color Chooser" ) );

	m_opacity = 1;

	m_fillCmd = 0;
	m_strokeCmd = 0;

	mTabWidget = new QTabWidget( this );

	/* ##### HSV WIDGET ##### */
	mHSVWidget = new KoHSVWidget( mTabWidget );
	connect( mHSVWidget, SIGNAL( sigFgColorChanged( const QColor &) ), this, SLOT( updateFgColor( const QColor &) ) );
	connect( mHSVWidget, SIGNAL( sigBgColorChanged( const QColor &) ), this, SLOT( updateBgColor( const QColor &) ) );
	connect(this, SIGNAL(fgColorChanged(const QColor &)), mHSVWidget, SLOT(setFgColor(const QColor &)));
	connect(this, SIGNAL(bgColorChanged(const QColor &)), mHSVWidget, SLOT(setBgColor(const QColor &)));
	connect( mHSVWidget, SIGNAL( sigModeChanged(KDualColorButton::DualColor) ), this, SLOT( updateMode( KDualColorButton::DualColor ) ) );
	mTabWidget->addTab( mHSVWidget, i18n( "HSV" ) );

	/* ##### RGB WIDGET ##### */
	mRGBWidget = new KoRGBWidget( mTabWidget );
	connect( mRGBWidget, SIGNAL( sigFgColorChanged( const QColor &) ), this, SLOT( updateFgColor( const QColor &) ) );
	connect( mRGBWidget, SIGNAL( sigBgColorChanged( const QColor &) ), this, SLOT( updateBgColor( const QColor &) ) );
	connect(this, SIGNAL(fgColorChanged(const QColor &)), mRGBWidget, SLOT(setFgColor(const QColor &)));
	connect(this, SIGNAL(bgColorChanged(const QColor &)), mRGBWidget, SLOT(setBgColor(const QColor &)));
	connect( mRGBWidget, SIGNAL( sigModeChanged(KDualColorButton::DualColor) ), this, SLOT( updateMode( KDualColorButton::DualColor ) ) );
	mTabWidget->addTab( mRGBWidget, i18n( "RGB" ) );

	/* ##### CMYK WIDGET ##### */
	/*mCMYKWidget = new KoCMYKWidget( mTabWidget );
	connect( mCMYKWidget, SIGNAL( sigFgColorChanged( const QColor &) ), this, SLOT( updateFgColor( const QColor &) ) );
	connect( mCMYKWidget, SIGNAL( sigBgColorChanged( const QColor &) ), this, SLOT( updateBgColor( const QColor &) ) );
	mTabWidget->addTab( mCMYKWidget, i18n( "CMYK" ) );*/

	//Opacity
	mOpacity = new VColorSlider( i18n( "Opacity:" ), QColor( "white" ), QColor( "black" ), 0, 100, 100, this );
	//TODO: Make "white" a transparent color
	connect( mOpacity, SIGNAL( valueChanged ( int ) ), this, SLOT( updateOpacity() ) );
	QToolTip::add( mOpacity, i18n( "Alpha (opacity)" ) );

	QVBoxLayout *mainWidgetLayout = new QVBoxLayout( this, 3 );
	mainWidgetLayout->addWidget( mTabWidget );
	mainWidgetLayout->addWidget( mOpacity );
	mainWidgetLayout->activate();
	setMaximumHeight( 174 );
	setMinimumWidth( 194 );
	
}

VColorDocker::~VColorDocker()
{
}

void VColorDocker::updateFgColor(const QColor &c)
{
	m_color = c;

	VColor v = VColor(c);
	v.setOpacity( m_opacity );

	// check if we have objects selected
	QPtrList<VObject> VNewObjectList = m_part->document().selection()->objects();
	if( ! VNewObjectList.count() )
		return;
	
	mHSVWidget->blockSignals(true);
	mRGBWidget->blockSignals(true);
	//mCMYKWidget->blockSignals(true);
	
	VCommandHistory* history = m_part->commandHistory();
	const QPtrList<VCommand>* commandList = history->commands();
	VStrokeCmd* command = dynamic_cast<VStrokeCmd*>(commandList->getLast());

	if(command == 0 || m_strokeCmd == 0)
	{
		m_strokeCmd = new VStrokeCmd( &m_part->document(), v );
		m_part->addCommand( m_strokeCmd, true );
	}
	else
	{
		QPtrList<VObject> VOldObjectList = command->getSelection()->objects();

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
 
	emit fgColorChanged( c );

	mHSVWidget->blockSignals(false);
	mRGBWidget->blockSignals(false);
	//mCMYKWidget->blockSignals(false);
}

void VColorDocker::updateBgColor(const QColor &c)
{
	m_color = c;

	VColor v = VColor(c);
	v.setOpacity( m_opacity );
	
	// check if we have objects selected
	QPtrList<VObject> VNewObjectList = m_part->document().selection()->objects();
	if( ! VNewObjectList.count() )
		return;

	mHSVWidget->blockSignals(true);
	mRGBWidget->blockSignals(true);
	//mCMYKWidget->blockSignals(true);
	
	VCommandHistory* history = m_part->commandHistory();
	const QPtrList<VCommand>* commandList = history->commands();
	VFillCmd* command = dynamic_cast<VFillCmd*>(commandList->getLast());

	if(command == 0 || m_fillCmd == 0)
	{
		m_fillCmd = new VFillCmd( &m_part->document(), VFill(v) );
		m_part->addCommand( m_fillCmd, true );
	}
	else
	{
		QPtrList<VObject> VOldObjectList = command->getSelection()->objects();

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
 
	emit bgColorChanged( c );

	mHSVWidget->blockSignals(false);
	mRGBWidget->blockSignals(false);
	//mCMYKWidget->blockSignals(false);
}

void VColorDocker::updateOpacity()
{
	m_opacity = mOpacity->value() / 100.0;

	VColor c = VColor(m_color);
	c.setOpacity( m_opacity );

	if ( isStrokeDocker() )
		m_part->addCommand( new VStrokeCmd( &m_part->document(), c ), true );
	else
		m_part->addCommand( new VFillCmd( &m_part->document(), VFill( c ) ), true );
}

void
VColorDocker::mouseReleaseEvent( QMouseEvent * )
{
	//changeColor();
}

void VColorDocker::setFillDocker()
{
	m_isStrokeDocker = false;
	mHSVWidget->setMode( KDualColorButton::Background );
	mRGBWidget->setMode( KDualColorButton::Background );
	update();
}

void VColorDocker::setStrokeDocker()
{
	m_isStrokeDocker = true;
	mHSVWidget->setMode( KDualColorButton::Foreground );
	mRGBWidget->setMode( KDualColorButton::Foreground );
	update();
}

void VColorDocker::update()
{
	mHSVWidget->blockSignals(true);
	mRGBWidget->blockSignals(true);
	//mCMYKWidget->blockSignals(true);

	int objCnt = m_part->document().selection()->objects().count();

	if( objCnt > 0 )
	{
		VObject *obj = m_part->document().selection()->objects().getFirst();

		QColor fgColor = QColor(obj->stroke() ? obj->stroke()->color() : VColor() );
		QColor bgColor = QColor(obj->fill() ? obj->fill()->color() : VColor() );

		mHSVWidget->setFgColor(fgColor);
		mRGBWidget->setFgColor(fgColor);
		//mCMYKWidget->setFgColor(fgColor);
			
		mHSVWidget->setBgColor(bgColor);
		mRGBWidget->setBgColor(bgColor);
		//mCMYKWidget->setBgColor(bgColor);

		if( m_isStrokeDocker )
			m_color = fgColor;
		else
			m_color = bgColor;
	}

	mHSVWidget->blockSignals(false);
	mRGBWidget->blockSignals(false);
	//mCMYKWidget->blockSignals(false);
}

void VColorDocker::updateMode( KDualColorButton::DualColor s )
{
	m_isStrokeDocker = (s == KDualColorButton::Foreground);
	update();
	emit modeChanged( s );
}

#include "vcolordocker.moc"

