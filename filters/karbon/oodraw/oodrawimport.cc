/* This file is part of the KDE project
   Copyright (c) 2003 Rob Buis <buis@kde.org>

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

#include "oodrawimport.h"

#include <math.h>

#include <qdatetime.h>
#include <qfileinfo.h>
#include <qdir.h>

#include <kzip.h>
#include <karchive.h>
#include <kdebug.h>
#include <koUnit.h>
#include <koDocumentInfo.h>
#include <koDocument.h>
#include <ooutils.h>

#include <kgenericfactory.h>
#include <koFilterChain.h>
#include <koGlobal.h>
#include <stylestack.h>

#include <shapes/vellipse.h>
#include <shapes/vrectangle.h>
#include <shapes/vpolygon.h>

#include <core/vfill.h>
#include <core/vgroup.h>

typedef KGenericFactory<OoDrawImport, KoFilter> OoDrawImportFactory;
K_EXPORT_COMPONENT_FACTORY( liboodrawimport, OoDrawImportFactory( "kofficefilters" ) )


OoDrawImport::OoDrawImport( KoFilter *, const char *, const QStringList & )
    : KoFilter(),
      m_styles( 23, true )
{
	m_styles.setAutoDelete( true );
}

OoDrawImport::~OoDrawImport()
{
}

KoFilter::ConversionStatus OoDrawImport::convert( QCString const & from, QCString const & to )
{
    kdDebug() << "Entering Oodraw Import filter: " << from << " - " << to << endl;

    if( from != "application/vnd.sun.xml.draw" || to != "application/x-karbon" )
    {
        kdWarning() << "Invalid mimetypes " << from << " " << to << endl;
        return KoFilter::NotImplemented;
    }

    m_zip = new KZip(  m_chain->inputFile() );

    if ( !m_zip->open( IO_ReadOnly ) )
    {
        kdError(30518) << "Couldn't open the requested file "<< m_chain->inputFile() << endl;
        delete m_zip;
        return KoFilter::FileNotFound;
    }

    KoFilter::ConversionStatus preStatus = openFile();

    if( preStatus != KoFilter::OK )
    {
        m_zip->close();
        delete m_zip;
        return preStatus;
    }
/*QDomDocument docinfo;
  createDocumentInfo( docinfo );

// store document info
KoStoreDevice* out = m_chain->storageFile( "documentinfo.xml", KoStore::Write );
if( out )
{
QCString info = docinfo.toCString();
//kdDebug() << " info :" << info << endl;
// WARNING: we cannot use KoStore::write(const QByteArray&) because it gives an extra NULL character at the end.
out->writeBlock( info , info.length() );
}*/
    QDomDocument docinfo;
    createDocumentInfo( docinfo );
    // store document info
    KoStoreDevice* out = m_chain->storageFile( "documentinfo.xml", KoStore::Write );
    if( out )
    {
        QCString info = docinfo.toCString();
        //kdDebug(30518) << " info :" << info << endl;
        // WARNING: we cannot use KoStore::write(const QByteArray&) because it gives an extra NULL character at the end.
        out->writeBlock( info , info.length() );
    }

    convert();
    QDomDocument outdoc = m_document.saveXML();

    // store document content
    out = m_chain->storageFile( "maindoc.xml", KoStore::Write );
    if( out )
    {
        QCString content = outdoc.toCString();
        kdDebug() << " content :" << content << endl;
        out->writeBlock( content , content.length() );
    }
    m_zip->close();
    delete m_zip;

    kdDebug() << "######################## OoDrawImport::convert done ####################" << endl;

    return KoFilter::OK;
}

// Very related to OoWriterImport::createDocumentInfo
void OoDrawImport::createDocumentInfo( QDomDocument &docinfo )
{
    docinfo = KoDocument::createDomDocument( "document-info" /*DTD name*/, "document-info" /*tag name*/, "1.1" );

    OoUtils::createDocumentInfo(m_meta, docinfo);
    //kdDebug(30518) << " meta-info :" << m_meta.toCString() << endl;
}


// Very related to OoWriterImport::openFile()
KoFilter::ConversionStatus OoDrawImport::openFile()
{
    KoFilter::ConversionStatus status = loadAndParse( "content.xml", m_content );
    if ( status != KoFilter::OK )
    {
        kdError(30518) << "Content.xml could not be parsed correctly! Aborting!" << endl;
        return status;
    }

    // We do not stop if the following calls fail.
    QDomDocument styles;
    loadAndParse( "styles.xml", styles );
    loadAndParse( "meta.xml", m_meta );
    loadAndParse( "settings.xml", m_settings );

    emit sigProgress( 10 );
    createStyleMap( styles );

    return KoFilter::OK;
}

