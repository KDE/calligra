/* This file is part of the KDE project
   Copyright (C) 2002 Nash Hoogwater <nrhoogwater@wanadoo.nl>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef kwtablestyle_h
#define kwtablestyle_h

#include "kwframestyle.h"
#include "kwstyle.h"

#include <qdom.h>
#include <qptrlist.h>
#include <qbrush.h>

class KWTableStyle;
class KWDocument;
class KWFrame;

/******************************************************************/
/* Class: KWTableStyleCollection                                  */
/******************************************************************/

class KWTableStyleCollection
{
public:
    KWTableStyleCollection();
    ~KWTableStyleCollection();
    const QPtrList<KWTableStyle> & tableStyleList() const { return m_styleList; }
    void clear();

    /**
     * find table style based on the untranslated name @p name
     */
    KWTableStyle* findTableStyle( const QString & name );
    /**
     * find table style based on the translated name @p name
     */
    KWTableStyle* findTranslatedTableStyle( const QString & name );
    KWTableStyle* findStyleShortCut( const QString & _shortCut );

    /**
     * Return style number @p i.
     */
    KWTableStyle* tableStyleAt( int i ) { return m_styleList.at(i); }

    KWTableStyle* addTableStyleTemplate( KWTableStyle *style );

    void removeTableStyleTemplate ( KWTableStyle *style );
    void updateTableStyleListOrder( const QStringList &list );

private:
    QPtrList<KWTableStyle> m_styleList;
    QPtrList<KWTableStyle> m_deletedStyles;
    static int styleNumber;
    KWTableStyle *m_lastStyle;
};

/******************************************************************/
/* Class: KWTableStyle                                            */
/******************************************************************/

class KWTableStyle
{
public:
    /** Create a blank framestyle (with default attributes) */
    KWTableStyle( const QString & name, KWStyle * _style, KWFrameStyle * _frameStyle );

    KWTableStyle( QDomElement & parentElem, KWDocument *_doc, int docVersion=2 );

    /** Copy another framestyle */
    KWTableStyle( const KWTableStyle & rhs ) { *this = rhs; }

    ~KWTableStyle() {}

    void operator=( const KWTableStyle & );

    /** The internal name (untranslated if a standard style) */
    QString name() const { return m_name; }
    void setName( const QString & name ) { m_name = name; }
    /** The translated name */
    QString displayName() const;

    // ATTRIBUTES
    KWStyle style() const { return *m_style; }
    KWStyle* pStyle() const { return m_style; }
    void setStyle( KWStyle *_style ) { m_style = _style; }

    KWFrameStyle frameStyle() const { return *m_frameStyle; }
    KWFrameStyle* pFrameStyle() const { return m_frameStyle; }
    void setFrameStyle( KWFrameStyle *_frameStyle ) { m_frameStyle = _frameStyle; }

    // SAVING METHODS
    void saveTableStyle( QDomElement & parentElem );

    // STATIC METHODS
    static KWTableStyle *loadStyle( QDomElement & parentElem, KWDocument *_doc, int docVersion=2 );

    QString shortCutName() const {
        return m_shortCut_name;
    }

    void setShortCutName( const QString & _shortCut) {
        m_shortCut_name=_shortCut;
    }

private:
    QString m_name;
    QString m_shortCut_name;
    KWStyle *m_style;
    KWFrameStyle *m_frameStyle;
};

#endif
