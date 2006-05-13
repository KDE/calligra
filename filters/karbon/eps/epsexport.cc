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

#include <QApplication>
#include <q3cstring.h>
#include <QDateTime>	// For creation date/time.
#include <qdom.h>
#include <QFile>
#include <QString>
#include <q3valuelist.h>
//Added by qt3to4:
#include <QTextStream>

#include <kdebug.h>
#include <kgenericfactory.h>
#include <KoDocumentInfo.h>
#include <KoFilter.h>
#include <KoFilterChain.h>
#include <KoStore.h>

#include "epsexport.h"
#include "epsexportdlg.h"
#include "vcolor.h"
#include "vcomposite.h"
#include "vdashpattern.h"
#include "vdocument.h"
#include "vfill.h"
#include "vgroup.h"
#include "vlayer.h"
#include "vpath.h"
#include "vsegment.h"
#include "vselection.h"
#include "vstroke.h"
#include "vtext.h"


// Define PostScript level1 operators.
static char l1_newpath		= 'N';
static char l1_closepath	= 'C';
static char l1_moveto		= 'm';
static char l1_curveto		= 'c';
static char l1_lineto		= 'l';
static char l1_stroke		= 's';
static char l1_fill			= 'f';
//static char l1_eofill		= 'e';
static char l1_setlinewidth	= 'w';
static char l1_setdash		= 'd';
static char l1_setrgbcolor	= 'r';
static char l1_gsave		= 'S';
static char l1_grestore		= 'R';


class EpsExportFactory : KGenericFactory<EpsExport>
{
public:
	EpsExportFactory( void )
		: KGenericFactory<EpsExport>( "karbonepsexport" )
	{}

protected:
	virtual void setupTranslations( void )
	{
		KGlobal::locale()->insertCatalog( "kofficefilters" );
	}
};


K_EXPORT_COMPONENT_FACTORY( libkarbonepsexport, EpsExportFactory() )


EpsExport::EpsExport( KoFilter*, const char*, const QStringList& )
	: KoFilter(parent)
{
}

KoFilter::ConversionStatus
EpsExport::convert( const QByteArray& from, const QByteArray& to )
{
	if ( to != "image/x-eps" || from != "application/x-karbon" )
	{
		return KoFilter::NotImplemented;
	}


	KoStoreDevice* storeIn = m_chain->storageFile( "root", KoStore::Read );

	if( !storeIn )
		return KoFilter::StupidError;


	KoFilter::ConversionStatus status = KoFilter::OK;

	// Ask questions about PS level etc.
	EpsExportDlg* dialog = new EpsExportDlg();

	QApplication::setOverrideCursor( Qt::ArrowCursor );

	if( dialog->exec() )
	{
		// Which PostScript level to support?
		m_psLevel = dialog->psLevel() + 1;

		QFile fileOut( m_chain->outputFile() );
		if( !fileOut.open( QIODevice::WriteOnly ) )
		{
			QApplication::restoreOverrideCursor();
			delete( dialog );

			return KoFilter::StupidError;
		}

		QDomDocument domIn;
		domIn.setContent( storeIn );
		QDomElement docNode = domIn.documentElement();

		m_stream = new QTextStream( &fileOut );

		// Load the document.
		VDocument doc;
		doc.load( docNode );

		// Process the document.
		doc.accept( *this );

		delete m_stream;
		fileOut.close();
	}
	else
	{
		// Dialog cancelled.
		status = KoFilter::UserCancelled;
	}

	QApplication::restoreOverrideCursor();
	delete( dialog );

	return status;
}