void OoDrawImport::convert()
{
	m_document.saveAsPath( false );

	QDomElement content = m_content.documentElement();

	// content.xml contains some automatic-styles that we need to store
	QDomNode automaticStyles = content.namedItem( "office:automatic-styles" );
	if( !automaticStyles.isNull() )
		insertStyles( automaticStyles.toElement() );

	QDomNode body = content.namedItem( "office:body" );
	if( body.isNull() )
		return;

	// we take the settings of the first slide for the whole document.
	QDomNode drawPage = body.namedItem( "draw:page" );
	if( drawPage.isNull() ) // no pages? give up.
		return;

	QDomElement dp = drawPage.toElement();
	QDomElement *master = m_styles[dp.attribute( "draw:master-page-name" )];
	QDomElement *style = m_styles[master->attribute( "style:page-master-name" )];
	QDomElement properties = style->namedItem( "style:properties" ).toElement();

	if( properties.isNull() )
	{
		m_document.setWidth( 550.0 );
		m_document.setHeight( 841.0 );
	}
	else
	{
		m_document.setWidth( KoUnit::parseValue(properties.attribute( "fo:page-width" ) ) );
		m_document.setHeight( KoUnit::parseValue(properties.attribute( "fo:page-height" ) ) );
	}

    // parse all pages
    for( QDomNode drawPage = body.firstChild(); !drawPage.isNull(); drawPage = drawPage.nextSibling() )
    {
        QDomElement dp = drawPage.toElement();
        m_styleStack.clear(); // remove all styles
        fillStyleStack( dp );
        //m_styleStack.setPageMark();

        // set the pagetitle
        //QDomElement titleElement = doc.createElement( "Title" );
        //titleElement.setAttribute( "title", dp.attribute( "draw:name" ) );
        //pageTitleElement.appendChild( titleElement );

	parseGroup( 0L, dp );
    }
}


KoFilter::ConversionStatus OoDrawImport::loadAndParse(const QString& filename, QDomDocument& doc)
{
     return OoUtils::loadAndParse( filename, doc, m_zip);
}

