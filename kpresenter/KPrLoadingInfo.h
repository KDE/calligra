/* This file is part of the KDE project
   Copyright (C) 2004 Laurent Montel <montel@kde.org>

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

#ifndef KPRLOADINGINFO_H
#define KPRLOADINGINFO_H

struct lstAnimation
{
    QDomElement *element;
    int order;
};

/// Temporary information used only during loading
class KPrLoadingInfo
{
public:
    KPrLoadingInfo( bool oldFormat = false) 
	: presSpeed( -1 )
	, m_header( false )
	, m_footer( false )
    , m_headerFooterByPage( false )
	, m_oldFormat( oldFormat )
    {
        m_animationsShowDict.setAutoDelete( true );
        m_animationsHideDict.setAutoDelete( true );
    }
    ~KPrLoadingInfo() {}

    lstAnimation* animationShowById( const QString& id ) const {
        return m_animationsShowDict[id]; // returns 0 if not found
    }
    void storePresentationShowAnimation( lstAnimation * element, const QString& id ) {
        m_animationsShowDict.insert( id , element );
    }
    void clearAnimationShowDict() {
        QDictIterator<lstAnimation> it( m_animationsShowDict ); // See QDictIterator
        for( ; it.current(); ++it )
        {
            delete it.current()->element;
        }
        m_animationsShowDict.clear();
    }

    lstAnimation* animationHideById( const QString& id ) const {
        return m_animationsHideDict[id]; // returns 0 if not found
    }
    void storePresentationHideAnimation( lstAnimation * element, const QString& id ) {
        m_animationsHideDict.insert( id , element );
    }
    void clearAnimationHideDict() {
        QDictIterator<lstAnimation> it( m_animationsHideDict ); // See QDictIterator
        for( ; it.current(); ++it )
        {
            delete it.current()->element;
        }
        m_animationsHideDict.clear();
    }
    bool oldFormat() const { return m_oldFormat; }
    int presSpeed;
    QMap<QString, KPrPage *> m_name2page;
    bool m_header;
    bool m_footer;
    bool m_headerFooterByPage;
private:
    QDict<lstAnimation> m_animationsShowDict;
    QDict<lstAnimation> m_animationsHideDict;
    bool m_oldFormat;
};

#endif /* KPRLOADINGINFO_H */

