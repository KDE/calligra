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

#ifndef kpbackground_h
#define kpbackground_h

#include <qcolor.h>
#include <qstring.h>
#include <qrect.h>
#include <qsize.h>

#include "global.h"
#include "kppixmapcollection.h"
#include "kpclipartcollection.h"

#include <koStream.h>
#include <qtextstream.h>

class KPresenterDoc;
class QPainter;
class QPixmap;
class KPGradientCollection;
class QPicture;
class DCOPObject;

/******************************************************************/
/* Class: KPBackGround                                            */
/******************************************************************/

class KPBackGround
{
public:
    KPBackGround( KPPixmapCollection *_pixmapCollection, KPGradientCollection *_gradientCollection,
                  KPClipartCollection *_clipartCollection, KPresenterDoc *_doc );
    virtual ~KPBackGround()
    {; }

    virtual DCOPObject* dcopObject();

    virtual void setBackType( BackType _backType )
    { backType = _backType; }
    virtual void setBackView( BackView _backView )
    { backView = _backView; }
    virtual void setBackColor1( QColor _color )
    { removeGradient(); backColor1 = _color; }
    virtual void setBackColor2( QColor _color )
    { removeGradient(); backColor2 = _color; }
    virtual void setBackColorType( BCType _bcType )
    { removeGradient(); bcType = _bcType; }
    virtual void setBackUnbalanced( bool _unbalanced )
    { removeGradient(); unbalanced = _unbalanced; }
    virtual void setBackXFactor( int _xfactor )
    { removeGradient(); xfactor = _xfactor; }
    virtual void setBackYFactor( int _yfactor )
    { removeGradient(); yfactor = _yfactor; }
    void setBackPixmap( const QString &_filename, QDateTime _lastModified );
    virtual void setBackClipFilename(  const QString &_filename, QDateTime _lastModified );
    virtual void setPageEffect( PageEffect _pageEffect )
    { pageEffect = _pageEffect; }
    virtual KPPixmapDataCollection::Key getKey()
    { return key.dataKey; }


    virtual void setSize( QSize _size )
    { removeGradient(); ext = _size; footerHeight = 0; }
    virtual void setSize( int _width, int _height )
    { removeGradient(); ext = QSize( _width, _height ); footerHeight = 0; }

    virtual BackType getBackType()
    { return backType; }
    virtual BackView getBackView()
    { return backView; }
    virtual QColor getBackColor1()
    { return backColor1; }
    virtual QColor getBackColor2()
    { return backColor2; }
    virtual BCType getBackColorType()
    { return bcType; }
    virtual QString getBackPixFilename()
    { return key.dataKey.filename; }
    virtual QString getBackClipFilename()
    { return clipKey.filename; }
    virtual PageEffect getPageEffect()
    { return pageEffect; }
    virtual bool getBackUnbalanced()
    { return unbalanced; }
    virtual int getBackXFactor()
    { return xfactor; }
    virtual int getBackYFactor()
    { return yfactor; }

    virtual QSize getSize()
    { return ext; }

    virtual void draw( QPainter *_painter, QPoint _offset, bool _drawBorders );

    virtual void restore();

    virtual void save( QTextStream& out );
    virtual void load( KOMLParser& parser, QValueList<KOMLAttrib>& lst );

protected:
    virtual void drawBackColor( QPainter *_painter );
    virtual void drawBackPix( QPainter *_painter );
    virtual void drawBackClip( QPainter *_painter );
    virtual void drawBorders( QPainter *_painter );
    virtual void drawHeaderFooter( QPainter *_painter, const QPoint &_offset );
    virtual void removeGradient();

    BackType backType;
    BackView backView;
    QColor backColor1;
    QColor backColor2;
    BCType bcType;
    PageEffect pageEffect;
    bool unbalanced;
    int xfactor, yfactor;

    QPixmap *backPix;
    KPPixmapCollection::Key key;
    KPPixmapCollection *pixmapCollection;
    KPGradientCollection *gradientCollection;
    KPClipartCollection *clipartCollection;
    QPixmap *gradient;
    QPicture *picture;
    KPClipartCollection::Key clipKey;

    QSize ext;
    KPresenterDoc *doc;
    int footerHeight;

    DCOPObject *dcop;

};

#endif