void
OoDrawImport::parseGroup( VGroup *parent, const QDomElement& parentobject )
{
	// parse all objects
	for( QDomNode object = parentobject.firstChild(); !object.isNull(); object = object.nextSibling() )
	{
		QDomElement o = object.toElement();
		QString name = o.tagName();
		QString drawID = o.attribute("draw:id");
		VObject *obj = 0L;

		if( name == "draw:g" ) // polyline
		{
			storeObjectStyles( o );
			VGroup *group = new VGroup( parent );
			appendPen( *group );
			appendBrush( *group );
			obj = group;
			parseGroup( group, o );
		}
		else if( name == "draw:rect" ) // rectangle
		{
			storeObjectStyles( o );
			double x = KoUnit::parseValue( o.attribute( "svg:x" ) );
			double y = ymirror( KoUnit::parseValue( o.attribute( "svg:y" ) ) );
			double w = KoUnit::parseValue( o.attribute( "svg:width" ) );
			double h = KoUnit::parseValue( o.attribute( "svg:height" ) );
			int corner = static_cast<int>( KoUnit::parseValue( o.attribute( "draw:corner-radius" ) ) );
			VRectangle *rect = new VRectangle( parent, KoPoint( x, y ), w, h, corner );
			appendPen( *rect );
			appendBrush( *rect );
			obj = rect;
		}
		else if( name == "draw:circle" || name == "draw:ellipse" )
		{
			storeObjectStyles( o );
			double w = KoUnit::parseValue( o.attribute( "svg:width" ) );
			double h = KoUnit::parseValue( o.attribute( "svg:height" ) );
			double x = KoUnit::parseValue( o.attribute( "svg:x" ) );
			double y = ymirror( KoUnit::parseValue( o.attribute( "svg:y" ) ) ) - h;
			double start = o.attribute( "draw:start-angle" ).toDouble();
			double end = o.attribute( "draw:end-angle" ).toDouble();
			QString kind = o.attribute( "draw:kind" );
			VEllipse::VEllipseType type = VEllipse::full;
			if( !kind.isEmpty() )
			{
				if( kind == "section" )
					type = VEllipse::cut;
				else if( kind == "cut" )
					type = VEllipse::section;
				else if( kind == "arc" )
					type = VEllipse::arc;
			}
			VEllipse *ellipse = new VEllipse( parent, KoPoint( x, y ), w, h, type, start, end );
			appendPen( *ellipse );
			// arc has no brush
			if( kind != "arc" )
				appendBrush( *ellipse );
			obj = ellipse;
		}
		else if( name == "draw:line" ) // line
		{
			storeObjectStyles( o );
			VPath *line = new VPath( parent );
			double x1 = KoUnit::parseValue( o.attribute( "svg:x1" ) );
			double y1 = ymirror( KoUnit::parseValue( o.attribute( "svg:y1" ) ) );
			double x2 = KoUnit::parseValue( o.attribute( "svg:x2" ) );
			double y2 = ymirror( KoUnit::parseValue( o.attribute( "svg:y2" ) ) );
			line->moveTo( KoPoint( x1, y1 ) );
			line->lineTo( KoPoint( x2, y2 ) );
			appendPen( *line );
			appendBrush( *line );
			obj = line;
		}
		else if( name == "draw:polyline" ) // polyline
		{
			storeObjectStyles( o );
			VPath *polyline = new VPath( parent );
			appendPoints( *polyline, o);
			appendPen( *polyline );
			appendBrush( *polyline );
			obj = polyline;
		}
		else if( name == "draw:polygon" ) // polygon
		{
			storeObjectStyles( o );
			//VPolygon *polygon = new VPolygon( parent );
			//polygon->load( o );
			VPath *polygon = new VPath( parent );
			appendPoints( *polygon, o );
			appendPen( *polygon );
			appendBrush( *polygon );
			obj = polygon;
		}
		else if( name == "draw:path" ) // path
		{
			storeObjectStyles( o );
			VPath *path = new VPath( parent );
			path->loadSvgPath( o.attribute( "svg:d" ) );
			KoRect rect = parseViewBox( o );
			double x = KoUnit::parseValue( o.attribute( "svg:x" ) );
			double y = ymirror( KoUnit::parseValue( o.attribute( "svg:y" ) ) );
			double w = KoUnit::parseValue( o.attribute( "svg:width" ) );
			double h = KoUnit::parseValue( o.attribute( "svg:height" ) );
			QWMatrix mat;
			mat.translate( x, y );
			mat.scale( w / rect.width(), -h / rect.height() );
			path->transform( mat );
			appendPen( *path );
			appendBrush( *path );
			obj = path;
		}
/*else if( name == "draw:image" ) // image
{
storeObjectStyles( o );
e = doc.createElement( "OBJECT" );
e.setAttribute( "type", 0 );
appendImage( doc, e, pictureElement, o );
}*/
		else
		{
			kdDebug() << "Unsupported object '" << name << "'" << endl;
			continue;
		}
		if( parent && obj )
			parent->append( obj );
		else if( obj )
			m_document.append( obj );
	}
}

void
OoDrawImport::appendPen( VObject &obj )
{
	if( m_styleStack.hasAttribute( "draw:stroke" ) )
	{
		VStroke stroke;

		if( m_styleStack.attribute( "draw:stroke" ) == "none" )
			stroke.setType( VStroke::none );
		else if( m_styleStack.attribute( "draw:stroke" ) == "solid" )
			stroke.setType( VStroke::solid );
		else if( m_styleStack.attribute( "draw:stroke" ) == "dash" )
		{
			QValueList<float> dashes;
			stroke.setType( VStroke::solid );
			QString style = m_styleStack.attribute( "draw:stroke-dash" );
			if( style == "Ultrafine Dashed" ||
				style == "Fine Dashed (var)" || style == "Dashed (var)" )
				stroke.dashPattern().setArray( dashes << 2 << 2 );
			else if( style == "Fine Dashed" )
				stroke.dashPattern().setArray( dashes << 10 << 10 );
			else if( style == "Fine Dotted" || style == "Ultrafine Dotted (var)" ||
				style == "Line with Fine Dots" )
				stroke.dashPattern().setArray( dashes << 2 << 10 );
			else if( style == "3 Dashes 3 Dots (var)" || style == "Ultrafine 2 Dots 3 Dashes" )
				stroke.dashPattern().setArray( dashes << 3 << 3 );
			else if( style == "2 Dots 1 Dash" )
				stroke.dashPattern().setArray( dashes << 2 << 1 );
		}
		if( m_styleStack.hasAttribute( "svg:stroke-width" ) )
		{
			double lwidth = KoUnit::parseValue( m_styleStack.attribute( "svg:stroke-width" ) );
			if( lwidth == 0 )
				lwidth = 1.0;
			stroke.setLineWidth( lwidth );
		}
		if( m_styleStack.hasAttribute( "svg:stroke-color" ) )
		{
			VColor c;
			parseColor( c, m_styleStack.attribute( "svg:stroke-color" ) );
			stroke.setColor( c );
		}

		obj.setStroke( stroke );
	}
}

