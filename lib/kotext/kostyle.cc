/* This file is part of the KDE project
   Copyright (C) 2001 David Faure <faure@kde.org>

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

#include "kostyle.h"
#include "kozoomhandler.h"
#include <qdom.h>
#include <kdebug.h>
#include <klocale.h>

KoStyle::KoStyle( const QString & name )
{
    m_name = name;
    m_followingStyle = this;
    m_format.setColor( Qt::black ); // ### why ? what about other color schemes ?

    // This way, KWTextParag::setParagLayout also sets the style pointer, to this style
    m_paragLayout.style = this;
}

void KoStyle::operator=( const KoStyle &rhs )
{
    m_paragLayout = rhs.m_paragLayout;
    m_name = rhs.m_name;
    m_format = rhs.m_format;
    m_followingStyle = rhs.m_followingStyle;
    m_paragLayout.style = this; // must always be "this"
}

QString KoStyle::translatedName() const
{
    return i18n( "Style name", name().utf8() );
}
