// -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-

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

// ### TODO: fix copyright date/authors

#ifndef kppixmapobject_h
#define kppixmapobject_h

#include <qdatetime.h>
#include <qvariant.h>

#include "kpobject.h"
#include <koPictureCollection.h>
#include <koSize.h>

class QPixmap;

// ### TODO rename the class (and the related files/classes) to a better name (for example KPPictureObject)

class KPPixmapObject : public KP2DObject
{

public:
    KPPixmapObject( KoPictureCollection *_imageCollection );
    KPPixmapObject( KoPictureCollection *_imageCollection, const KoPictureKey & key );
    virtual ~KPPixmapObject() {}

    KPPixmapObject &operator=( const KPPixmapObject & );

    virtual DCOPObject* dcopObject();

    /**
     * Only used as a default value in the filedialog, in changePicture
     * \warning Do not use for anything else
     */
    QString getFileName() const
        { return image.getKey().filename(); }

    KoPictureKey getKey() const
        { return image.getKey(); }

    QSize originalSize() const
        { return image.getOriginalSize(); }

    /// Deprecated, please use KPPixmapObject::setPicture
    void setPixmap( const KoPictureKey & key ) KDE_DEPRECATED;
    void setPicture( const KoPictureKey & key );

    void reload( void );

    virtual ObjType getType() const
        { return OT_PICTURE; }
    virtual QString getTypeString() const
        { return i18n("Picture"); }

    virtual QDomDocumentFragment save( QDomDocument& doc, double offset );
    virtual bool saveOasis( KoXmlWriter &xmlWriter, KoSavingContext& context, int indexObj );

    virtual double load(const QDomElement &element);
    virtual void loadOasis(const QDomElement &element, KoOasisContext & context, KPRLoadingInfo *info);

    virtual void draw( QPainter *_painter, KoZoomHandler*_zoomHandler,
                       SelectionMode selectionMode, bool drawContour = FALSE );

    QPixmap getOriginalPixmap();
    PictureMirrorType getPictureMirrorType() const { return mirrorType; }
    int getPictureDepth() const { return depth; }
    bool getPictureSwapRGB() const { return swapRGB; }
    bool getPictureGrayscal() const { return grayscal; }
    int getPictureBright() const { return bright; }

    ImageEffect getImageEffect() const {return m_effect;}
    QVariant getIEParam1() const {return m_ie_par1;}
    QVariant getIEParam2() const {return m_ie_par2;}
    QVariant getIEParam3() const {return m_ie_par3;}
    void setImageEffect(ImageEffect eff) { m_effect = eff; }
    void setIEParams(QVariant p1, QVariant p2, QVariant p3) {
        m_ie_par1=p1;
        m_ie_par2=p2;
        m_ie_par3=p3;
    }

    void setPictureMirrorType(const PictureMirrorType &_mirrorType) { mirrorType = _mirrorType; }
    void setPictureDepth(int _depth) { depth = _depth; }
    void setPictureSwapRGB(bool _swapRGB) { swapRGB = _swapRGB; }
    void setPictureGrayscal(bool _grayscal) { grayscal = _grayscal; }
    void setPictureBright(int _bright) { bright = _bright; }

    KoPicture picture() const { return image;}

    /// Deprecated, please use KPPixmapObject::loadPicture
    void loadImage( const QString & fileName ) KDE_DEPRECATED;
    void loadPicture( const QString & fileName );

    virtual void flip(bool horizontal );

protected:
    KPPixmapObject() {}

    QPixmap changePictureSettings( QPixmap _tmpPixmap );
    virtual void saveOasisPictureElement( KoGenStyle &styleobjectauto );
    void loadOasisPictureEffect(KoOasisContext & context );

    /**
     * @internal
     * Draws the shadow
     */
    void drawShadow( QPainter* _painter,  KoZoomHandler* _zoomHandler);

    QPixmap generatePixmap(KoZoomHandler*_zoomHandler);

    KoPictureCollection *imageCollection;
    KoPicture image;

    PictureMirrorType mirrorType, m_cachedMirrorType;
    int depth, m_cachedDepth;
    bool swapRGB, m_cachedSwapRGB;
    bool grayscal, m_cachedGrayscal;
    int bright, m_cachedBright;

    //image effect and its params
    ImageEffect m_effect, m_cachedEffect;
    QVariant m_ie_par1, m_cachedPar1;
    QVariant m_ie_par2, m_cachedPar2;
    QVariant m_ie_par3, m_cachedPar3;

    QPixmap m_cachedPixmap;
    QRect m_cachedRect;
};

#endif
