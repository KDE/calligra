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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef KWSTYLEMANAGER_H
#define KWSTYLEMANAGER_H

#include <KoStyleManager.h>

class KWDocument;
class KoParagStyle;
class KoUnit;
/******************************************************************/
/* Class: KWStyleManager                                          */
/******************************************************************/
class KWStyleManager : public KoStyleManager
{
    Q_OBJECT

public:
    KWStyleManager( QWidget *_parent, KoUnit::Unit unit, KWDocument *_doc,
                    const KoStyleCollection& styleCollection,
                    const QString & activeStyleName = QString::null );

    virtual KoParagStyle* addStyleTemplate(KoParagStyle *style);
    virtual void applyStyleChange( KoStyleChangeDefMap changed  );
    virtual void removeStyleTemplate( KoParagStyle *style );
    virtual void updateAllStyleLists();
    virtual void updateStyleListOrder( const QStringList & list);
protected:
    KWDocument *m_doc;
};

#endif
