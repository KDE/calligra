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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <qapplication.h>
#include <qcstring.h>
#include <qdatetime.h>	// For creation date/time.
#include <qdom.h>
#include <qfile.h>
#include <qstring.h>
#include <qvaluelist.h>

#include <kgenericfactory.h>
#include <koDocumentInfo.h>
#include <koFilter.h>
#include <koFilterChain.h>
#include <koStore.h>

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

#include <kdebug.h>


//Define level1 operators:
static char l1_newpath		= 'N';
static char l1_closepath	= 'C';
static char l1_moveto		= 'm';
static char l1_curveto		= 'c';
static char l1_lineto		= 'l';
static char l1_stroke		= 's';
static char l1_fill			= 'f';
static char l1_eofill		= 'e';
static char l1_setlinewidth	= 'w';
static char l1_setdash		= 'd';
static char l1_setrgbcolor	= 'r';
static char l1_gsave		= 'S';
static char l1_grestore		= 'R';


class EpsExportFactory : KGenericFactory<EpsExport, KoFilter>
{
public:
	EpsExportFactory( void )
		: KGenericFactory<EpsExport, KoFilter>( "karbonepsexport" )
	{}

protected:
	virtual void setupTranslations( void )
	{
		KGlobal::locale()->insertCatalogue( "karbonepsfilter" );
	}
};


K_EXPORT_COMPONENT_FACTORY( libkarbonepsexport, EpsExportFactory() );


EpsExport::EpsExport( KoFilter*, const char*, const QStringList& )
	: KoFilter()
{
}

KoFilter::ConversionStatus
EpsExport::convert( const QCString& from, const QCString& to )
{
	if ( to != "image/x-eps" || from != "application/x-karbon" )
	{
		return KoFilter::NotImplemented;
	}


	KoStoreDevice* storeIn = m_chain->storageFile( "root", KoStore::Read );

	if( !storeIn )
		return KoFilter::StupidError;


	KoFilter::ConversionStatus status = KoFilter::OK;

	// Ask questions about PS level etc:
	EpsExportDlg* dialog = new EpsExportDlg();

	QApplication::setOverrideCursor( Qt::arrowCursor );

	if( dialog->exec() )
	{
		// Which PostScript level to support?
		m_psLevel = dialog->psLevel() + 1;

		QFile fileOut( m_chain->outputFile() );
		if( !fileOut.open( IO_WriteOnly ) )
		{
			QApplication::restoreOverrideCursor();
			delete( dialog );

			return KoFilter::StupidError;
		}

		QDomDocument domIn;
		domIn.setContent( storeIn );
		QDomElement docNode = domIn.documentElement();

		m_stream = new QTextStream( &fileOut );

		// Load the document and export it:
		VDocument doc;
		doc.load( docNode );
		doc.accept( *this );

		delete m_stream;
		fileOut.close();
	}
	else
	{
		// Dialog cancelled:
		status = KoFilter::UserCancelled;
	}

	QApplication::restoreOverrideCursor();
	delete( dialog );

	return status;
}

void
EpsExport::visitVDocument( VDocument& document )
{
	// Select all objects:
	document.selection()->append();

	// Get the bounding box of all selected objects:
	const KoRect& rect = document.selection()->boundingBox();

	// Print a header:
	*m_stream <<
		"%!PS-Adobe-3.0 EPSF-3.0\n"
		"%%BoundingBox: "
		// Round down:
			<< qRound( rect.left()   - 0.5 ) << " "
			<< qRound( rect.top()    - 0.5 ) << " "
		// Round up:
			<< qRound( rect.right()  + 0.5 ) << " "
			<< qRound( rect.bottom() + 0.5 ) << "\n"
		"%%HiResBoundingBox: "
			<< rect.left()   << " "
			<< rect.top()    << " "
			<< rect.right()  << " "
			<< rect.bottom() << "\n"
		"%%Creator: Karbon14 EPS 0.5"
	<< endl;

	// We dont need the selection anymore:
	document.selection()->clear();


	// Process document info:
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

		// Get creation date/time aka "now":
		QDateTime now( QDateTime::currentDateTime() );

		*m_stream <<
			"%%CreationDate: (" << now.toString( Qt::LocalDate ) << ")\n"
			"%%For: (" << authorPage->fullName() << ") (" << authorPage->company() << ")\n"
			"%%Title: (" << docInfo.title() << ")"
		<< endl;
	}


	// Print operator definitions:
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

	// Export layers:
	VVisitor::visitVDocument( document );

	// Finished:
	*m_stream <<
		"%%EOF"
	<< endl;
}

void
EpsExport::visitVComposite( VComposite& composite )
{
	VVisitor::visitVComposite( composite );

	getFill( *composite.fill() );
	getStroke( *composite.stroke() );

	*m_stream << endl;
}

void
EpsExport::visitVPath( VPath& path )
{
	*m_stream << l1_newpath << "\n";

	// Export segments:
	VPathIterator itr( path );
	for( ; itr.current(); ++itr )
	{
		switch( itr.current()->type() )
		{
			case VSegment::curve:
				*m_stream <<
					itr.current()->ctrlPoint1().x() << " " <<
					itr.current()->ctrlPoint1().y() << " " <<
					itr.current()->ctrlPoint2().x() << " " <<
					itr.current()->ctrlPoint2().y() << " " <<
					itr.current()->knot().x() << " " <<
					itr.current()->knot().y() << " " <<
					l1_curveto << "\n";
			break;
			case VSegment::line:
				*m_stream <<
					itr.current()->knot().x() << " " <<
					itr.current()->knot().y() << " " <<
					l1_lineto << "\n";
			break;
			case VSegment::begin:
				*m_stream <<
					itr.current()->knot().x() << " " <<
					itr.current()->knot().y() << " " <<
					l1_moveto << "\n";
			break;
			default:
			break;
		}
	}

	if( path.isClosed() )
		*m_stream << l1_closepath << "\n";
}

void
EpsExport::getStroke( const VStroke& stroke )
{
	// Solid stroke:
	if( stroke.type() == VStroke::solid )
	{
		// dash pattern:
		*m_stream << "[";

		const QValueList<float>&
			array( stroke.dashPattern().array() );

		QValueListConstIterator<float> itr = array.begin();
		for( ; itr != array.end(); ++itr )
			 *m_stream << *itr << " ";

		*m_stream <<
			"] " << stroke.dashPattern().offset() <<
			" "  << l1_setdash << " ";

		getColor( stroke.color() );

		// setlinewidth, stroke:
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
	// Solid fill:
	if( fill.type() == VFill::solid )
	{
		// gsave:
		*m_stream << l1_gsave << " ";

		// setrgbcolor:
		getColor( fill.color() );

		// fill, grestore:
		*m_stream << " " << l1_fill << " " << l1_grestore << "\n";
	}
	// Gradient:
	else if( fill.type() == VFill::grad )
	{
		if( m_psLevel == 3 )
		{
			
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