void
OoDrawImport::appendBrush( VObject &obj )
{
	if( m_styleStack.hasAttribute( "draw:fill" ) )
	{
		const QString fill = m_styleStack.attribute( "draw:fill" );
		VFill f;

		if( fill == "solid" )
		{
			f.setType( VFill::solid );
			if( m_styleStack.hasAttribute( "draw:fill-color" ) )
			{
				VColor c;
				parseColor( c, m_styleStack.attribute( "draw:fill-color" ) );
				f.setColor( c );
			}
		}
		else if( fill == "gradient" )
		{
			VGradient gradient;
			gradient.clearStops();
			gradient.setRepeatMethod( VGradient::none );
			QString style = m_styleStack.attribute( "draw:fill-gradient-name" );

			QDomElement* draw = m_draws[style];
			if( draw )
			{
				double border = 0.0;
				if( draw->hasAttribute( "draw:border" ) )
					border += draw->attribute( "draw:border" ).remove( '%' ).toDouble() / 100.0;
				VColor c;
				parseColor( c, draw->attribute( "draw:start-color" ) );
				gradient.addStop( c, border, 0.5 );
				parseColor( c, draw->attribute( "draw:end-color" ) );
				gradient.addStop( c, 1.0, 0.5 );

				QString type = draw->attribute( "draw:style" );
				if( type == "linear" || type == "axial" )
				{
					gradient.setType( VGradient::linear );
					int angle = draw->attribute( "draw:angle" ).toInt() / 10;

					// make sure the angle is between 0 and 359
					angle = abs( angle );
					angle -= ( (int) ( angle / 360 ) ) * 360;

					// What we are trying to do here is to find out if the given
					// angle belongs to a horizontal, vertical or diagonal gradient.
					int lower, upper, nearAngle = 0;
					for ( lower = 0, upper = 45; upper < 360; lower += 45, upper += 45 )
					{
						if( upper >= angle )
						{
							int distanceToUpper = abs( angle - upper );
							int distanceToLower = abs( angle - lower );
							nearAngle = distanceToUpper > distanceToLower ? lower : upper;
							break;
						}
					}
					KoRect rect = obj.boundingBox();
					KoPoint origin, vector;
					// nearAngle should now be one of: 0, 45, 90, 135, 180...
					kdDebug() << "nearAngle: " << nearAngle << endl;
					if( nearAngle == 0 || nearAngle == 180 )
					{
						origin.setX( rect.x() + rect.width() );
						origin.setY( rect.y() + rect.height());
						vector.setX( rect.x() + rect.width() );
						vector.setY( rect.y() );
					}
					else if( nearAngle == 90 || nearAngle == 270 )
					{
						origin.setX( rect.x() );
						origin.setY( rect.y() + rect.height() );
						vector.setX( rect.x() + rect.width() );
						vector.setY( rect.y() + rect.height() );
					}
					else if( nearAngle == 45 || nearAngle == 225 )
					{
						origin.setX( rect.x() );
						origin.setY( rect.y() );
						vector.setX( rect.x() + rect.width() );
						vector.setY( rect.y() + rect.height() );
					}
					else if( nearAngle == 135 || nearAngle == 315 )
					{
						origin.setX( rect.x() + rect.width() );
						origin.setY( rect.y() + rect.height() );
						vector.setX( rect.x() );
						vector.setY( rect.y() );
					}

					gradient.setOrigin( origin );
					gradient.setVector( vector );
				}
				else if( type == "radial" || type == "ellipsoid" )
				{
					gradient.setType( VGradient::radial );
//else if( type == "square" || type == "rectangular" )
//gradient.setAttribute( "type", 6 ); // rectangle
//else if( type == "axial" )
//gradient.setAttribute( "type", 7 ); // pipecross

					// Hard to map between x- and y-center settings of oodraw
					// and (un-)balanced settings of kpresenter. Let's try it.
					double x, y;
					if( draw->hasAttribute( "draw:cx" ) )
						x = draw->attribute( "draw:cx" ).remove( '%' ).toDouble() / 100.0;
					else
						x = 0.5;

					if( draw->hasAttribute( "draw:cy" ) )
						y = draw->attribute( "draw:cy" ).remove( '%' ).toDouble() / 100.0;
					else
						y = 0.5;

					KoRect rect = obj.boundingBox();
					gradient.setOrigin( KoPoint( rect.x() + x * rect.width(),
												 rect.y() + y * rect.height() ) );
					gradient.setFocalPoint( KoPoint( rect.x() + x * rect.width(),
													 rect.y() + y * rect.height() ) );
					gradient.setVector( KoPoint( rect.x() + rect.width(),
												 rect.y() + y * rect.height() ) );
				}
				f.gradient() = gradient;
				f.setType( VFill::grad );
			}
		}
		obj.setFill( f );
	}
/*else if( fill == "hatch" )
{
QDomElement brush = doc.createElement( "BRUSH" );
QString style = m_styleStack.attribute( "draw:fill-hatch-name" );
if( style == "Black 0 Degrees" )
brush.setAttribute( "style", 9 );
else if( style == "Black 90 Degrees" )
brush.setAttribute( "style", 10 );
else if( style == "Red Crossed 0 Degrees" || style == "Blue Crossed 0 Degrees" )
brush.setAttribute( "style", 11 );
else if( style == "Black 45 Degrees" || style == "Black 45 Degrees Wide" )
brush.setAttribute( "style", 12 );
else if( style == "Black -45 Degrees" )
brush.setAttribute( "style", 13 );
else if( style == "Red Crossed 45 Degrees" || style == "Blue Crossed 45 Degrees" )
brush.setAttribute( "style", 14 );

QDomElement* draw = m_draws[style];
if( draw && draw->hasAttribute( "draw:color" ) )
brush.setAttribute( "color", draw->attribute( "draw:color" ) );
e.appendChild( brush );
}*/
}

