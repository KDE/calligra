/* This file is part of the KDE project
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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "roundcornersplugin.h"
#include <karbon_view.h>
#include <karbon_part.h>
#include <core/vpath.h>
#include <core/vsegment.h>
#include <kgenericfactory.h>
#include <kdebug.h>
#include <qgroupbox.h>
#include <QLabel>

#include <knuminput.h>

typedef KGenericFactory<VRoundCornersPlugin, KarbonView> VRoundCornersPluginFactory;
K_EXPORT_COMPONENT_FACTORY( karbon_roundcornersplugin, VRoundCornersPluginFactory( "karbonroundcornersplugin" ) )

VRoundCornersPlugin::VRoundCornersPlugin( KarbonView *parent, const QStringList & ) : Plugin( parent )
{
	new KAction(
		i18n( "&Round Corners..." ), "14_roundcorners", 0, this,
		SLOT( slotRoundCorners() ), actionCollection(), "path_round_corners" );

	m_roundCornersDlg = new VRoundCornersDlg();
	m_roundCornersDlg->setRadius( 10.0 );
}

VRoundCornersPlugin::~VRoundCornersPlugin()
{
}

void
VRoundCornersPlugin::slotRoundCorners()
{
	KarbonPart *part = ((KarbonView *)parent())->part();
	if( part && m_roundCornersDlg->exec() )
		part->addCommand( new VRoundCornersCmd( &part->document(), m_roundCornersDlg->radius() ), true );
}


VRoundCornersDlg::VRoundCornersDlg( QWidget* parent, const char* name )
	: KDialogBase( parent, name, true,  i18n( "Polygonize" ), Ok | Cancel  )
{
	// add input:
/* TODO needs porting (parent of QGroubBox)
	QGroupBox* group = new QGroupBox( 2, Qt::Horizontal, i18n( "Properties" ), this );

	new QLabel( i18n( "Round corners:" ), group );
	m_radius = new KDoubleNumInput( group );
	group->setMinimumWidth( 300 );

	// signals and slots:
	connect( this, SIGNAL( okClicked() ), this, SLOT( accept() ) );
	connect( this, SIGNAL( cancelClicked() ), this, SLOT( reject() ) );

	setMainWidget( group );
	setFixedSize( baseSize() );
*/
}

double
VRoundCornersDlg::radius() const
{
	return m_radius->value();
}

void
VRoundCornersDlg::setRadius( double value )
{
	m_radius->setValue(value);
}



VRoundCornersCmd::VRoundCornersCmd( VDocument* doc, double radius )
	: VReplacingCmd( doc, i18n( "Round Corners" ) )
{
	// Set members.
	m_radius = radius > 0.0 ? radius : 1.0;
}