void
EpsExport::visitVDocument( VDocument& document )
{
	// Select all objects.
	document.selection()->append();

	// Get the bounding box of all selected objects.
	const KoRect& rect = document.selection()->boundingBox();

	// Print a header.
	*m_stream <<
		"%!PS-Adobe-3.0 EPSF-3.0\n"
		"%%BoundingBox: " <<
		// Round down.
			qRound( rect.left()   - 0.5 ) << " " <<
			qRound( rect.top()    - 0.5 ) << " " <<
		// Round up.
			qRound( rect.right()  + 0.5 ) << " " <<
			qRound( rect.bottom() + 0.5 ) << "\n" <<
		"%%HiResBoundingBox: " <<
			rect.left()   << " " <<
			rect.top()    << " " <<
			rect.right()  << " " <<
			rect.bottom() << "\n"
		"%%Creator: Karbon14 EPS Exportfilter 0.5"
	<< endl;

	// We dont need the selection anymore.
	document.selection()->clear();


	// Process document info.
	KoStoreDevice* storeIn;
	storeIn = m_chain->storageFile( "documentinfo.xml", KoStore::Read );

	if( storeIn )
	{
		QDomDocument domIn;
		domIn.setContent( storeIn );

		KoDocumentInfo docInfo;
		docInfo.load( domIn );

		KoDocumentInfoAuthor* authorPage =
			static_cast<KoDocumentInfoAuthor*>( docInfo.page( "author" ) );

		// Get creation date/time = "now".
		QDateTime now( QDateTime::currentDateTime() );

		*m_stream <<
			"%%CreationDate: (" << now.toString( Qt::LocalDate ) << ")\n"
			"%%For: (" << authorPage->fullName() << ") (" << authorPage->company() << ")\n"
			"%%Title: (" << docInfo.title() << ")"
		<< endl;
	}


	// Print operator definitions.
	*m_stream <<
		"\n"
		"/" << l1_newpath		<< " {newpath} def\n"
		"/" << l1_closepath		<< " {closepath} def\n"
		"/" << l1_moveto		<< " {moveto} def\n"
		"/" << l1_curveto		<< " {curveto} def\n"
		"/" << l1_lineto		<< " {lineto} def\n"
		"/" << l1_stroke		<< " {stroke} def\n"
		"/" << l1_fill			<< " {fill} def\n"
		"/" << l1_setlinewidth	<< " {setlinewidth} def\n"
		"/" << l1_setdash		<< " {setdash} def\n"
		"/" << l1_setrgbcolor	<< " {setrgbcolor} def\n"
		"/" << l1_gsave			<< " {gsave} def\n"
		"/" << l1_grestore		<< " {grestore} def\n"
	<< endl;

	// Export layers.
	VVisitor::visitVDocument( document );

	// Finished.
	*m_stream <<
		"%%EOF"
	<< endl;
}

void
EpsExport::visitVPath( VPath& composite )
{
	*m_stream << l1_newpath << "\n";

	VVisitor::visitVPath( composite );

	getFill( *composite.fill() );
	getStroke( *composite.stroke() );

	*m_stream << endl;
}

void
EpsExport::visitVSubpath( VSubpath& path )
{
	// Export segments.
	VSubpathIterator itr( path );

	for( ; itr.current(); ++itr )
	{
		VSegment *segment = itr.current();
		if ( segment->isCurve() ) {
		    *m_stream <<
			itr.current()->point( 0 ).x() << " " <<
			itr.current()->point( 0 ).y() << " " <<
			itr.current()->point( 1 ).x() << " " <<
			itr.current()->point( 1 ).y() << " " <<
			itr.current()->knot().x() << " " <<
			itr.current()->knot().y() << " " <<
			l1_curveto << "\n";
		} else if ( segment->isLine() ) {
		    *m_stream <<
			itr.current()->knot().x() << " " <<
			itr.current()->knot().y() << " " <<
			l1_lineto << "\n";
		} else if ( segment->isBegin() ) {
		    *m_stream <<
			itr.current()->knot().x() << " " <<
			itr.current()->knot().y() << " " <<
			l1_moveto << "\n";
		}
	}

	if( path.isClosed() )
		*m_stream << l1_closepath << "\n";
}

void
EpsExport::visitVText( VText& text )
{
	// TODO: currently we only export the glyphs if available.

	// Export the glyphs (= VPaths).
	VPathListIterator itr( text.glyphs() );

	for( ; itr.current(); ++itr )
	{
		visit( *itr.current() );
	}
}

void
EpsExport::getStroke( const VStroke& stroke )
{
	// Solid stroke.
	if( stroke.type() == VStroke::solid )
	{
		// Dash pattern.
		*m_stream << "[";

		const Q3ValueList<float>&
			array( stroke.dashPattern().array() );

		Q3ValueListConstIterator<float> itr = array.begin();
		for( ; itr != array.end(); ++itr )
			 *m_stream << *itr << " ";

		*m_stream <<
			"] " << stroke.dashPattern().offset() <<
			" "  << l1_setdash << " ";

		getColor( stroke.color() );

		// "setlinewidth", "stroke".
		*m_stream <<
			" " << stroke.lineWidth() <<
			" " << l1_setlinewidth <<
			" " << l1_stroke << "\n";
	}
	else if( stroke.type() == VStroke::grad )
	{
		if( m_psLevel == 3 )
		{

		}
	}
}

