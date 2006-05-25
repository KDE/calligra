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

#include <qcursor.h>
#include <qevent.h>
#include <QLabel>
#include <q3groupbox.h>
#include <QComboBox>
#include <QCheckBox>
#include <q3vbox.h>
#include <q3widgetstack.h>
//Added by qt3to4:
#include <Q3PtrList>

#include <klocale.h>
#include <knuminput.h>
//#include <KoUnitWidgets.h>

#include <karbon_part.h>
#include <karbon_view.h>

#include <core/vcolor.h>
#include <core/vcomposite.h>
#include <core/vfill.h>
#include <core/vstroke.h>
#include <core/vglobal.h>
#include <core/vselection.h>
#include <core/vcursor.h>
#include <render/vpainter.h>
#include <render/vpainterfactory.h>
#include "vpenciltool.h"
#include <commands/vshapecmd.h>

#include "vcurvefit.h"

#include "vpenciltool.moc"

VPencilOptionsWidget::VPencilOptionsWidget( KarbonView*view, QWidget* parent, const char* name )
	: KDialogBase( parent, name, true, i18n( "Pencil Settings" ), Ok | Cancel ), m_view( view )
{
	Q3VBox *vbox = new Q3VBox( this );

	m_combo = new QComboBox( vbox );

	m_combo->insertItem( i18n( "Raw" ) );
	m_combo->insertItem( i18n( "Curve" ) );
	m_combo->insertItem( i18n( "Straight" ) );

	m_widgetStack  = new Q3WidgetStack( vbox );

	Q3GroupBox *group1 = new Q3GroupBox( 2, Qt::Horizontal, i18n( "Properties" ), m_widgetStack );
	m_widgetStack->addWidget( group1, 1 );
	m_optimizeRaw = new QCheckBox( i18n( "Optimize" ), group1 );

	group1->setInsideMargin( 4 );
	group1->setInsideSpacing( 2 );

	Q3GroupBox *group2 = new Q3GroupBox( 2, Qt::Horizontal, i18n( "Properties" ), m_widgetStack );
	m_widgetStack->addWidget( group2, 2 );

	Q3VBox *vbox2 = new Q3VBox( group2 );

	m_optimizeCurve = new QCheckBox( i18n( "Optimize" ), vbox2 );
	m_fittingError = new KDoubleNumInput( 0.0, 400.0, 4.00, 0.50, 3, vbox2 );
	m_fittingError->setLabel( i18n( "Exactness:" ) );

	group2->setInsideMargin( 4 );
	group2->setInsideSpacing( 2 );

	Q3GroupBox *group3 = new Q3GroupBox( 2, Qt::Horizontal, i18n( "Properties" ), m_widgetStack );
	m_widgetStack->addWidget( group3, 3 );

	m_combineAngle = new KDoubleNumInput( 0.0, 360.0, 0.10, 0.50, 3, group3 );
	m_combineAngle->setSuffix( " deg" );
	m_combineAngle->setLabel( i18n( "Combine angle:" ) );

	group3->setInsideMargin( 4 );
	group3->setInsideSpacing( 2 );

	connect( m_combo, SIGNAL( activated( int ) ), this, SLOT( selectMode() ) );

	//Set the default settings
	m_mode = VPencilTool::CURVE;
	selectMode();

	m_optimizeCurve->setChecked( true );
	m_optimizeRaw->setChecked( true );

	setMainWidget( vbox );
}

float VPencilOptionsWidget::combineAngle()
{
	return m_combineAngle->value();
}

bool VPencilOptionsWidget::optimize()
{
	return ( m_optimizeRaw->isChecked() || m_optimizeCurve->isChecked() );
}

float VPencilOptionsWidget::fittingError()
{
	return m_fittingError->value();
}

void VPencilOptionsWidget::selectMode()
{
	m_widgetStack->raiseWidget( m_combo->currentItem() + 1 );

	switch( m_combo->currentItem() )
	{
		case 0: m_mode = VPencilTool::RAW; break;
		case 1: m_mode = VPencilTool::CURVE; break;
		case 2: m_mode = VPencilTool::STRAIGHT; break;
	}
}

int VPencilOptionsWidget::currentMode(){
	return m_mode;
}

/* ------------------------------------------------------------------------------------------------------------------------*/

VPencilTool::VPencilTool( KarbonView *view )
	: VTool( view, "tool_pencil" )
{
	m_Points.setAutoDelete( true );
	m_optionWidget = new VPencilOptionsWidget( view );
	registerTool( this );
	m_mode = CURVE;
	m_optimize = true;
	m_combineAngle = 3.0f;
	m_cursor = new QCursor( VCursor::createCursor( VCursor::CrossHair ) );
}

VPencilTool::~VPencilTool()
{
	delete m_cursor;
}

QString
VPencilTool::contextHelp()
{
	QString s = i18n( "<qt><b>Pencil tool:</b><br>" );
	s += i18n( "- <i>Click</i> to begin drawing, release when you have finished.");
	s += i18n( "- Press <i>Enter</i> or <i>double click</i> to end the polyline.</qt>" );

	return s;
}

