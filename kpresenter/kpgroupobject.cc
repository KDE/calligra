/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>

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

#include "kpgroupobject.h"

#include "kpresenter_doc.h"
#include "kplineobject.h"
#include "kprectobject.h"
#include "kpellipseobject.h"
#include "kpautoformobject.h"
#include "kpclipartobject.h"
#include "kptextobject.h"
#include "kppixmapobject.h"
#include "kppieobject.h"

#include <qpainter.h>
#include <iostream>
using namespace std;  

/******************************************************************/
/* Class: KPGroupObject						  */
/******************************************************************/

/*================================================================*/
KPGroupObject::KPGroupObject()
    : KPObject(), objects(), updateObjs( false )
{
    objects.setAutoDelete( false );
}

/*================================================================*/
KPGroupObject::KPGroupObject( const QList<KPObject> &objs )
    : KPObject(), objects( objs ), updateObjs( false )
{
    objects.setAutoDelete( false );
}

/*================================================================*/
KPGroupObject &KPGroupObject::operator=( const KPGroupObject & )
{
    return *this;
}

/*================================================================*/
void KPGroupObject::setSize( int _width, int _height )
{
    float fx = (float)_width / (float)ext.width();
    float fy = (float)_height / (float)ext.height();
    KPObject::setSize( _width, _height );
    updateSizes( fx, fy );
}

/*================================================================*/
void KPGroupObject::setOrig( QPoint _point )
{
    int dx = orig.x() - _point.x();
    int dy = orig.y() - _point.y();
    KPObject::setOrig( _point );
    updateCoords( dx, dy );
}

/*================================================================*/
void KPGroupObject::setOrig( int _x, int _y )
{
    int dx = orig.x() - _x;
    int dy = orig.y() - _y;
    KPObject::setOrig( _x, _y );
    updateCoords( dx, dy );
}

/*================================================================*/
void KPGroupObject::moveBy( QPoint _point )
{
    KPObject::moveBy( _point );
    updateCoords( _point.x(), _point.y() );
}

/*================================================================*/
void KPGroupObject::moveBy( int _dx, int _dy )
{
    KPObject::moveBy( _dx, _dy );
    updateCoords( _dx, _dy );
}

/*================================================================*/
void KPGroupObject::resizeBy( QSize _size )
{
    float fx = (float)( (float)ext.width() + (float)_size.width() ) / (float)ext.width();
    float fy = (float)( (float)ext.height() + (float)_size.height() ) / (float)ext.height();
    KPObject::resizeBy( _size );
    updateSizes( fx, fy );
}

/*================================================================*/
void KPGroupObject::resizeBy( int _dx, int _dy )
{
    float fx = (float)( (float)ext.width() + (float)_dx ) / (float)ext.width();
    float fy = (float)( (float)ext.height() + (float)_dy ) / (float)ext.height();
    KPObject::resizeBy( _dx, _dy );
    updateSizes( fx, fy );
}

/*================================================================*/
void KPGroupObject::save( QTextStream& out )
{
    out << indent << "<ORIG x=\"" << orig.x() << "\" y=\"" << orig.y() << "\"/>" << endl;
    out << indent << "<SIZE width=\"" << ext.width() << "\" height=\"" << ext.height() << "\"/>" << endl;
    out << indent << "<OBJECTS>" << endl;

    KPObject *kpobject = 0;
    for ( unsigned int i = 0; i < objects.count() ; ++i ) {
	kpobject = objects.at( i );
	if ( kpobject->getType() == OT_PART )
	    continue;
	out << otag << "<OBJECT type=\"" << static_cast<int>( kpobject->getType() ) << "\">" << endl;
	kpobject->save( out );
	out << etag << "</OBJECT>" << endl;
    }

    out << indent << "</OBJECTS>" << endl;
}

