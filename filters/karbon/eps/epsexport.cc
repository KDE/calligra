/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers
*/

#include <qcstring.h>
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
#include "vdashpattern.h"
#include "vdocument.h"
#include "vfill.h"
#include "vgroup.h"
#include "vlayer.h"
#include "vpath.h"
#include "vsegment.h"
#include "vsegmentlist.h"
#include "vselection.h"
#include "vstroke.h"

#include <kdebug.h>


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

	// Ask questions about ps level etc:
	EpsExportDlg* dialog = new EpsExportDlg();

	if( dialog->exec() )
	{
		// Which postscript level to support?
		m_psLevel = dialog->psLevel();

		QFile fileOut( m_chain->outputFile() );
		if( !fileOut.open( IO_WriteOnly ) )
		{
			delete storeIn;
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

	delete( dialog );

	storeIn->close();
	delete storeIn;

	return KoFilter::OK;
}

void
EpsExport::visitVDocument( VDocument& document )
{
	// Select all objects:
	document.select();

	// Get the bounding box of all selected objects:
	const KoRect& rect = document.selection()->boundingBox();

	// Print a header:
	*m_stream <<
		"%!PS-Adobe-2.0 EPSF-1.2\n"
		"%%BoundingBox: "
			<< qRound( rect.left()   ) << " "	// The standard says: integer values.
			<< qRound( rect.top()    ) << " "
			<< qRound( rect.right()  ) << " "
			<< qRound( rect.bottom() ) << "\n"
		"%%HiResBoundingBox: "
			<< rect.left()   << " "
			<< rect.top()    << " "
			<< rect.right()  << " "
			<< rect.bottom() << "\n"
		"%%Creator: Karbon14 EPS 0.5"
	<< endl;

	// We dont need the selection anymore:
	document.deselect();


	// Process document info:
	KoStoreDevice* storeIn;
	storeIn = m_chain->storageFile( "documentinfo.xml", KoStore::Read );

	if( storeIn )
	{
		QDomDocument domIn;
		domIn.setContent( storeIn );
		storeIn->close();

		KoDocumentInfo docInfo;
		docInfo.load( domIn );

		KoDocumentInfoAuthor* authorPage =
			static_cast<KoDocumentInfoAuthor*>( docInfo.page( "author" ) );

		*m_stream <<
		// TODO:
			"%%CreationDate: (12/24/01) (12:34 PM)\n"
			"%%For: (" << authorPage->fullName() << ") (" << authorPage->company() << ")\n"
			"%%Title: (" << docInfo.title() << ")"
		<< endl;
	}


	// Print some definitions:
	*m_stream <<
		"\n"
		"/N {newpath} def\n"
		"/C {closepath} def\n"
		"/m {moveto} def\n"
		"/c {curveto} def\n"
		"/l {lineto} def\n"
		"/s {stroke} def\n"
		"/f {fill} def\n"
		"/w {setlinewidth} def\n"
		"/d {setdash} def\n"
		"/r {setrgbcolor} def\n"
		"/S {gsave} def\n"
		"/R {grestore} def\n"
	<< endl;


	// Export layers:
	VLayerListIterator itr( document.layers() );
	for( ; itr.current(); ++itr )
		itr.current()->accept( *this );


	// Finished:
	*m_stream <<
		"%%EOF"
	<< endl;
}

void
EpsExport::visitVGroup( VGroup& group )
{
	// Export objects:
	VObjectListIterator itr( group.objects() );
	for( ; itr.current(); ++itr )
		itr.current()->accept( *this );
}

void
EpsExport::visitVLayer( VLayer& layer )
{
	// Export objects:
	VObjectListIterator itr( layer.objects() );
	for( ; itr.current(); ++itr )
		itr.current()->accept( *this );
}

void
EpsExport::visitVPath( VPath& path )
{
	// Export segmentlists:
	VSegmentListListIterator itr( path.segmentLists() );
	for( ; itr.current(); ++itr )
		itr.current()->accept( *this );

	getFill( *path.fill() );
	getStroke( *path.stroke() );

	*m_stream << endl;
}

void
EpsExport::visitVSegmentList( VSegmentList& segmentList )
{
	*m_stream << "N\n";

	// Export segments:
	VSegmentListIterator itr( segmentList );
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
					"c\n";
			break;
			case VSegment::line:
				*m_stream <<
					itr.current()->knot().x() << " " <<
					itr.current()->knot().y() << " " <<
					"l\n";
			break;
			case VSegment::begin:
				*m_stream <<
					itr.current()->knot().x() << " " <<
					itr.current()->knot().y() << " " <<
					"m\n";
			break;
			default:
			break;
		}
	}

	if( segmentList.isClosed() )
		*m_stream << "C\n";
}

void
EpsExport::getStroke( const VStroke& stroke )
{
	if( stroke.type() != VStroke::none )
	{
		// gsave:
		*m_stream << "S ";

		// dash pattern:
		*m_stream << "[";

		const QValueList<float>&
			array( stroke.dashPattern().array() );

		QValueListConstIterator<float> itr = array.begin();
		for( ; itr != array.end(); ++itr )
			 *m_stream << *itr << " ";

		*m_stream << "] " << stroke.dashPattern().offset()
			<< " d ";

		getColor( stroke.color() );

		// setlinewidth, stroke, grestore:
		*m_stream << " " << stroke.lineWidth() << " w s R\n";
	}
}

void
EpsExport::getFill( const VFill& fill )
{
	if( fill.type() != VFill::none )
	{
		// gsave:
		*m_stream << "S ";

		// setrgbcolor:
		getColor( fill.color() );

		// fill, grestore:
		*m_stream << " f R\n";
	}
}

void
EpsExport::getColor( const VColor& color )
{
	VColor copy( color );
	copy.setColorSpace( VColor::rgb );

	*m_stream <<
		copy.value( 0 ) << " " <<
		copy.value( 1 ) << " " <<
		copy.value( 2 ) << " r";
}

#include "epsexport.moc"

