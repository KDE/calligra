/* This file is part of the KDE project
   Copyright (C) 2002 Nash Hoogwater <nrhoogwater@wanadoo.nl>
   Copyright (C) 2005 David Faure <faure@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; using
   version 2 of the License.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef kwtablestyle_h
#define kwtablestyle_h

#include "KWFrameStyle.h"
#include <KoParagStyle.h>

#include <qdom.h>
#include <qptrlist.h>
#include <qbrush.h>

class KWTableStyle;
class KWDocument;
class KWFrame;

/**
 * Table style (can created/modified/deleted by the user)
 */
class KWTableStyle : public KoUserStyle
{
public:
    /** Create a blank framestyle (with default attributes) */
    KWTableStyle( const QString & name, KoParagStyle * _style, KWFrameStyle * _frameStyle );

    KWTableStyle( QDomElement & parentElem, KWDocument *_doc, int docVersion=2 );

    /** Copy another framestyle */
    KWTableStyle( const KWTableStyle & rhs ) : KoUserStyle( QString::null ) { *this = rhs; }

    ~KWTableStyle() {}

    void operator=( const KWTableStyle & );

    // ATTRIBUTES
    KoParagStyle* paragraphStyle() const { return m_paragStyle; }
    void setParagraphStyle( KoParagStyle *paragStyle ) { m_paragStyle = paragStyle; }

    KWFrameStyle* frameStyle() const { return m_frameStyle; }
    void setFrameStyle( KWFrameStyle *frameStyle ) { m_frameStyle = frameStyle; }

    // SAVING METHODS
    void saveTableStyle( QDomElement & parentElem );

    // STATIC METHODS
    static KWTableStyle *loadStyle( QDomElement & parentElem, KWDocument *_doc, int docVersion=2 );

private:
    KoParagStyle *m_paragStyle;
    KWFrameStyle *m_frameStyle;
};

/**
 * Collection of user-defined table styles
 */
class KWTableStyleCollection : public KoUserStyleCollection
{
public:
    KWTableStyleCollection();

    QString defaultStyleName() const { return QString::fromLatin1( "Plain" ); }

    /**
     * Find style based on the untranslated name @p name.
     * Overloaded for convenience
     */
    KWTableStyle* findStyle( const QString & name ) const {
        return static_cast<KWTableStyle*>( KoUserStyleCollection::findStyle( name, defaultStyleName() ) );
    }

    /**
     * See KoUserStyleCollection::addStyle.
     * Overloaded for convenience.
     */
    KWTableStyle* addStyle( KWTableStyle* sty ) {
        return static_cast<KWTableStyle*>( KoUserStyleCollection::addStyle( sty ) );
    }

    /**
     * Return style number @p i.
     */
    KWTableStyle* tableStyleAt( int i ) const {
        return static_cast<KWTableStyle*>( m_styleList[i] );
    }

    void saveOasis( KoGenStyles& mainStyles, KoSavingContext& savingContext ) const;
    void loadOasisStyles( KoOasisContext& context );
};

#endif