/*================================================================*/
void KPGroupObject::load( KOMLParser& parser, vector<KOMLAttrib>& lst,
			  KPresenterDoc *doc )
{
    string tag;
    string name;

    updateObjs = false;

    while ( parser.open( 0L, tag ) ) {
	parser.parseTag( tag.c_str(), name, lst );

	// orig
	if ( name == "ORIG" ) {
	    parser.parseTag( tag.c_str(), name, lst );
	    vector<KOMLAttrib>::const_iterator it = lst.begin();
	    for( ; it != lst.end(); it++ ) {
		if ( ( *it ).m_strName == "x" )
		    orig.setX( atoi( ( *it ).m_strValue.c_str() ) );
		if ( ( *it ).m_strName == "y" )
		    orig.setY( atoi( ( *it ).m_strValue.c_str() ) );
	    }
	}

	// size
	else if ( name == "SIZE" ) {
	    parser.parseTag( tag.c_str(), name, lst );
	    vector<KOMLAttrib>::const_iterator it = lst.begin();
	    for( ; it != lst.end(); it++ ) {
		if ( ( *it ).m_strName == "width" )
		    ext.setWidth( atoi( ( *it ).m_strValue.c_str() ) );
		if ( ( *it ).m_strName == "height" )
		    ext.setHeight( atoi( ( *it ).m_strValue.c_str() ) );
	    }
	} else if ( name == "OBJECTS" ) {
	    parser.parseTag( tag.c_str(), name, lst );
	    vector<KOMLAttrib>::const_iterator it = lst.begin();
	    for( ; it != lst.end(); it++ ) {
	    }

	    ObjType t = OT_LINE;

	    while ( parser.open( 0L, tag ) ) {
		parser.parseTag( tag.c_str(), name, lst );

		// object
		if ( name == "OBJECT" ) {
		    parser.parseTag( tag.c_str(), name, lst );
		    vector<KOMLAttrib>::const_iterator it = lst.begin();
		    for( ; it != lst.end(); it++ ) {
			if ( ( *it ).m_strName == "type" )
			    t = ( ObjType )atoi( ( *it ).m_strValue.c_str() );
		    }

		    switch ( t ) {
		    case OT_LINE: {
			KPLineObject *kplineobject = new KPLineObject();
			kplineobject->load( parser, lst );

			objects.append( kplineobject );
		    } break;
		    case OT_RECT: {
			KPRectObject *kprectobject = new KPRectObject();
			kprectobject->setRnds( doc->getRndX(), doc->getRndY() );
			kprectobject->load( parser, lst );

			objects.append( kprectobject );
		    } break;
		    case OT_ELLIPSE: {
			KPEllipseObject *kpellipseobject = new KPEllipseObject();
			kpellipseobject->load( parser, lst );

			objects.append( kpellipseobject );
		    } break;
		    case OT_PIE: {
			KPPieObject *kppieobject = new KPPieObject();
			kppieobject->load( parser, lst );

			objects.append( kppieobject );
		    } break;
		    case OT_AUTOFORM: {
			KPAutoformObject *kpautoformobject = new KPAutoformObject();
			kpautoformobject->load( parser, lst );

			objects.append( kpautoformobject );
		    } break;
		    case OT_CLIPART: {
			KPClipartObject *kpclipartobject = new KPClipartObject( doc->getClipartCollection() );
			kpclipartobject->load( parser, lst );

			objects.append( kpclipartobject );
		    } break;
		    case OT_TEXT: {
			KPTextObject *kptextobject = new KPTextObject( doc );
			kptextobject->load( parser, lst );

			objects.append( kptextobject );
		    } break;
		    case OT_PICTURE: {
			KPPixmapObject *kppixmapobject = new KPPixmapObject( doc->getPixmapCollection() );
			kppixmapobject->load( parser, lst );

			objects.append( kppixmapobject );
		    } break;
		    case OT_GROUP: {
			KPGroupObject *kpgroupobject = new KPGroupObject();
			kpgroupobject->load( parser, lst, doc );

			objects.append( kpgroupobject );
		    } break;
		    default: break;
		    }

		} else
		    cerr << "Unknown tag '" << tag << "' in OBJECTS" << endl;

		if ( !parser.close( tag ) ) {
		    cerr << "ERR: Closing Child" << endl;
		    return;
		}
	    }
	
	
	} else
	    cerr << "Unknown tag '" << tag << "' in GROUP_OBJECT" << endl;

	if ( !parser.close( tag ) ) {
	    cerr << "ERR: Closing Child" << endl;
	    return;
	}
    }

    updateObjs = true;
}

/*================================================================*/
void KPGroupObject::draw( QPainter *_painter, int _diffx, int _diffy )
{
    if ( move ) {
	KPObject::draw( _painter, _diffx, _diffy );
	return;
    }

    KPObject *kpobject = 0;
    for ( unsigned int i = 0; i < objects.count(); i++ ) {
	kpobject = objects.at( i );
	kpobject->draw( _painter, _diffy, _diffy );
    }

    KPObject::draw( _painter, _diffx, _diffy );
}

/*================================================================*/
void KPGroupObject::updateSizes( float fx, float fy )
{
    if ( !updateObjs )
	return;

    KPObject *kpobject = 0;
    for ( unsigned int i = 0; i < objects.count(); i++ ) {
	kpobject = objects.at( i );
	int w = (int)( (float)kpobject->getSize().width() * fx );
	int h = (int)( (float)kpobject->getSize().height() * fy );
	kpobject->setSize( w, h );
    }
}

