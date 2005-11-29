// -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
/* This file is part of the KDE project
   Copyright (C) 2002 Laurent Montel <lmontel@mandrakesoft.com>

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

#ifndef kprstylemanager_h
#define kprstylemanager_h

#include <kdialogbase.h>
#include <qstringlist.h>
#include <koStylist.h>

class KPrDocument;
class KoParagStyle;
class KoUnit;

class KPrStyleManager : public KoStyleManager
{
    Q_OBJECT

public:
    KPrStyleManager( QWidget *_parent, KoUnit::Unit unit,KPrDocument *_doc,
                     const KoStyleCollection & styleCollection, const QString & activeStyleName );

    virtual KoParagStyle* addStyleTemplate(KoParagStyle *style);
    virtual void applyStyleChange( KoStyleChangeDefMap changed  );
    virtual void removeStyleTemplate( KoParagStyle *style );
    virtual void updateAllStyleLists();
    virtual void updateStyleListOrder( const QStringList & list);
protected:
    KPrDocument *m_doc;
};

#endif