void
OoDrawImport::createStyleMap( QDomDocument &docstyles )
{
	QDomElement styles = docstyles.documentElement();
	if( styles.isNull() )
		return;

	QDomNode fixedStyles = styles.namedItem( "office:styles" );
	if( !fixedStyles.isNull() )
	{
		insertDraws( fixedStyles.toElement() );
		insertStyles( fixedStyles.toElement() );
	}
	QDomNode automaticStyles = styles.namedItem( "office:automatic-styles" );
	if( !automaticStyles.isNull() )
		insertStyles( automaticStyles.toElement() );

	QDomNode masterStyles = styles.namedItem( "office:master-styles" );
	if( !masterStyles.isNull() )
		insertStyles( masterStyles.toElement() );
}

void
OoDrawImport::insertDraws( const QDomElement& styles )
{
	for( QDomNode n = styles.firstChild(); !n.isNull(); n = n.nextSibling() )
	{
		QDomElement e = n.toElement();

		if( !e.hasAttribute( "draw:name" ) )
			continue;

		QString name = e.attribute( "draw:name" );
		m_draws.insert( name, new QDomElement( e ) );
	}
}


void
OoDrawImport::insertStyles( const QDomElement& styles )
{
	for ( QDomNode n = styles.firstChild(); !n.isNull(); n = n.nextSibling() )
	{
		QDomElement e = n.toElement();

		if( !e.hasAttribute( "style:name" ) )
			continue;

		QString name = e.attribute( "style:name" );
		m_styles.insert( name, new QDomElement( e ) );
		//kdDebug() << "Style: '" << name << "' loaded " << endl;
	}
}

void
OoDrawImport::fillStyleStack( const QDomElement& object )
{
    // find all styles associated with an object and push them on the stack
    if( object.hasAttribute( "presentation:style-name" ) )
        addStyles( m_styles[object.attribute( "presentation:style-name" )] );

    if( object.hasAttribute( "draw:style-name" ) )
        addStyles( m_styles[object.attribute( "draw:style-name" )] );

    if( object.hasAttribute( "draw:text-style-name" ) )
        addStyles( m_styles[object.attribute( "draw:text-style-name" )] );

    if( object.hasAttribute( "text:style-name" ) )
        addStyles( m_styles[object.attribute( "text:style-name" )] );
}