/*================================================================*/
void KPGroupObject::updateCoords( int dx, int dy )
{
    if ( !updateObjs )
	return;

    for ( unsigned int i = 0; i < objects.count(); i++ )
	objects.at( i )->moveBy( dx, dy );
}

/*================================================================*/
void KPGroupObject::rotate( float _angle )
{
    KPObject::rotate( _angle );

    if ( !updateObjs )
	return;

    for ( unsigned int i = 0; i < objects.count(); i++ )
	objects.at( i )->rotate( _angle );
}

/*================================================================*/
void KPGroupObject::setShadowDistance( int _distance )
{
    KPObject::setShadowDistance( _distance );

    if ( !updateObjs )
	return;

    for ( unsigned int i = 0; i < objects.count(); i++ )
	objects.at( i )->setShadowDistance( _distance );
}

/*================================================================*/
void KPGroupObject::setShadowDirection( ShadowDirection _direction )
{
    KPObject::setShadowDirection( _direction );

    if ( !updateObjs )
	return;

    for ( unsigned int i = 0; i < objects.count(); i++ )
	objects.at( i )->setShadowDirection( _direction );
}

/*================================================================*/
void KPGroupObject::setShadowColor( QColor _color )
{
    KPObject::setShadowColor( _color );

    if ( !updateObjs )
	return;

    for ( unsigned int i = 0; i < objects.count(); i++ )
	objects.at( i )->setShadowColor( _color );
}

/*================================================================*/
void KPGroupObject::setEffect( Effect _effect )
{
    KPObject::setEffect( _effect );

    if ( !updateObjs )
	return;

    for ( unsigned int i = 0; i < objects.count(); i++ )
	objects.at( i )->setEffect( _effect );
}

/*================================================================*/
void KPGroupObject::setEffect2( Effect2 _effect2 )
{
    KPObject::setEffect2( _effect2 );

    if ( !updateObjs )
	return;

    for ( unsigned int i = 0; i < objects.count(); i++ )
	objects.at( i )->setEffect2( _effect2 );
}

/*================================================================*/
void KPGroupObject::setPresNum( int _presNum )
{
    KPObject::setPresNum( _presNum );

    if ( !updateObjs )
	return;

    for ( unsigned int i = 0; i < objects.count(); i++ )
	objects.at( i )->setPresNum( _presNum );
}

/*================================================================*/
void KPGroupObject::setDisappear( bool b )
{
    KPObject::setDisappear( b );

    if ( !updateObjs )
	return;

    for ( unsigned int i = 0; i < objects.count(); i++ )
	objects.at( i )->setDisappear( b );
}

/*================================================================*/
void KPGroupObject::setDisappearNum( int num )
{
    KPObject::setDisappearNum( num );

    if ( !updateObjs )
	return;

    for ( unsigned int i = 0; i < objects.count(); i++ )
	objects.at( i )->setDisappearNum( num );
}

/*================================================================*/
void KPGroupObject::setEffect3( Effect3 _effect3)
{
    KPObject::setEffect3( _effect3 );

    if ( !updateObjs )
	return;

    for ( unsigned int i = 0; i < objects.count(); i++ )
	objects.at( i )->setEffect3( _effect3 );
}

/*================================================================*/
void KPGroupObject::zoom( float _fakt )
{
    KPObject::zoom( _fakt );

    if ( !updateObjs )
	return;

    for ( unsigned int i = 0; i < objects.count(); i++ )
	objects.at( i )->zoom( _fakt );
}

/*================================================================*/
void KPGroupObject::zoomOrig()
{
    KPObject::zoomOrig();

    if ( !updateObjs )
	return;

    for ( unsigned int i = 0; i < objects.count(); i++ )
	objects.at( i )->zoomOrig();
}

/*================================================================*/
void KPGroupObject::setOwnClipping( bool _ownClipping )
{
    KPObject::setOwnClipping( _ownClipping );

    if ( !updateObjs )
	return;

    for ( unsigned int i = 0; i < objects.count(); i++ )
	objects.at( i )->setOwnClipping( _ownClipping );
}

/*================================================================*/
void KPGroupObject::setSubPresStep( int _subPresStep )
{
    KPObject::setSubPresStep( _subPresStep );

    if ( !updateObjs )
	return;

    for ( unsigned int i = 0; i < objects.count(); i++ )
	objects.at( i )->setSubPresStep( _subPresStep );
}

/*================================================================*/
void KPGroupObject::doSpecificEffects( bool _specEffects, bool _onlyCurrStep )
{
    KPObject::doSpecificEffects( _specEffects, _onlyCurrStep );

    if ( !updateObjs )
	return;

    for ( unsigned int i = 0; i < objects.count(); i++ )
	objects.at( i )->doSpecificEffects( _specEffects, _onlyCurrStep );
}
