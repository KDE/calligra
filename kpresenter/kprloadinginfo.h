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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef KPRLOADINGINFO_H
#define KPRLOADINGINFO_H

struct lstAnimation
{
    QDomElement *element;
    int order;
};

/// Temporary information used only during loading
class KPRLoadingInfo
{
public:
    KPRLoadingInfo() { presSpeed = -1; }
    ~KPRLoadingInfo() {}

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

    int presSpeed;
    CustomListMap m_tmpCustomListMap;
private:
    QDict<lstAnimation> m_animationsShowDict;
    QDict<lstAnimation> m_animationsHideDict;
};

#endif /* KPRLOADINGINFO_H */

