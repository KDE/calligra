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

/// Temporary information used only during loading
class KPRLoadingInfo
{
public:
    KPRLoadingInfo() {}
    ~KPRLoadingInfo() {}

    QDomElement* animationById( const QString& id ) const {
        return m_animationsDict[id]; // returns 0 if not found
    }
    void storePresentationAnimation( QDomElement * element, const QString& id ) {
        m_animationsDict.insert( id , element );
    }
    void clearAnimationDict() {
      m_animationsDict.clear();
    }
    void clearStyleStack() {
      m_styleStack.clear();
    }
    void saveStyleStack() {
      m_styleStack.save();
    }
    void restoreStyleStack() {
      m_styleStack.restore();
    }
    bool styleStackHasAttribute( const QString & attr ) const {
      return m_styleStack.hasAttribute( attr );
    }
    void styleStackPush(const QDomElement & style) {
      m_styleStack.push(style);
    }
    KoStyleStack styleStack() const {
      return m_styleStack;
    }
private:
    QDict<QDomElement> m_animationsDict; 
    KoStyleStack m_styleStack;
};

#endif /* KPRLOADINGINFO_H */

