// -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>
   Copyright (C) 2004,2006 Thorsten Zachmann <zachmann@kde.org>

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

#ifndef kpbackground_h
#define kpbackground_h

#include <qsize.h>
#include <qdatetime.h>
#include <KoBrush.h>
#include <KoStyleStack.h>
#include <KoOasisStyles.h>
#include <KoOasisContext.h>

#include <KoPictureCollection.h>

#include "global.h"

class KPrDocument;
class QPainter;
class QPixmap;
class KPrGradientCollection;
class QDomDocument;
class QDomElement;
class KoTextZoomHandler;
class KPrPage;
class KoXmlWriter;
class KoGenStyles;
class KoGenStyle;


class KPrBackGround
{
public:
    struct Settings
    {
        Settings( const BackType backType, const QColor backColor1, const QColor backColor2,
                  const BCType bcType, const bool unbalanced, const int xfactor, const int yfactor,
                  const KoPictureKey &key, const BackView backView )
        : m_backType( backType )
        , m_backColor1( backColor1 )
        , m_backColor2( backColor2 )
        , m_bcType( bcType )
        , m_unbalanced( unbalanced )
        , m_xfactor( xfactor )
        , m_yfactor( yfactor )
        , m_backPictureKey( key )
        , m_backView( backView )
        {}

        BackType m_backType;
        QColor m_backColor1;
        QColor m_backColor2;
        // Gradient setting
        BCType m_bcType;
        bool m_unbalanced;
        // Gradient factors
        int m_xfactor, m_yfactor;

        KoPictureKey m_backPictureKey;
        BackView m_backView;
    };

    KPrBackGround( KPrPage *_page );
    ~KPrBackGround() {}

    void setBackType( BackType _backType )
        {
            backType = _backType;
            if (backType==BT_PICTURE || backType==BT_CLIPART)
                removeGradient();
        }
    void setBackView( BackView _backView )
        { backView = _backView; }
    void setBackColor1( const QColor &_color )
        { removeGradient(); backColor1 = _color; }
    void setBackColor2( const QColor &_color )
        { removeGradient(); backColor2 = _color; }
    void setBackColorType( BCType _bcType )
        { removeGradient(); bcType = _bcType; }
    void setBackUnbalanced( bool _unbalanced )
        { removeGradient(); unbalanced = _unbalanced; }
    void setBackXFactor( int _xfactor )
        { removeGradient(); xfactor = _xfactor; }
    void setBackYFactor( int _yfactor )
        { removeGradient(); yfactor = _yfactor; }
    /// set the back picture to a new KoPicture
    void setBackPicture( const KoPicture& picture );
    void setBackPicture ( const KoPictureKey& key );
    void setBackGround( const struct Settings &settings );
    struct Settings getBackGround() const;

    KoPicture picture() const { return backPicture;}

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
    KoPictureKey getBackPictureKey() const
        { return backPicture.getKey(); }
    KoPicture getBackPicture() const
        { return backPicture;}

    bool getBackUnbalanced() const
        { return unbalanced; }
    int getBackXFactor() const
        { return xfactor; }
    int getBackYFactor() const
        { return yfactor; }

    // Draw the background.
    // Uses the @p zoomHandler to determine the size of the background
    void drawBackground( QPainter *_painter, const KoTextZoomHandler* zoomHandler,
                         const QRect& crect, bool _drawBorders );
    // Draw the background. The size of the background is passed explicitly
    void drawBackground( QPainter *_painter, const QSize& ext,
                         const QRect& crect, bool _drawBorders );

    void reload();

    QDomElement save( QDomDocument &doc );
    void load( const QDomElement &element );
    void loadOasis( KoOasisContext & context );
    void saveOasisBackgroundPageStyle( KoGenStyle &stylepageauto, KoGenStyles &mainStyles );

protected:
    void drawBackColor( QPainter *_painter, const QSize& ext, const QRect& crect );
    void drawBackPix( QPainter *_painter, const QSize& ext, const QRect& crect );
    void drawBorders( QPainter *_painter, const QSize& ext, const QRect& crect );
    // The current gradient isn't valid anymore (e.g. size or settings changed) -> discard it
    void removeGradient();
    // Generate a new gradient pixmap, for the given size
    void generateGradient( const QSize& size );

    KoPictureCollection *pictureCollection() const;
    KPrGradientCollection *gradientCollection() const;

    QString saveOasisPictureStyle( KoGenStyles& mainStyles );

private:
    BackType backType;
    BackView backView;
    QColor backColor1;
    QColor backColor2;
    BCType bcType;

    // Background picture
    KoPicture backPicture;

    // Pixmap used to cache the drawing of the gradient, at the current size
    const QPixmap *gradientPixmap;

    // The page for which this background exists
    KPrPage *m_page;

    //int footerHeight;

    // Gradient factors
    int xfactor, yfactor;

    // Gradient setting
    bool unbalanced;
};

#endif
