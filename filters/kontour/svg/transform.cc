/*
    Copyright (C) 2001, Rob Buis <rwlbuis@wanadoo.nl>.
    This file is part of the KDE project

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

DESCRIPTION
    Filter to convert a svg file to a killustrator document using koDocument
*/

#include "transform.h"
#include <kdebug.h>

/********************* Transform stuff *******************************/

void SVGTransformable::performTransformations( GObject *obj ) {
    if( transform.isNull() || !obj ) return;

    SVGTransform *t;
    // Split string for handling 1 transform statement at a time
    QStringList subtransforms = QStringList::split( ';', transform );
    for( QStringList::Iterator it = subtransforms.begin(); it != subtransforms.end(); ++it ) {
        QStringList subtransform = QStringList::split( '(', (*it) );

        subtransform[0] = subtransform[0].stripWhiteSpace();
        subtransform[1] = subtransform[1].stripWhiteSpace();
        kdDebug() << "[0]: " << subtransform[0].local8Bit() << endl;
	kdDebug() << "[1]: " << subtransform[1].local8Bit() << endl;
        subtransform[1] = subtransform[1].simplifyWhiteSpace();
        QRegExp reg( "[a-zA-Z,() ]" );
        QStringList params = QStringList::split( reg, subtransform[1] );

        t = new SVGTransform;
        if( subtransform[0] == "rotate" ) {
            Rect boundingBox = obj->boundingBox();

            float middleX = boundingBox.left() + ((boundingBox.right() - boundingBox.left()) / 2.0);
            float middleY = boundingBox.top()  + ((boundingBox.bottom() - boundingBox.top()) / 2.0);

	    t->setRotate( params[0].toFloat(), middleX, middleY );
        }
	else if( subtransform[0] == "translate" ) {
            if(params.count() == 2)
	        t->setTranslate( params[0].toFloat(), params[1].toFloat() );
            else
	        t->setTranslate( params[0].toFloat() , 0 );
        }
	else if( subtransform[0] == "scale" ) {
            //Rect boundingBox = obj->boundingBox();

            //float middleX = boundingBox.left() + ((boundingBox.right() - boundingBox.left()) / 2.0);
            //float middleY = boundingBox.top()  + ((boundingBox.bottom() - boundingBox.top()) / 2.0);

	    if(params.count() == 2)
                t->setScale( params[0].toFloat(), params[1].toFloat() );
            else
	        t->setScale( params[0].toFloat(), 1 );	    
        }
	else if( subtransform[0] == "skewX" ) {
	    t->setSkewX( params[0].toFloat() );
        }
	else if( subtransform[0] == "skewY" ) {
            t->setSkewY( params[0].toFloat() );
        }
	else if(subtransform[0] == "matrix") {
	    if( params.count() >= 6 )
	    {
	        QWMatrix matrix;
                matrix.setMatrix( params[0].toFloat(), params[1].toFloat(),
                                  params[2].toFloat(), params[3].toFloat(),
                                  params[4].toFloat(), params[5].toFloat() );
                t->setMatrix( matrix );
            }
        }
	translateProps.appendItem( t );
            /*translateProps.append( new TranslateProperty(-middleX, -middleY) );

            if( params.count() == 2 )
                translateProps.append( new ScaleProperty(params[0].toFloat(), params[1].toFloat()) );
            else
                translateProps.append( new ScaleProperty(params[0].toFloat()) );
		
            translateProps.append( new TranslateProperty(middleX, middleY) );*/

        // perform transformations for this round
        for( unsigned int i = 0 ; i < translateProps.numberOfItems() ; i++ ) {
	    obj->transform( ((SVGTransform *) translateProps.getItem( i ))->matrix(), true );
        }
        translateProps.clear();
    }
}


void SVGTransform::setTranslate( float tx, float ty ) {
    _matrix = _matrix.translate( tx, ty );
}


void SVGTransform::setRotate( float angle, float cx, float cy ) {
    _matrix = _matrix.translate( cx, cy );
    _matrix = _matrix.rotate( angle );
    _matrix = _matrix.translate( -cx, -cy );
}


void SVGTransform::setScale( float sx, float sy ) {
    _matrix = _matrix.scale( sx, sy );
}


void SVGTransform::setSkewX( float angle ) {
    _matrix = _matrix.shear( angle, 0 );
}


void SVGTransform::setSkewY( float angle ) {
    _matrix = _matrix.shear( 0, angle );
}


void SVGTransform::setMatrix( const QWMatrix & m )
{
    _matrix = m;
}
