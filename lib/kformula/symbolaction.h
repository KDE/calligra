/* This file is part of the KDE project
   Copyright (C) 2002 Heinrich Kuettler <heinrich.kuettler@gmx.de>

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

#ifndef _SYMBOLACTION_H_
#define _SYMBOLACTION_H_

#include <kaction.h>
#include <qvaluelist.h>
#include <qstringlist.h>
#include <qmemarray.h>

#include "kformuladefs.h"

KFORMULA_NAMESPACE_BEGIN

class SymbolAction : public KSelectAction
{
public:
    SymbolAction( QObject* parent = 0, const char* name = 0 );
    SymbolAction( const QString& text, const KShortcut& cut,
                 const QObject* receiver, const char* slot, QObject* parent,
                 const char* name = 0 );

    int plug( QWidget*, int index = -1 );
    void setSymbols( const QStringList&, const QFont& font, const QMemArray<QChar>& );
    void updateItems( int );

private:
    QValueList<QFont> m_fonts;
    QMemArray<QChar> m_chars;
    QFont m_font;
};

KFORMULA_NAMESPACE_END

#endif // _SYMBOLACTION_H_