void
VRoundCornersCmd::visitVSubpath( VSubpath& path )
{
	// Optimize and avoid a crash.
	if( path.isEmpty() )
		return;

	// Note: we modiy segments from path. that doesn't hurt, since we
	// replace "path" with the temporary path "newPath" afterwards.

	VSubpath newPath( 0L );

	path.first();
	// Skip "begin".
	path.next();

	/* This algorithm is worked out by <kudling AT kde DOT org> to produce similar results as
	 * the "round corners" algorithms found in other applications. Neither code nor
	 * algorithms from any 3rd party is used though.
	 *
	 * We want to replace all corners with round corners having "radius" m_radius.
	 * The algorithm doesn't really produce circular arcs, but that's ok since
	 * the algorithm achieves nice looking results and is generic enough to be applied
	 * to all kind of paths.
	 * Note also, that this algorithm doesn't touch smooth joins (in the sense of
	 * VSegment::isSmooth() ).
	 *
	 * We'll manipulate the input path for bookkeeping purposes and construct a new
	 * temporary path in parallel. We finally replace the input path with the new path.
	 *
	 *
	 * Without restricting generality, let's assume the input path is closed and
	 * contains segments which build a rectangle.
	 *
	 *           2
	 *    O------------O
	 *    |            |        Numbers reflect the segments' order
	 *   3|            |1       in the path. We neglect the "begin"
	 *    |            |        segment here.
	 *    O------------O
	 *           0
	 *
	 * There are three unique steps to process. The second step is processed
	 * many times in a loop.
	 *
	 * 1) Begin
	 *    -----
	 *    Split the first segment of the input path (called "path[0]" here)
	 *    at parameter t
	 *
	 *        t = path[0]->param( m_radius )
	 *
	 *    and move newPath to this new knot. If current segment is too small
	 *    (smaller than 2 * m_radius), we always set t = 0.5 here and in the further
	 *    steps as well.
	 *
	 *    path:                 new path:
	 *
	 *           2
	 *    O------------O
	 *    |            |
	 *  3 |            | 1                    The current segment is marked with "#"s.
	 *    |            |
	 *    O##O#########O        ...O
	 *           0                     0
	 *
	 * 2) Loop
	 *    ----
	 *    The loop step is iterated over all segments. After each appliance the index n
	 *    is incremented and the loop step is reapplied until no untouched segment is left.
	 *
	 *    Split the current segment path[n] of the input path at parameter t
	 *
	 *        t = path[n]->param( path[n]->length() - m_radius )
	 *
	 *    and add the first subsegment of the curent segment to newPath.
	 *
	 *    path:                 new path:
	 *
	 *           2
	 *    O------------O
	 *    |            |
	 *  3 |            | 1
	 *    |            |
	 *    O--O######O##O           O------O...
	 *           0                     0
	 *
	 *    Now make the second next segment (the original path[1] segment in our example)
	 *    the current one. Split it at parameter t
	 *
	 *        t = path[n]->param( m_radius )
	 *
	 *    path:                 new path:
	 *
	 *           2
	 *    O------------O
	 *    |            #
	 *  3 |            O 1
	 *    |            #
	 *    O--O------O--O           O------O...
	 *           0                     0
	 *
	 *    Make the first subsegment of the current segment the current one.
	 *
	 *    path:                 new path:
	 *
	 *           2
	 *    O------------O
	 *    |            |
	 *  3 |            O 1                   O
	 *    |            #                    /.1
	 *    O--O------O--O           O------O...
	 *           0                     0
	 *
	 * 3) End
	 *    ---
	 *
	 *    path:                 new path:
	 *
	 *           2                     4
	 *    O--O------O--O        5 .O------O. 3
	 *    |            |         /          \
	 *  3 O            O 1    6 O            O 2
	 *    |            |      7 .\          /
	 *    O--O------O--O        ...O------O. 1
	 *           0                     0
	 */

	double length;
	double param;

	// "Begin" step.
	// =============

	// Convert flat beziers to lines.
	if( path.current()->isFlat() )
		path.current()->setDegree( 1 );

	if( path.getLast()->isFlat() )
		path.getLast()->setDegree( 1 );

	if(
		path.isClosed() &&
		// Don't touch smooth joins.
		!path.getLast()->isSmooth( *path.current() ) )
	{
		length = path.current()->length();

		param = length > 2 * m_radius
			? path.current()->lengthParam( m_radius )
			: param = 0.5;


		path.insert(
			path.current()->splitAt( param ) );

		newPath.moveTo(
			path.current()->knot() );

		path.next();


		if( !success() )
			setSuccess();
	}
	else
	{
		newPath.moveTo(
			path.current()->prev()->knot() );
	}


	// "Loop" step.
	// ============

	while(
		path.current() &&
		path.current()->next() )
	{
		// Convert flat beziers to lines.
		if( path.current()->isFlat() )
			path.current()->setDegree( 1 );

		if( path.current()->next()->isFlat() )
			path.current()->next()->setDegree( 1 );


		// Don't touch smooth joins.
		if( path.current()->isSmooth() )
		{
			newPath.append( path.current()->clone() );
			path.next();
			continue;
		}


		// Split the current segment at param( m_radius ) counting
		// from the end.
		length = path.current()->length();

		// If the current segment is too short to be split, just don't split it
		// because it was split already a t=0.5 during the former step.
		if( length > m_radius )
		{
			param = path.current()->lengthParam( length - m_radius );

			path.insert(
				path.current()->splitAt( param ) );
			newPath.append(
				path.current()->clone() );

			path.next();
		}


		// Jump to the next untouched segment.
		path.next();


		// Split the current segment at param( m_radius ).
		length = path.current()->length();

		param = length > 2 * m_radius
			? path.current()->lengthParam( m_radius )
			: 0.5;

		path.insert(
			path.current()->splitAt( param ) );


		// Round corner.
		newPath.curveTo(
			path.current()->prev()->pointAt( 0.5 ),
			path.current()->pointAt( 0.5 ),
			path.current()->knot() );


		if( !success() )
			setSuccess();

		path.next();
	}


	// "End" step.
	// ===========

	if( path.isClosed() )
	{
		// Convert flat beziers to lines.
		if( path.current()->isFlat() )
			path.current()->setDegree( 1 );

		if( path.getFirst()->next()->isFlat() )
			path.getFirst()->next()->setDegree( 1 );

		// Don't touch smooth joins.
		if( !path.current()->isSmooth( *path.getFirst()->next() ) )
		{
			length = path.current()->length();

			// If the current segment is too short to be split, just don't split it
			// because it was split already at t=0.5 during the former step.
			if( length > m_radius )
			{
				param = path.current()->lengthParam( length - m_radius );

				path.insert(
					path.current()->splitAt( param ) );
				newPath.append(
					path.current()->clone() );

				path.next();
			}


			path.first();
			path.next();

			// Round corner.
			newPath.curveTo(
				path.getLast()->pointAt( 0.5 ),
				path.current()->pointAt( 0.5 ),
				path.current()->knot() );


			if( !success() )
				setSuccess();
		}
		else
			newPath.append( path.current()->clone() );

		newPath.close();
	}
	else
		newPath.append( path.current()->clone() );


	path = newPath;

	// Invalidate bounding box once.
	path.invalidateBoundingBox();
}

#include "roundcornersplugin.moc"

