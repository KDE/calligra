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
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
*/

#include <config.h>
#include <qdom.h>
#include <qcstring.h>
#include <kdebug.h>
#include <kgenericfactory.h>
#include <koFilterChain.h>
#include <koStore.h>
#include <koStoreDevice.h>
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
TODO: bs.wmf stroke in red in Word97 and in brown kword ??
*/

typedef KGenericFactory<WmfExport, KoFilter> WmfExportFactory;
K_EXPORT_COMPONENT_FACTORY( libwmfexport, WmfExportFactory( "wmfexport" ) );


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


void WmfExport::visitVComposite( VComposite& composite ) {
    QPen      pen;
    QBrush    brush;
    
    getPen( pen, composite.stroke() );
    getBrush( brush, composite.fill() );
    
    VVisitor::visitVComposite( composite );
    
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
void WmfExport::visitVPath( VPath& path ) {
    VPath *newPath;
    VPathIterator itr( path );
    QPointArray *pa = new QPointArray( path.count() );            
    int  nbrPoint=0;      // number of points in the path
    
    for( ; itr.current(); ++itr ) {
        switch( itr.current()->type() ) {
            case VSegment::curve:
                // convert curveTo into lineTo
                newPath = new VPath( mDoc );
                
                // newPath duplicate the list of curve
                newPath->moveTo( itr.current()->prev()->knot() );
                newPath->append( itr.current()->clone() );
                while( itr.current()->next() ) {
                    if ( itr.current()->next()->type() == VSegment::curve ) {
                        newPath->append( itr.current()->next()->clone() );
                    }
                    else {
                        break;
                    }
                    ++itr;
                }
                
                // flatness proportionnal to the resolution (mDpi)
                // INCH_TO_POINT(1/mDpi) doesn't give enough precision
                flattenPath( *newPath, INCH_TO_POINT(0.3 / (double)mDpi) );

                // adjust the number of points                
                pa->resize( pa->size() + newPath->count() - 2 );
                
                // Ommit the first segment and insert points
                newPath->first();
                while( newPath->next() ) {
                    pa->setPoint( nbrPoint++, coordX( newPath->current()->knot().x() ), 
                                        coordY( newPath->current()->knot().y() ) );
                }
                delete newPath;
            break;
            case VSegment::line:
                pa->setPoint( nbrPoint++, coordX( itr.current()->knot().x() ), 
                                    coordY( itr.current()->knot().y() ) );
            break;
            case VSegment::begin:
                // start a new polygon
                pa->setPoint( nbrPoint++, coordX( itr.current()->knot().x() ), 
                                    coordY( itr.current()->knot().y() ) );
            break;
            default:
            break;
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
        kdDebug() << "WmfExport::visitVPath : Empty path ?" << endl;
    }
}


void WmfExport::visitVText( VText& text ) {
    // TODO: export text
    visitVPath( text.basePath() );
}


// this code is duplicated from flattenPathPlugin.cc
// TODO: remove WmfExport::flattenPath() if a similar code is recheable
void WmfExport::flattenPath( VPath& path, double flatness ) {
	VFlattenCmd cmd( 0L, INCH_TO_POINT(0.3 / (double)mDpi) );
	cmd.visit( path );
	return;
    // Ommit first segment.    
    /*path.first();
    while( path.next() ) {
            while( !path.current()->isFlat( flatness )  ) {
                // Split at midpoint.
                path.insert( path.current()->splitAt( 0.5 ) );
            }

        // Convert to line : join tangent with points
        if ( path.current()->type() == VSegment::curve ) {
            path.current()->setPoint( 0, path.current()->prev()->knot() );
            path.current()->setPoint( 1, path.current()->knot() );
        }
        // disabled path.current()->setDegree( 1 );
    }*/
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

