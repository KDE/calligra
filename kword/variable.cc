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

#include "variable.h"
#include <koVariable.h>
#include <koUtils.h>
#include "serialletter.h"
#include "kwdoc.h"

#include <klocale.h>
#include <kdebug.h>

/******************************************************************/
/* Class: KWPgNumVariable                                         */
/******************************************************************/
KWPgNumVariable::KWPgNumVariable( KoTextDocument *textdoc, int subtype, KoVariableFormat *varFormat ,KoVariableCollection *_varColl, KWDocument *doc  )
    : KoPgNumVariable( textdoc, subtype, varFormat ,_varColl ),m_doc(doc)
{
}

void KWPgNumVariable::recalc()
{
    if ( m_subtype == VST_PGNUM_CURRENT )
    {
#if 0 // Made obsolete by the (more dynamic) code in drawFrame.
        KWTextParag * parag = static_cast<KWTextParag *>( paragraph() );
        if ( !parag ) // too early
            return;
        KWTextFrameSet * fs = parag->kwTextDocument()->textFrameSet();
        QPoint iPoint = parag->rect().topLeft(); // small bug if a paragraph is cut between two pages.
        QPoint cPoint;
        KWFrame * frame = fs->internalToNormal( iPoint, cPoint );
        if ( frame )
            m_pgNum = frame->pageNum() + 1;
#endif
    }
    else
        m_pgNum = m_doc->getPages();
    resize();
}

QString KWPgNumVariable::text()
{
    KoVariableNumberFormat * format = dynamic_cast<KoVariableNumberFormat *>( m_varFormat );
    Q_ASSERT( format );
    if ( format )
        return format->convert( m_pgNum );
    // make gcc happy
    return QString::null;
}

/******************************************************************/
/* Class: KWSerialLetterVariable                                  */
/******************************************************************/
KWSerialLetterVariable::KWSerialLetterVariable( KoTextDocument *textdoc, const QString &name, KoVariableFormat *varFormat,KoVariableCollection *_varColl, KWDocument *doc  )
    : KoSerialLetterVariable( textdoc, name, varFormat,_varColl ), m_doc(doc)
{
}


QString KWSerialLetterVariable::value() const
{
    return m_doc->getSerialLetterDataBase()->getValue( m_name );
}

QString KWSerialLetterVariable::text()
{
    // ## should use a format maybe
    QString v = value();
    if ( v == name() )
        return "<" + v + ">";
    return v;
}

void KWSerialLetterVariable::recalc()
{
    resize();
}