void
EpsExport::getFill( const VFill& fill )
{
	// Solid fill.
	if( fill.type() == VFill::solid )
	{
		// "gsave".
		*m_stream << l1_gsave << " ";

		// "setrgbcolor".
		getColor( fill.color() );

		// "fill", "grestore".
		*m_stream << " " << l1_fill << " " << l1_grestore << "\n";
	}
	// Gradient.
	else if( fill.type() == VFill::grad )
	{
		if( m_psLevel == 3 )
		{
			// "gsave".
			*m_stream << l1_gsave << " ";

			VGradient grad = fill.gradient();
			Q3PtrVector<VColorStop> ramp = grad.colorStops();
			if( ramp.size() < 2 )
			{
				if( ramp.size() == 1 )
					getColor( ramp[0]->color );
			}
			if( ramp.size() > 2 || ramp.size() == 2 && ramp[0]->midPoint != 0.5 )
			{
				// Gradient with more than two colors or asymmetrical midpoint.
				for( uint i = 1;i < ramp.size();i++ )
				{
					char name[15];
					sprintf( name, "Function%d", 2 * i - 1 );

					VColorStop stop1 = *ramp[i - 1];
					VColorStop stop2 = *ramp[i];
					VColor mid;
					mid.set( 0.5 * ( stop1.color[0] + stop2.color[0] ), 0.5 * ( stop1.color[1] + stop2.color[1] ), 0.5 * ( stop1.color[2] + stop2.color[2] ) );
					*m_stream << "/" << name << " 7 dict def " << name << " begin\n" << "\t/FunctionType 2 def\n"
								<< "\t/Domain [ 0 1 ] def\n" << "\t/C0 [ " << stop1.color[0] << " " << stop1.color[1] << " "
								<< stop1.color[2] << " ] def\n" << "\t/C1 [ " << mid[0] << " " << mid[1] << " "
								<< mid[2] << " ] def\n" << "\t/N 1 def\n" << "end\n";

					sprintf( name, "Function%d", 2 * i );

					*m_stream << "/" << name << " 7 dict def " << name << " begin\n" << "\t/FunctionType 2 def\n" << "\t/Domain [ 0 1 ] def\n"
								<< "\t/C0 [ " << mid[0] << " " << mid[1] << " " << mid[2] << " ] def\n" << "\t/C1 [ " << stop2.color[0] << " "
								<< stop2.color[1] << " " << stop2.color[2] << " ] def\n" << "\t/N 1 def\n" << "end\n";
				}
			}
			if( grad.type() == VGradient::linear )
				*m_stream << "clip newpath\n" << "/DeviceRGB setcolorspace\n" << "<<\n" << "\t/ShadingType 2\n" << "\t/ColorSpace /DeviceRGB\n" << "\t/Coords [ "
							<< grad.origin().x() << " " << grad.origin().y() << " " << grad.vector().x() << " " << grad.vector().y() << " ]\n\t/Extend[ true true ]\n" << "\t/Function <<\n";
			else if( grad.type() == VGradient::radial )
			{
				double r = sqrt( pow( grad.vector().x() - grad.origin().x(), 2 ) + pow( grad.vector().y() - grad.origin().y(), 2 ) );
				*m_stream << "clip newpath\n" << "/DeviceRGB setcolorspace\n" << "<<\n" << "\t/ShadingType 3\n" << "\t/ColorSpace /DeviceRGB\n" << "\t/Coords [ "
			                << grad.origin().x() << " " << grad.origin().y() << " 0.0 " << grad.origin().x() << " " << grad.origin().y()
			                << " " << r << "]\n\t\t/Extend [ false true ]\n" << "\t/Function <<\n";
			}
			if( ramp.size() == 2 && ramp[0]->midPoint == 0.5 )
			{
				// Gradient with only two colors and symmetrical midpoint.
				VColorStop stop1 = *ramp[0];
				VColorStop stop2 = *ramp[1];
				*m_stream << "\t\t/FunctionType 2\n" << "\t\t/C0 [ " << stop1.color[0] << " " << stop1.color[1] << " " << stop1.color[2]
						  << " ]\n" << "\t\t/C1 [ " << stop2.color[0] << " " << stop2.color[1] << " " << stop2.color[2] << " ]\n" << "\t\t/N 1\n";
			}
			else if( ramp.size() > 2 || ramp.size() == 2 && ramp[0]->midPoint != 0.5 )
			{
				// Gradient with more than two colors or asymmetrical midpoint.
				*m_stream << "\t\t/FunctionType 3\n" << "\t\t/Functions [ ";
				for( uint i = 1; i < ( 2 * ramp.size() - 1 );i++ )
					*m_stream << "Function" << i << " ";
				*m_stream << "]\n" << "\t\t/Bounds [";
				for( uint i = 0;i < ramp.size() - 1;i++ )
				{
					VColorStop stop = *ramp[i];
					if( i > 0 )
						*m_stream << " " << stop.rampPoint;
					*m_stream << " " << ( stop.rampPoint + ( ramp[i + 1]->rampPoint - stop.rampPoint ) * stop.midPoint );
				}
				*m_stream << " ]\n" << "\t\t/Encode [ ";
				for( uint i = 0;i < 2 * ramp.size() - 2;i++ )
					*m_stream << "0 1 ";
				*m_stream << "]\n";
			}
			*m_stream << "\t\t/Domain [ " << ramp[0]->rampPoint << " "
					<< ramp[ramp.size() - 1]->rampPoint << " ]\n" << "\t>>\n" << ">>\n";
			// "shfill", "grestore".
			*m_stream << " shfill " << l1_grestore << "\n";
		}
	}
}

void
EpsExport::getColor( const VColor& color )
{
	VColor copy( color );
	copy.setColorSpace( VColor::rgb );

	*m_stream <<
		copy[0] << " " <<
		copy[1] << " " <<
		copy[2] << " " << l1_setrgbcolor;
}


#include "epsexport.moc"