void
VPencilTool::activate()
{
	VTool::activate();
	view()->statusMessage()->setText( i18n( "Pencil Tool" ) );
	view()->setCursor( *m_cursor );
	view()->part()->document().selection()->showHandle( false );

	m_Points.clear();
	m_close = false;
}

void
VPencilTool::deactivate()
{
	m_Points.removeLast();
	m_Points.removeLast();

	VPath* line = 0L;

	Q3PtrList<KoPoint> complete;
	Q3PtrList<KoPoint> *points = &m_Points;

	if( m_Points.count() > 1 )
	{
		if( m_optimize || m_mode == STRAIGHT )
		{
			complete.setAutoDelete( true );
			m_Points.setAutoDelete( false );

			float cangle;

			if( m_mode == STRAIGHT )
				cangle = m_combineAngle;
			else
				cangle = 0.50f;

			#define ANGLE(P0,P1)\
				atan((P1)->y()-(P0)->y())/((P1)->x()-(P0)->x())*(180/M_PI)

			//Add the first point
			complete.append( m_Points.first() );
			complete.append( m_Points.next() );

			//Now we need to get the angle of the first line
			float langle = ANGLE( complete.at( 0 ), complete.at( 1 ) );

			KoPoint *nextp = NULL;
			while( ( nextp = m_Points.next() ) )
			{
				float angle = ANGLE( complete.last(), nextp );
				if( QABS( angle - langle ) < cangle )
					complete.removeLast();
				complete.append(nextp);
				langle=angle;
			}
			m_Points.clear();
			m_Points.setAutoDelete(true);

			points = &complete;
		}

		switch(m_mode)
		{
			case CURVE:
			{
				line = bezierFit( *points, m_optionWidget->fittingError() );
				break;
			}
			case STRAIGHT:
			case RAW:
			{
				line = new VPath( 0L );
				KoPoint* p1 = (*points).first();
				KoPoint* plast = p1;
				line->moveTo( *p1 );

				KoPoint* pnext = 0L;

				while( ( pnext = (*points).next() ) )
				{
					line->lineTo( *pnext );
					plast = pnext;
				}
				break;
			}
		}

		if( shiftPressed() )
			line->close();
	}

	if( line )
	{
		VShapeCmd* cmd = new VShapeCmd(
			&view()->part()->document(),
			i18n( "Pencil" ),
			line,
			"14_pencil" );

		view()->part()->addCommand( cmd, true );
	}
}

void
VPencilTool::draw()
{
	VPainter* painter = view()->painterFactory()->editpainter();
	painter->setRasterOp( Qt::NotROP );

	m_mode = m_optionWidget->currentMode();
	m_optimize = m_optionWidget->optimize();
	m_combineAngle = m_optionWidget->combineAngle();

	if( m_Points.count() > 1 )
	{
		VPath line( 0L );
		line.moveTo( *m_Points.first() );

		KoPoint *pnext;
		while((pnext=m_Points.next())){
			line.lineTo( *pnext );
		}

		line.setState( VObject::edit );
		line.draw( painter, &line.boundingBox() );
	}

}


void
VPencilTool::mouseMove()
{
}

void
VPencilTool::mouseButtonPress()
{
	m_Points.append( new KoPoint( last() ) );

	draw();
}

void
VPencilTool::mouseButtonRelease()
{
	m_Points.append( new KoPoint( last() ) );
	draw();
	accept();
	return;
}

void
VPencilTool::mouseButtonDblClick()
{
	accept();
}

void
VPencilTool::mouseDrag()
{
	if( m_Points.count() != 0 )
	{
		draw();

		m_Points.append( new KoPoint( last() ) );

		draw();
	}
}

void
VPencilTool::mouseDragRelease()
{
	mouseButtonRelease();
}

void
VPencilTool::mouseDragShiftPressed()
{
}

void
VPencilTool::mouseDragCtrlPressed()
{

}

void
VPencilTool::mouseDragShiftReleased()
{
}

void
VPencilTool::mouseDragCtrlReleased()
{
}

void
VPencilTool::cancel()
{
	draw();

	m_Points.clear();
}

void
VPencilTool::cancelStep()
{
	draw();

	m_Points.clear();

	draw();
}

void
VPencilTool::accept()
{
	deactivate();
	activate();
}

bool
VPencilTool::showDialog() const
{
	return m_optionWidget->exec() == QDialog::Accepted;
}

void
VPencilTool::setup( KActionCollection *collection )
{
	m_action = static_cast<KRadioAction *>(collection -> action( name() ) );

	if( m_action == 0 )
	{
		m_action = new KRadioAction( i18n( "Pencil Tool" ), "14_pencil", Qt::SHIFT+Qt::Key_P, this, SLOT( activate() ), collection, name() );
		m_action->setToolTip( i18n( "Pencil" ) );
		m_action->setExclusiveGroup( "freehand" );
		//m_ownAction = true;
	}
}

