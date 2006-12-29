/* This file is part of the KDE project
   Copyright (C) 1998, 1999, 2000 Reginald Stadlbauer <reggie@kde.org>
   Copyright (C) 2005 Thomas Zander <zander@kde.org>

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

#ifndef frame_picture_frame_set_h
#define frame_picture_frame_set_h

#include "KWFrameSet.h"
#include <KoPicture.h>

class KWPictureFrameSet : public KWFrameSet
{
public:
    KWPictureFrameSet( KWDocument *_doc, const QString & name );
    /// Used for OASIS loading
    KWPictureFrameSet( KWDocument* doc, const QDomElement& frame, const QDomElement& imageTag, KoOasisContext& context );
    virtual ~KWPictureFrameSet();


    /**
     * The type of frameset. Use this to differentiate between different instantiations of
     *  the framesets. Each implementation will return a different frameType.
     */
    virtual FrameSetType type() const;

    void setPicture( const KoPicture &picture ) { m_picture = picture; }
    KoPicture picture() const { return m_picture; }

    KoPictureKey key() const { return m_picture.getKey(); }

    void loadPicture( const QString &fileName );
    void insertPicture( const KoPicture& picture );

    /**
     * Reload a picture, which already exists in the picture collection
     * (if not, it gives a defaulft KoPicture() )
     */
    void reloadPicture( const KoPictureKey& key );

    virtual QDomElement save( QDomElement &parentElem, bool saveFrames = true );
    virtual void load( QDomElement &attributes, bool loadFrames = true );
    virtual void saveOasis( KoXmlWriter& writer, KoSavingContext& context, bool saveFrames ) const;

    virtual void drawFrameContents( KWFrame * frame, QPainter *painter, const QRect & fcrect,
                                    const QColorGroup &cg, bool onlyChanged, bool resetChanged,
                                    KWFrameSetEdit * edit, KWViewMode *viewMode );

    /// Pixmaps can be transparent
    virtual void createEmptyRegion( const QRect &, QRegion &, KWViewMode * ) { }

#ifndef NDEBUG
    virtual void printDebug( KWFrame* frame );
#endif

    bool keepAspectRatio() const;
    void setKeepAspectRatio( bool b );

    /**
     * This method is part of the KWFrameSet API and is left empty here,
     * protect content is irrelevant for picture frame sets.
     */
    virtual void setProtectContent ( bool ) { };
    /**
     * This method is part of the KWFrameSet API and is left empty here,
     * protect content is irrelevant for picture frame sets.
     */
    virtual bool protectContent() const { return false; }

protected:
    void loadOasis( const QDomElement& frame, const QDomElement& imageTag, KoOasisContext& context );

    /// The picture
    KoPicture m_picture;
    bool m_keepAspectRatio;
    /// Cache the finalSize parameter of the method resizeFrame for drawFrame
    bool m_finalSize;
};

#endif