void
OoDrawImport::addStyles( const QDomElement* style )
{
    // this function is necessary as parent styles can have parents themself
    if( style->hasAttribute( "style:parent-style-name" ) )
        addStyles( m_styles[style->attribute( "style:parent-style-name" )] );

    m_styleStack.push( *style );
}

void
OoDrawImport::storeObjectStyles( const QDomElement& object )
{
    //m_styleStack.clearPageMark(); // remove styles of previous object
    fillStyleStack( object );
    //m_styleStack.setObjectMark();
}

KoRect
OoDrawImport::parseViewBox( const QDomElement& object )
{
	KoRect rect;
	if( !object.attribute( "svg:viewBox" ).isEmpty() )
	{
		// allow for viewbox def with ',' or whitespace
		QString viewbox( object.attribute( "svg:viewBox" ) );
		QStringList points = QStringList::split( ' ', viewbox.replace( ',', ' ').simplifyWhiteSpace() );

		rect.setX( points[0].toFloat() );
		rect.setY( points[1].toFloat() );
		rect.setWidth( points[2].toFloat() );
		rect.setHeight( points[3].toFloat() );
	}
	return rect;
}

void
OoDrawImport::appendPoints(VPath &path, const QDomElement& object)
{
	double x = KoUnit::parseValue( object.attribute( "svg:x" ) );
	double y = KoUnit::parseValue( object.attribute( "svg:y" ) );
	double w = KoUnit::parseValue( object.attribute( "svg:width" ) );
	double h = KoUnit::parseValue( object.attribute( "svg:height" ) );

	KoRect rect = parseViewBox( object );
	rect.setX( rect.x() + x );
	rect.setY( rect.y() + y );

	QStringList ptList = QStringList::split( ' ', object.attribute( "draw:points" ) );

	QString pt_x, pt_y;
	double tmp_x, tmp_y;
	KoPoint point;
	bool bFirst = true;
	for( QStringList::Iterator it = ptList.begin(); it != ptList.end(); ++it )
	{
		tmp_x = rect.x() + ( (*it).section( ',', 0, 0 ).toInt() * w ) / rect.width();
		tmp_y = rect.y() + ( (*it).section( ',', 1, 1 ).toInt() * h ) / rect.height();

		point.setX( tmp_x );
		point.setY( ymirror( tmp_y ) );
		if( bFirst )
		{
			path.moveTo( point );
			bFirst = false;
		}
		else
			path.lineTo( point );
    }
}

void
OoDrawImport::parseColor( VColor &color, const QString &s )
{
	if( s.startsWith( "rgb(" ) )
	{
		QString parse = s.stripWhiteSpace();
		QStringList colors = QStringList::split( ',', parse );
		QString r = colors[0].right( ( colors[0].length() - 4 ) );
		QString g = colors[1];
		QString b = colors[2].left( ( colors[2].length() - 1 ) );

		if( r.contains( "%" ) )
		{
			r = r.left( r.length() - 1 );
			r = QString::number( int( ( double( 255 * r.toDouble() ) / 100.0 ) ) );
		}

		if( g.contains( "%" ) )
		{
			g = g.left( g.length() - 1 );
			g = QString::number( int( ( double( 255 * g.toDouble() ) / 100.0 ) ) );
		}

		if( b.contains( "%" ) )
		{
			b = b.left( b.length() - 1 );
			b = QString::number( int( ( double( 255 * b.toDouble() ) / 100.0 ) ) );
		}

		QColor c( r.toInt(), g.toInt(), b.toInt() );
		color.set( c.red() / 255.0, c.green() / 255.0, c.blue() / 255.0 );
	}
	else
	{
		QString rgbColor = s.stripWhiteSpace();
		QColor c;
		if( rgbColor.startsWith( "#" ) )
			c.setNamedColor( rgbColor );
		//else
		//	c = parseColor( rgbColor );
		color.set( c.red() / 255.0, c.green() / 255.0, c.blue() / 255.0 );
	}
}

double
OoDrawImport::ymirror( double y )
{
	return m_document.height() - y;
}

#include "oodrawimport.moc"
