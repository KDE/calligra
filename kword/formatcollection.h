/******************************************************************/
/* KWord - (c) by Reginald Stadlbauer and Torben Weis 1997-1998	  */
/* Version: 0.0.1						  */
/* Author: Reginald Stadlbauer, Torben Weis			  */
/* E-Mail: reggie@kde.org, weis@kde.org				  */
/* Homepage: http://boch35.kfunigraz.ac.at/~rs			  */
/* needs c++ library Qt (http://www.troll.no)			  */
/* written for KDE (http://www.kde.org)				  */
/* needs mico (http://diamant.vsb.cs.uni-frankfurt.de/~mico/)	  */
/* needs OpenParts and Kom (weis@kde.org)			  */
/* License: GNU GPL						  */
/******************************************************************/
/* Module: Format Collection (header)				  */
/******************************************************************/

#ifndef formatcollection_h
#define formatcollection_h

#include "format.h"

#include <qdict.h>
#include <qstring.h>
#include <qdom.h>
#include <qmap.h>

class KWordDocument;

/******************************************************************/
/* Class: KWFormatCollection					  */
/******************************************************************/

class KWFormatCollection
{
public:
    KWFormatCollection( KWordDocument *_doc );
    ~KWFormatCollection();

    /**
      * Uses the @ref reverseIndexMap to map an id to
      * a loaded KWFormat. This is only useful during loading.
      * The function increases the reference count of the returned
      * format.
      */
    KWFormat *getFormat( int id );
    /**
      * The function increases the reference count of the returned
      * format.
      */
    KWFormat *getFormat( const KWFormat &_format );
    void removeFormat( KWFormat *_format );

    QString generateKey( KWFormat *_format )
    { return generateKey( *_format ); }

    /**
      * Maps a format to an id. That is useful during saving only.
      */
    int getId( const KWFormat &_format );
    
    /**
      * This function is called from KWordDocument after loading/saving.
      */
    void clearIndexMaps();
    
    QDomElement save( QDomDocument &doc );
    bool load( const QDomElement& );
    
protected:
    QString generateKey( const KWFormat &_format );
    KWFormat *findFormat( QString _key );
    KWFormat *insertFormat( QString _key, const KWFormat &_format );

    QDict<KWFormat> formats;
    QMap< KWFormat*, int > indexMap;
    QMap< int, KWFormat* > reverseIndexMap;
    KWordDocument *doc;

};

#endif
