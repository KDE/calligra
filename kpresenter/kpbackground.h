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
#include <qdatetime.h>

#include <koPictureCollection.h>

#include "global.h"

class KPresenterDoc;
class QPainter;
class QPixmap;
class KPGradientCollection;
class QDomDocument;
class QDomElement;
class KoZoomHandler;
class KPrPage;

/******************************************************************/
/* Class: KPBackGround                                            */
/* The background for a given page.                               */
/******************************************************************/

class KPBackGround
{
public:
    KPBackGround( KPrPage *_page );
    ~KPBackGround() {}

    void setBackType( BackType _backType )
    { backType = _backType; }
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
    void setBackPixmap( const QString &_filename, QDateTime _lastModified );
    /// set the back pixmap to a new KoPicture
    void setBackPixmap( const KoPicture& pixmap );
    void setBackClipart(  const QString &_filename, QDateTime _lastModified );
    /// set the back clipart to a new KoPicture
    void setBackClipart( const KoPicture& clipart );
    void setPageEffect( PageEffect _pageEffect )
    { pageEffect = _pageEffect; }
    void setPageTimer( int _pageTimer )
    { pageTimer = _pageTimer; }
    void setPageSoundEffect( bool _soundEffect )
    { soundEffect = _soundEffect; }
    void setPageSoundFileName( const QString &_soundFileName )
    { soundFileName = _soundFileName; }

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
    KoPictureKey getBackPixKey() const
    { return backImage.getKey(); }
    KoPictureKey getBackClipKey() const
    { return backClipart.getKey(); }

    PageEffect getPageEffect() const
    { return pageEffect; }
    bool getBackUnbalanced() const
    { return unbalanced; }
    int getBackXFactor() const
    { return xfactor; }
    int getBackYFactor() const
    { return yfactor; }
    int getPageTimer() const
    { return pageTimer; }
    bool getPageSoundEffect() const
    { return soundEffect; }
    QString getPageSoundFileName() const
    { return soundFileName; }

    // Draw the background.
    // Uses the @p zoomHandler to determine the size of the background
    void draw( QPainter *_painter, const KoZoomHandler* zoomHandler,
               const QRect& crect, bool _drawBorders );
    // Draw the background. The size of the background is passed explicitely
    void draw( QPainter *_painter, const QSize& ext,
               const QRect& crect, bool _drawBorders );

    void reload();

    QDomElement save( QDomDocument &doc );
    void load( const QDomElement &element );

protected:
    void drawBackColor( QPainter *_painter, const QSize& ext, const QRect& crect );
    void drawBackPix( QPainter *_painter, const QSize& ext, const QRect& crect );
    void drawBorders( QPainter *_painter, const QSize& ext, const QRect& crect );
    // The current gradient isn't valid anymore (e.g. size or settings changed) -> discard it
    void removeGradient();
    // Generate a new gradient pixmap, for the given size
    void generateGradient( const QSize& size );

    KoPictureCollection *imageCollection() const;
    KPGradientCollection *gradientCollection() const;
    KoPictureCollection *clipartCollection() const;

private:
    BackType backType;
    BackView backView;
    QColor backColor1;
    QColor backColor2;
    BCType bcType;
    PageEffect pageEffect;
    // Sound played when showing this page
    QString soundFileName;

    // Background image or clipart
    KoPicture backImage;
    KoPicture backClipart;
    // Pixmap used to cache the drawing of the gradient, at the current size
    const QPixmap *gradientPixmap;

    // The page for which this background exists
    KPrPage *m_page;

    //int footerHeight;

    // Gradient factors
    int xfactor, yfactor;
    // ### Not related to the background, but to the page: delay for the page
    int pageTimer;
    // ### This could be simply !soundFileName.isEmpty()...
    bool soundEffect;
    // Gradient setting
    bool unbalanced;
};

#endif
