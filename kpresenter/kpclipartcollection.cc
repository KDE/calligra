/******************************************************************/
/* KPresenter - (c) by Reginald Stadlbauer 1998			  */
/* Version: 0.1.0						  */
/* Author: Reginald Stadlbauer					  */
/* E-Mail: reggie@kde.org					  */
/* needs c++ library Qt (http://www.troll.no)			  */
/* needs mico (http://diamant.vsb.cs.uni-frankfurt.de/~mico/)	  */
/* needs OpenParts and Kom (weis@kde.org)			  */
/* written for KDE (http://www.kde.org)				  */
/* KPresenter is under GNU GPL					  */
/******************************************************************/
/* Module: clipart collection					  */
/******************************************************************/

#include "kpclipartcollection.h"
#include "qwmf.h"

#include <qstring.h>

/******************************************************************/
/* Class: KPClipartCollection					  */
/******************************************************************/

/*================================================================*/
KPClipartCollection::~KPClipartCollection()
{
    data.clear();
    refs.clear();
}

/*================================================================*/
QPicture *KPClipartCollection::findClipart( const Key &key )
{
    QMap< Key, QPicture >::Iterator it = data.find ( key );

    if ( it != data.end() && it.key() == key )
	return &it.data();
    else {
	QWinMetaFile wmf;
	wmf.load( key.filename );
	QPicture pic;
	wmf.paint( &pic );
	return insertClipart( key, pic );
    }
}

/*================================================================*/
QPicture *KPClipartCollection::insertClipart( const Key &key, const QPicture &pic )
{
    QPicture *picture = new QPicture;
    picture->setData( pic.data(), pic.size() );

    data.insert( Key( key ), *picture );

    int ref = 1;
    refs.insert( Key( key ), ref );

    return picture;
}

/*================================================================*/
void KPClipartCollection::addRef( const Key &key )
{
    if ( !allowChangeRef )
	return;

//     qDebug( "KPClipartCollection::addRef" );
    
    if ( refs.contains( key ) ) {
	int ref = refs[ key ];
	refs[ key ] = ++ref;
// 	qDebug( "    ref: %d", ref );
    }
}

/*================================================================*/
void KPClipartCollection::removeRef( const Key &key )
{
    if ( !allowChangeRef )
	return;

//     qDebug( "KPClipartCollection::removeRef" );
    
    if ( refs.contains( key ) ) {
	int ref = refs[ key ];
	refs[ key ] = --ref;
// 	qDebug( "     ref: %d", ref );
	if ( ref == 0 ) {
// 	    qDebug( "        remove" );
	    refs.remove( key );
	    data.remove( key );
	}
    }
    
}

/*================================================================*/
ostream& operator<<( ostream &out, KPClipartCollection::Key &key )
{
    QDate date = key.lastModified.date();
    QTime time = key.lastModified.time();

    out << " filename=\"" << key.filename.latin1() << "\" year=\"" << date.year()
	<< "\" month=\"" << date.month() << "\" day=\"" << date.day()
	<< "\" hour=\"" << time.hour() << "\" minute=\"" << time.minute()
	<< "\" second=\"" << time.second() << "\" msec=\"" << time.msec() << "\" ";

    return out;
}
