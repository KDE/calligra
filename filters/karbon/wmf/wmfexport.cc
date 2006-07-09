/* This file is part of the KDE project
 * Copyright (c) 2003 thierry lorthiois (lorthioist@wanadoo.fr)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License version 2 as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include <config.h>
#include <qdom.h>
#include <qcstring.h>
#include <kdebug.h>
#include <kgenericfactory.h>
#include <KoFilterChain.h>
#include <KoStore.h>
#include <KoStoreDevice.h>
#include "vdocument.h"
#include "vcolor.h"
#include "vcomposite.h"
#include "vdashpattern.h"
#include "vdocument.h"
#include "vlayer.h"
#include "vpath.h"
#include "vsegment.h"
#include "vfill.h"
#include "vstroke.h"
#include "vtext.h"
#include "vflattencmd.h"

#include "wmfexport.h"
#include "kowmfwrite.h"

/*
TODO: bs.wmf stroke in red with MSword and in brown with Kword ??
*/

typedef KGenericFactory<WmfExport, KoFilter> WmfExportFactory;
K_EXPORT_COMPONENT_FACTORY( libwmfexport, WmfExportFactory( "kofficefilters" ) )


WmfExport::WmfExport( KoFilter *, const char *, const QStringList&) :
        KoFilter()
{
}

WmfExport::~WmfExport()
{
}

KoFilter::ConversionStatus WmfExport::convert( const QCString& from, const QCString& to )
{
    if( to != "image/x-wmf" || from != "application/x-karbon" ) {
        return KoFilter::NotImplemented;
    }

    KoStoreDevice* storeIn = m_chain->storageFile( "root", KoStore::Read );

    if( !storeIn ) {
        return KoFilter::StupidError;
    }

    // open Placeable Wmf file
    mWmf = new KoWmfWrite( m_chain->outputFile() );
    if( !mWmf->begin() ) {
        delete mWmf;
        return KoFilter::WrongFormat;
    }

    QDomDocument domIn;
    domIn.setContent( storeIn );
    QDomElement docNode = domIn.documentElement();

    // Load the document.
    mDoc = new VDocument;
    mDoc->load( docNode );

    // Process the document.
    mDoc->accept( *this );

    mWmf->end();

    delete mWmf;
    delete mDoc;

    return KoFilter::OK;
}


void WmfExport::visitVDocument( VDocument& document ) {
    int width;
    int height;

    mDoc = &document;
    mListPa.setAutoDelete( true );

    // resolution
    mDpi = 1000;
    width = (int)(POINT_TO_INCH( document.width() ) * mDpi);
    height = (int)(POINT_TO_INCH( document.height() ) * mDpi);

    mWmf->setDefaultDpi( mDpi );
    mWmf->setWindow( 0, 0, width, height );

    if ( (document.width() != 0) && (document.height() != 0) ) {
        mScaleX = (double)width / document.width();
        mScaleY = (double)height / document.height();
    }

    // Export layers.
    VVisitor::visitVDocument( document );

}


void WmfExport::visitVPath( VPath& composite ) {
    QPen      pen;
    QBrush    brush;

    getPen( pen, composite.stroke() );
    getBrush( brush, composite.fill() );

    VVisitor::visitVPath( composite );

    if ( mListPa.count() > 0 ) {
        mWmf->setPen( pen );
        if( (brush.style() == Qt::NoBrush)
            && (mListPa.count() == 1) ) {
            mWmf->drawPolyline( *mListPa.first() );
        }
        else {
            mWmf->setBrush( brush );

            if ( mListPa.count() == 1 ) {
                mWmf->drawPolygon( *mListPa.first() );
            }
            else {
                // combined path
                mWmf->drawPolyPolygon( mListPa );
            }
        }
    }
    mListPa.clear();
}


// Export segment.
void WmfExport::visitVSubpath( VSubpath& path ) {
    VSubpath *newPath;
    VSubpathIterator itr( path );
    VFlattenCmd cmd( 0L, INCH_TO_POINT(0.3 / (double)mDpi) );
    QPointArray *pa = new QPointArray( path.count() );
    int  nbrPoint=0;      // number of points in the path

    for( ; itr.current(); ++itr ) {
	VSegment *segment= itr.current();
	if (segment->isCurve()) {
	    newPath = new VSubpath( mDoc );

	    // newPath duplicate the list of curve
	    newPath->moveTo( itr.current()->prev()->knot() );
	    newPath->append( itr.current()->clone() );
	    while( itr.current()->next() ) {
		if ( itr.current()->next()->isCurve() ) {
		    newPath->append( itr.current()->next()->clone() );
		}
		else {
		    break;
		}
		++itr;
	    }

	    // flatten the curve
	    cmd.visit( *newPath );

	    // adjust the number of points
	    pa->resize( pa->size() + newPath->count() - 2 );

	    // Ommit the first segment and insert points
	    newPath->first();
	    while( newPath->next() ) {
		pa->setPoint( nbrPoint++, coordX( newPath->current()->knot().x() ),
			coordY( newPath->current()->knot().y() ) );
	    }
	    delete newPath;
	} else if (segment->isLine()) {
	    pa->setPoint( nbrPoint++, coordX( itr.current()->knot().x() ),
		    coordY( itr.current()->knot().y() ) );
	} else if (segment->isBegin()) {
	    // start a new polygon
	    pa->setPoint( nbrPoint++, coordX( itr.current()->knot().x() ),
		    coordY( itr.current()->knot().y() ) );
        }
    }

    // adjust the number of points
    if ( nbrPoint > 1 ) {
        pa->resize( nbrPoint );
        mListPa.append( pa );
    }
    else {
        delete pa;
        // TODO: check why we have empty path
        kdDebug() << "WmfExport::visitVSubpath : Empty path ?" << endl;
    }
}


void WmfExport::visitVText( VText& text ) {
    // TODO: export text
    visitVSubpath( text.basePath() );
}


void WmfExport::getBrush( QBrush& brush, const VFill *fill ) {
    if( (fill->type() == VFill::solid) || (fill->type() == VFill::grad)
        || (fill->type() == VFill::patt) ) {
        if ( fill->color().opacity() < 0.1 ) {
            brush.setStyle( Qt::NoBrush );
        }
        else {
            brush.setStyle( Qt::SolidPattern );
            brush.setColor( fill->color() );
        }
    }
    else {
        brush.setStyle( Qt::NoBrush );
    }
}


void WmfExport::getPen( QPen& pen, const VStroke *stroke ) {
    if( (stroke->type() == VStroke::solid) || (stroke->type() == VStroke::grad)
        || (stroke->type() == VStroke::patt) ) {
        // TODO : Dash pattern.

        if ( stroke->lineCap() == VStroke::capRound ) {
            pen.setCapStyle( Qt::RoundCap );
        }
        else {
            pen.setCapStyle( Qt::SquareCap );
        }
        pen.setStyle( Qt::SolidLine );
        pen.setColor( stroke->color() );
        pen.setWidth( coordX( stroke->lineWidth() ) );
    }
    else {
        pen.setStyle( Qt::NoPen );
    }
}


#include <wmfexport.moc>

