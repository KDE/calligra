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

#include <qsize.h>
#include <komlParser.h>
#include <global.h>
#include <kpimage.h>
#include <kpclipartcollection.h>

class KPresenterDoc;
class QPainter;
class QPixmap;
class KPGradientCollection;
class QPicture;
class DCOPObject;
class QDomDocument;
class QDomElement;

/******************************************************************/
/* Class: KPBackGround                                            */
/* The background for a given page.                               */
/* Stored in KPresenterDoc's list of backgrounds.                 */
/******************************************************************/

class KPBackGround
{
public:
    KPBackGround( KPImageCollection *_imageCollection, KPGradientCollection *_gradientCollection,
                  KPClipartCollection *_clipartCollection, KPresenterDoc *_doc );
    ~KPBackGround()
    {; }

    DCOPObject* dcopObject();

    void setBackType( BackType _backType )
    { backType = _backType; }
    void setBackView( BackView _backView )
    { backView = _backView; }
    void setBackColor1( QColor _color )
    { removeGradient(); backColor1 = _color; }
    void setBackColor2( QColor _color )
    { removeGradient(); backColor2 = _color; }
    void setBackColorType( BCType _bcType )
    { removeGradient(); bcType = _bcType; }
    void setBackUnbalanced( bool _unbalanced )
    { removeGradient(); unbalanced = _unbalanced; }
    void setBackXFactor( int _xfactor )
    { removeGradient(); xfactor = _xfactor; }
    void setBackYFactor( int _yfactor )
    { removeGradient(); yfactor = _yfactor; }
    void setBackPixmap( const QString &_filename, QDateTime _lastModified );
    void setBackClipFilename(  const QString &_filename, QDateTime _lastModified );
    void setPageEffect( PageEffect _pageEffect )
    { pageEffect = _pageEffect; }
    KPImageKey getKey() const
    { return backImage.key(); }


    void setBgSize( QSize _size, bool visible = true );

    //void setBgSize( int width, int height )
    //{ setBgSize( QSize( width, height ) ); }

    BackType getBackType() const
    { return backType; }
    BackView getBackView() const
    { return backView; }
    QColor getBackColor1() const
    { return backColor1; }
    QColor getBackColor2() const
    { return backColor2; }
    BCType getBackColorType() const
    { return bcType; }
    QString getBackPixFilename() const
    { return backImage.key().filename; }
    QString getBackClipFilename() const
    { return clipKey.filename; }
    PageEffect getPageEffect() const
    { return pageEffect; }
    bool getBackUnbalanced() const
    { return unbalanced; }
    int getBackXFactor() const
    { return xfactor; }
    int getBackYFactor() const
    { return yfactor; }

    QSize getSize() const
    { return ext; }

    void draw( QPainter *_painter, QPoint _offset, bool _drawBorders );

    void restore();

    QDomElement save( QDomDocument &doc );
    void load( KOMLParser& parser, QValueList<KOMLAttrib>& lst );

protected:
    void drawBackColor( QPainter *_painter );
    void drawBackPix( QPainter *_painter );
    void drawBackClip( QPainter *_painter );
    void drawBorders( QPainter *_painter );
    void drawHeaderFooter( QPainter *_painter, const QPoint &_offset );
    void removeGradient();

    BackType backType;
    BackView backView;
    QColor backColor1;
    QColor backColor2;
    BCType bcType;
    PageEffect pageEffect;
    bool unbalanced;
    int xfactor, yfactor;

    KPImage backImage;
    KPImageCollection *imageCollection;
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
