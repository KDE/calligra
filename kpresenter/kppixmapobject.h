/******************************************************************/
/* KPresenter - (c) by Reginald Stadlbauer 1997-1998              */
/* Version: 0.1.0                                                 */
/* Author: Reginald Stadlbauer                                    */
/* E-Mail: reggie@kde.org                                         */
/* Homepage: http://boch35.kfunigraz.ac.at/~rs                    */
/* needs c++ library Qt (http://www.troll.no)                     */
/* written for KDE (http://www.kde.org)                           */
/* needs mico (http://diamant.vsb.cs.uni-frankfurt.de/~mico/)     */
/* needs OpenParts and Kom (weis@kde.org)                         */
/* License: GNU GPL                                               */
/******************************************************************/
/* Module: pixmap object (header)                                 */
/******************************************************************/

#ifndef kppixmapobject_h
#define kppixmapobject_h

#include <qdatetime.h>

#include "kpobject.h"
#include "kppixmapcollection.h"
#include "kpgradient.h"

class QPixmap;

/******************************************************************/
/* Class: KPPixmapObject                                          */
/******************************************************************/

class KPPixmapObject : public KPObject
{
public:
    KPPixmapObject( KPPixmapCollection *_pixmapCollection );
    KPPixmapObject( KPPixmapCollection *_pixmapCollection, const QString &_filename, QDateTime _lastModified );
    virtual ~KPPixmapObject()
    {}

    KPPixmapObject &operator=( const KPPixmapObject & );
    
    virtual void setSize( int _width, int _height );
    virtual void setSize( QSize _size )
    { setSize( _size.width(), _size.height() ); }
    virtual void resizeBy( int _dx, int _dy );

    virtual void setPen( QPen _pen )
    { pen = _pen; }
    virtual void setBrush( QBrush _brush )
    { brush = _brush; }
    virtual void setFillType( FillType _fillType );
    virtual void setGColor1( QColor _gColor1 )
    { if ( gradient ) gradient->setColor1( _gColor1 ); gColor1 = _gColor1; }
    virtual void setGColor2( QColor _gColor2 )
    { if ( gradient ) gradient->setColor2( _gColor2 ); gColor2 = _gColor2; }
    virtual void setGType( BCType _gType )
    { if ( gradient ) gradient->setBackColorType( _gType ); gType = _gType; }
    virtual QString getFileName()
    { return key.dataKey.filename; }

    void setPixmap( const QString &_filename, QDateTime _lastModified )
    { setPixmap( _filename, _lastModified, orig_size ); }
    void setPixmap( const QString &_filename, QDateTime _lastModified, const QSize &_size );
    void reload()
    { setPixmap( key.dataKey.filename, key.dataKey.lastModified, key.size ); }

    virtual ObjType getType()
    { return OT_PICTURE; }
    virtual QPen getPen()
    { return pen; }
    virtual QBrush getBrush()
    { return brush; }
    virtual FillType getFillType()
    { return fillType; }
    virtual QColor getGColor1()
    { return gColor1; }
    virtual QColor getGColor2()
    { return gColor2; }
    virtual BCType getGType()
    { return gType; }

    virtual void save( ostream& out );
    virtual void load( KOMLParser& parser, vector<KOMLAttrib>& lst );

    virtual void draw( QPainter *_painter, int _diffx, int _diffy );

protected:
    KPPixmapObject()
    {; }

    KPPixmapCollection *pixmapCollection;
    QPixmap *pixmap;
    KPPixmapCollection::Key key;
    KPGradient *gradient;

    QPen pen;
    QBrush brush;
    QColor gColor1, gColor2;
    BCType gType;
    FillType fillType;

};

#endif
